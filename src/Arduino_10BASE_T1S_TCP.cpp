/**
 * @file Arduino_10BASE_T1S_TCP.cpp
 * @brief lwIP raw-TCP implementation for Arduino_10BASE_T1S_TCPClient/Server.
 */

#include "Arduino_10BASE_T1S_TCP.h"
#include <string.h>

/* ==========================================================================
 * Connection pool
 * ======================================================================= */
T1STCPState g_t1s_tcp_pool[T1S_TCP_MAX_CONNS];

T1STCPState *t1s_tcp_alloc() {
    for (int i = 0; i < T1S_TCP_MAX_CONNS; ++i) {
        if (!g_t1s_tcp_pool[i].active) {
            memset(&g_t1s_tcp_pool[i], 0, sizeof(T1STCPState));
            g_t1s_tcp_pool[i].active = true;
            return &g_t1s_tcp_pool[i];
        }
    }
    return nullptr;
}

void t1s_tcp_free(T1STCPState *s) {
    if (s) {
        s->active         = false;
        s->connected      = false;
        s->pending_accept = false;
        s->pcb            = nullptr;
        s->rxhead         = 0;
        s->rxtail         = 0;
    }
}

/* --------------------------------------------------------------------------
 * Ring buffer helpers
 * ----------------------------------------------------------------------- */
static inline uint16_t rb_avail(const T1STCPState *s) {
    return (uint16_t)((s->rxtail - s->rxhead + T1S_TCP_RX_BUF_SIZE)
                       % T1S_TCP_RX_BUF_SIZE);
}
static inline uint16_t rb_free(const T1STCPState *s) {
    return (uint16_t)(T1S_TCP_RX_BUF_SIZE - 1 - rb_avail(s));
}
static inline void rb_push(T1STCPState *s, uint8_t b) {
    s->rxbuf[s->rxtail] = b;
    s->rxtail = (s->rxtail + 1) % T1S_TCP_RX_BUF_SIZE;
}
static inline uint8_t rb_pop(T1STCPState *s) {
    uint8_t b = s->rxbuf[s->rxhead];
    s->rxhead = (s->rxhead + 1) % T1S_TCP_RX_BUF_SIZE;
    return b;
}
static inline uint8_t rb_peek(const T1STCPState *s) {
    return s->rxbuf[s->rxhead];
}

/* ==========================================================================
 * Shared lwIP callbacks
 * ======================================================================= */

/* recv — called whenever data or EOF arrives */
static err_t s_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    T1STCPState *s = reinterpret_cast<T1STCPState*>(arg);
    if (!s) return ERR_VAL;

    if (!p) {
        /* Remote closed the connection */
        s->connected = false;
        tcp_close(tpcb);
        s->pcb = nullptr;
        return ERR_OK;
    }
    if (err != ERR_OK) {
        pbuf_free(p);
        return err;
    }

    /* Acknowledge bytes to lwIP */
    tcp_recved(tpcb, p->tot_len);

    /* Walk pbuf chain and push bytes into ring buffer */
    for (struct pbuf *q = p; q; q = q->next) {
        const uint8_t *d = reinterpret_cast<const uint8_t*>(q->payload);
        uint16_t n = q->len;
        for (uint16_t i = 0; i < n; ++i) {
            if (rb_free(s) == 0) {
                /* RX buffer full — drop remaining bytes */
                Serial.println("[T1S TCP] RX buffer full, bytes dropped");
                break;
            }
            rb_push(s, d[i]);
        }
    }
    pbuf_free(p);
    return ERR_OK;
}

/* err — called on fatal errors (pcb already freed by lwIP) */
static void s_tcp_err(void *arg, err_t err)
{
    (void)err;
    T1STCPState *s = reinterpret_cast<T1STCPState*>(arg);
    if (s) {
        s->connected = false;
        s->pcb       = nullptr;
    }
}

/* connected — called when an outgoing tcp_connect() succeeds */
static err_t s_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    (void)tpcb;
    Arduino_10BASE_T1S_TCPClient *client =
        reinterpret_cast<Arduino_10BASE_T1S_TCPClient*>(arg);
    if (client)
        client->_setConnectResult(err == ERR_OK);
    return ERR_OK;
}

/* ==========================================================================
 * Arduino_10BASE_T1S_TCPClient
 * ======================================================================= */

