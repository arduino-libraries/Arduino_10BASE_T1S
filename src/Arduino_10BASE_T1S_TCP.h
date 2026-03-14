/**
 * @file Arduino_10BASE_T1S_TCP.h
 * @brief Arduino TCP Client and Server classes built on lwIP raw TCP for
 *        the 10BASE-T1S (LAN865x) library.
 *
 * Provides:
 *   Arduino_10BASE_T1S_TCPClient  — subclasses Arduino Client
 *   Arduino_10BASE_T1S_TCPServer  — subclasses Arduino Server
 *
 * Both classes use a shared pool of T1STCPState connection objects backed by
 * lwIP raw-TCP callbacks.  
 *
 * Tunable constants (override before inclusion if needed):
 *   T1S_TCP_MAX_CONNS    – simultaneous connections (default 8)
 *   T1S_TCP_RX_BUF_SIZE  – per-connection receive ring buffer (default 512)
 */

#pragma once

#include <Arduino.h>
#include <Client.h>
#include <Server.h>

/* lwIP headers — conditionalized for RP2040 vs. bundled liblwip */
#if defined(ARDUINO_ARCH_RP2040)
#  include <lwip/tcp.h>
#  include <lwip/pbuf.h>
#  include <lwip/ip_addr.h>
#  include <lwip/timeouts.h>
#else
#  include "lib/liblwip/include/lwip/tcp.h"
#  include "lib/liblwip/include/lwip/pbuf.h"
#  include "lib/liblwip/include/lwip/ip_addr.h"
#  include "lib/liblwip/include/lwip/timeouts.h"
#endif

/* -----------------------------------------------------------------------
 * Tuneable limits
 * -------------------------------------------------------------------- */
#ifndef T1S_TCP_MAX_CONNS
#  define T1S_TCP_MAX_CONNS    8
#endif
#ifndef T1S_TCP_RX_BUF_SIZE
#  define T1S_TCP_RX_BUF_SIZE  512
#endif

/* -----------------------------------------------------------------------
 * Shared connection state (heap-free, pool-allocated)
 * -------------------------------------------------------------------- */
struct T1STCPState {
    struct tcp_pcb *pcb;
    uint8_t  rxbuf[T1S_TCP_RX_BUF_SIZE];
    uint16_t rxhead;          /* ring read  index */
    uint16_t rxtail;          /* ring write index */
    bool     active;          /* slot occupied     */
    bool     connected;       /* TCP link is up    */
    bool     pending_accept;  /* waiting for TCPServer::available() */
};

/* global pool — defined in the .cpp */
extern T1STCPState g_t1s_tcp_pool[T1S_TCP_MAX_CONNS];

T1STCPState *t1s_tcp_alloc();
void         t1s_tcp_free(T1STCPState *s);

/* -----------------------------------------------------------------------
 * Arduino_10BASE_T1S_TCPClient
 * -------------------------------------------------------------------- */
class Arduino_10BASE_T1S_TCPClient : public Client {
public:
    Arduino_10BASE_T1S_TCPClient();
    explicit Arduino_10BASE_T1S_TCPClient(T1STCPState *state);

    /* Client interface */
    int     connect(IPAddress ip, uint16_t port) override;
    int     connect(const char *host, uint16_t port) override;
    size_t  write(uint8_t b)                             override;
    size_t  write(const uint8_t *buf, size_t size)       override;
    int     available()                                  override;
    int     read()                                       override;
    int     read(uint8_t *buf, size_t size)              override;
    int     peek()                                       override;
    void    flush()                                      override;
    void    stop()                                       override;
    uint8_t connected()                                  override;
    operator bool()                                      override;

    /* Internal — used by lwIP connect callback */
    void _setConnectResult(bool ok);

private:
    T1STCPState *_state;
    volatile bool _connect_done;
    volatile bool _connect_ok;
};

/* -----------------------------------------------------------------------
 * Arduino_10BASE_T1S_TCPServer
 * -------------------------------------------------------------------- */
class Arduino_10BASE_T1S_TCPServer : public Server {
public:
    explicit Arduino_10BASE_T1S_TCPServer(uint16_t port);

    /* Server interface */
    void   begin()                                 override;
    size_t write(uint8_t b)                        override;
    size_t write(const uint8_t *buf, size_t size)  override;

    /** Returns the next pending client; operator bool() == false if none. */
    Arduino_10BASE_T1S_TCPClient available();

    /* Internal — lwIP accept callback */
    err_t _onAccept(struct tcp_pcb *newpcb, err_t err);

private:
    uint16_t        _port;
    struct tcp_pcb *_listen_pcb;
};