Arduino_10BASE_T1S_TCPClient::Arduino_10BASE_T1S_TCPClient()
    : _state(nullptr), _connect_done(false), _connect_ok(false) {}

Arduino_10BASE_T1S_TCPClient::Arduino_10BASE_T1S_TCPClient(T1STCPState *state)
    : _state(state), _connect_done(false), _connect_ok(false) {}

void Arduino_10BASE_T1S_TCPClient::_setConnectResult(bool ok) {
    _connect_ok   = ok;
    _connect_done = true;
    if (_state) _state->connected = ok;
}

/* connect() — outgoing TCP connection (blocking, 5 s timeout).
 * Requires t1s_phy.service() to be running (via interrupts) while waiting. */
int Arduino_10BASE_T1S_TCPClient::connect(IPAddress ip, uint16_t port)
{
    stop(); /* release any existing connection */

    _state = t1s_tcp_alloc();
    if (!_state) {
        Serial.println("[T1S TCP] connect: no free connection slots");
        return 0;
    }

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        t1s_tcp_free(_state); _state = nullptr;
        return 0;
    }

    _state->pcb       = pcb;
    _state->connected = false;
    _connect_done     = false;
    _connect_ok       = false;

    tcp_arg(pcb, this);     /* pass client* so s_tcp_connected can call back */
    tcp_recv(pcb, s_tcp_recv);
    tcp_err(pcb,  s_tcp_err);

    ip_addr_t addr;
    IP4_ADDR(&addr, ip[0], ip[1], ip[2], ip[3]);

    err_t err = tcp_connect(pcb, &addr, port, s_tcp_connected);
    if (err != ERR_OK) {
        tcp_abort(pcb);
        t1s_tcp_free(_state); _state = nullptr;
        return 0;
    }

    /* Spin until connected or 5 s timeout.
     * lwIP progresses through interrupts; sys_check_timeouts drives retransmits. */
    unsigned long deadline = millis() + 5000UL;
    while (!_connect_done && (long)(millis() - deadline) < 0) {
        sys_check_timeouts();
        delay(1);
    }

    /* After connect, switch the arg to state* so s_tcp_recv / s_tcp_err work */
    if (_connect_ok) {
        tcp_arg(pcb, _state);
        return 1;
    }
    t1s_tcp_free(_state); _state = nullptr;
    return 0;
}

int Arduino_10BASE_T1S_TCPClient::connect(const char * /*host*/, uint16_t /*port*/)
{
    /* DNS resolution not implemented — use IP-address overload */
    Serial.println("[T1S TCP] connect(host): DNS not supported; use IPAddress");
    return 0;
}

size_t Arduino_10BASE_T1S_TCPClient::write(uint8_t b) {
    return write(&b, 1);
}

size_t Arduino_10BASE_T1S_TCPClient::write(const uint8_t *buf, size_t size)
{
    if (!_state || !_state->connected || !_state->pcb) return 0;
    if (tcp_sndbuf(_state->pcb) < size) size = tcp_sndbuf(_state->pcb);
    if (size == 0) return 0;
    err_t err = tcp_write(_state->pcb, buf, (u16_t)size, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) return 0;
    return size;
}

int Arduino_10BASE_T1S_TCPClient::available() {
    if (!_state) return 0;
    return (int)rb_avail(_state);
}

int Arduino_10BASE_T1S_TCPClient::read() {
    if (!_state || rb_avail(_state) == 0) return -1;
    return (int)rb_pop(_state);
}

int Arduino_10BASE_T1S_TCPClient::read(uint8_t *buf, size_t size)
{
    if (!_state) return 0;
    uint16_t n = (uint16_t)size;
    if (n > rb_avail(_state)) n = rb_avail(_state);
    for (uint16_t i = 0; i < n; ++i)
        buf[i] = rb_pop(_state);
    return (int)n;
}

int Arduino_10BASE_T1S_TCPClient::peek() {
    if (!_state || rb_avail(_state) == 0) return -1;
    return (int)rb_peek(_state);
}

void Arduino_10BASE_T1S_TCPClient::flush() {
    if (_state && _state->pcb)
        tcp_output(_state->pcb);
}

void Arduino_10BASE_T1S_TCPClient::stop()
{
    if (!_state) return;
    if (_state->pcb) {
        tcp_arg(_state->pcb,  nullptr);
        tcp_recv(_state->pcb, nullptr);
        tcp_err(_state->pcb,  nullptr);
        tcp_close(_state->pcb);
        _state->pcb = nullptr;
    }
    t1s_tcp_free(_state);
    _state = nullptr;
}

uint8_t Arduino_10BASE_T1S_TCPClient::connected() {
    if (!_state) return 0;
    /* Still connected, or connected but data unread */
    return (_state->connected || rb_avail(_state) > 0) ? 1 : 0;
}

Arduino_10BASE_T1S_TCPClient::operator bool() {
    return _state != nullptr && _state->active;
}

/* ==========================================================================
 * Arduino_10BASE_T1S_TCPServer — lwIP accept shim
 * ======================================================================= */
static err_t s_tcp_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    return reinterpret_cast<Arduino_10BASE_T1S_TCPServer*>(arg)
               ->_onAccept(newpcb, err);
}

/* Incoming connection recv/err: these are set on the accepted PCB only.
 * We reuse the same s_tcp_recv / s_tcp_err with T1STCPState* as arg. */

Arduino_10BASE_T1S_TCPServer::Arduino_10BASE_T1S_TCPServer(uint16_t port)
    : _port(port), _listen_pcb(nullptr) {}

void Arduino_10BASE_T1S_TCPServer::begin()
{
    if (_listen_pcb) return; /* already started */

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        Serial.println("[T1S TCP] Server::begin tcp_new failed");
        return;
    }
    if (tcp_bind(pcb, IP_ADDR_ANY, _port) != ERR_OK) {
        Serial.printf("[T1S TCP] Server::begin bind port %u failed\n", _port);
        tcp_close(pcb);
        return;
    }
    _listen_pcb = tcp_listen(pcb);
    if (!_listen_pcb) {
        Serial.println("[T1S TCP] Server::begin tcp_listen failed");
        tcp_close(pcb);
        return;
    }
    tcp_arg(_listen_pcb, this);
    tcp_accept(_listen_pcb, s_tcp_accept);
    Serial.printf("[T1S TCP] Server listening on port %u\n", _port);
}

err_t Arduino_10BASE_T1S_TCPServer::_onAccept(struct tcp_pcb *newpcb, err_t err)
{
    if (err != ERR_OK || !newpcb) return ERR_VAL;

    T1STCPState *s = t1s_tcp_alloc();
    if (!s) {
        Serial.println("[T1S TCP] Accept: no free slots, rejecting client");
        tcp_abort(newpcb);
        return ERR_ABRT;
    }

    s->pcb            = newpcb;
    s->connected      = true;
    s->pending_accept = true;

    tcp_arg(newpcb,  s);
    tcp_recv(newpcb, s_tcp_recv);
    tcp_err(newpcb,  s_tcp_err);
    tcp_nagle_disable(newpcb);

    Serial.println("[T1S TCP] Client accepted");
    return ERR_OK;
}

Arduino_10BASE_T1S_TCPClient Arduino_10BASE_T1S_TCPServer::available()
{
    for (int i = 0; i < T1S_TCP_MAX_CONNS; ++i) {
        T1STCPState *s = &g_t1s_tcp_pool[i];
        if (s->active && s->pending_accept) {
            s->pending_accept = false;
            return Arduino_10BASE_T1S_TCPClient(s);
        }
    }
    /* Also return existing connected clients that have data waiting */
    for (int i = 0; i < T1S_TCP_MAX_CONNS; ++i) {
        T1STCPState *s = &g_t1s_tcp_pool[i];
        if (s->active && s->connected && rb_avail(s) > 0)
            return Arduino_10BASE_T1S_TCPClient(s);
    }
    return Arduino_10BASE_T1S_TCPClient(nullptr);
}

/* broadcast write to all active server-side connections */
size_t Arduino_10BASE_T1S_TCPServer::write(uint8_t b) {
    return write(&b, 1);
}
size_t Arduino_10BASE_T1S_TCPServer::write(const uint8_t *buf, size_t size)
{
    size_t sent = 0;
    for (int i = 0; i < T1S_TCP_MAX_CONNS; ++i) {
        T1STCPState *s = &g_t1s_tcp_pool[i];
        if (s->active && s->connected && s->pcb) {
            if (tcp_sndbuf(s->pcb) >= size) {
                if (tcp_write(s->pcb, buf, (u16_t)size,
                              TCP_WRITE_FLAG_COPY) == ERR_OK)
                    sent = size;
            }
        }
    }
    return sent;
}
