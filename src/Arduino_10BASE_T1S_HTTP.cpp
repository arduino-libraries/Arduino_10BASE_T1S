/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**************************************************************************************
 * INCLUDES
 **************************************************************************************/

#include "Arduino_10BASE_T1S_HTTP.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**************************************************************************************
 * INTERNAL HELPERS
 **************************************************************************************/

/** Safely find the first occurrence of needle in the first haystack_len bytes. */
static const char * mem_find(const char *haystack, uint16_t haystack_len,
                             const char *needle,   uint16_t needle_len)
{
  if (needle_len == 0 || haystack_len < needle_len) return nullptr;
  for (uint16_t i = 0; i <= (uint16_t)(haystack_len - needle_len); i++) {
    if (memcmp(haystack + i, needle, needle_len) == 0)
      return haystack + i;
  }
  return nullptr;
}

static bool starts_with_ci(const char *s, const char *prefix)
{
  while (*prefix && *s) {
    if ((char)tolower((unsigned char)*s) != (char)tolower((unsigned char)*prefix)) {
      return false;
    }
    s++;
    prefix++;
  }
  return *prefix == '\0';
}

static uint32_t parse_content_length(const char *headers)
{
  const char *line = headers;
  while (line && *line) {
    const char *line_end = strstr(line, "\r\n");
    if (!line_end) {
      line_end = strchr(line, '\n');
    }
    if (!line_end) {
      line_end = line + strlen(line);
    }

    if (starts_with_ci(line, "Content-Length:")) {
      const char *v = line + 15;
      while (*v == ' ' || *v == '\t') {
        v++;
      }
      return (uint32_t)strtoul(v, nullptr, 10);
    }

    if (*line_end == '\0') {
      break;
    }
    line = line_end + 1;
    if (*line == '\n') {
      line++;
    }
  }
  return 0;
}

/**************************************************************************************
 * CTOR / DTOR
 **************************************************************************************/

Arduino_10BASE_T1S_HTTP::Arduino_10BASE_T1S_HTTP(uint16_t port)
  : _port(port)
  , _listen_pcb(nullptr)
  , _route_count(0)
  , _upload_route_count(0)
  , _not_found_handler(nullptr)
{
  memset(_routes, 0, sizeof(_routes));
  memset(_upload_routes, 0, sizeof(_upload_routes));
}

Arduino_10BASE_T1S_HTTP::~Arduino_10BASE_T1S_HTTP()
{
  stop();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

bool Arduino_10BASE_T1S_HTTP::begin()
{
  if (_listen_pcb) {
    /* Already running — stop and restart cleanly. */
    stop();
  }

  struct tcp_pcb *pcb = tcp_new();
  if (!pcb) {
    Serial.println("[HTTP] tcp_new failed");
    return false;
  }

  /* Allow quick re-bind after a reset. */
  ip_set_option(pcb, SOF_REUSEADDR);

  err_t err = tcp_bind(pcb, IP_ADDR_ANY, _port);
  if (err != ERR_OK) {
    Serial.print("[HTTP] tcp_bind failed: ");
    Serial.println(err);
    tcp_close(pcb);
    return false;
  }

  struct tcp_pcb *lpcb = tcp_listen(pcb);
  if (!lpcb) {
    Serial.println("[HTTP] tcp_listen failed");
    tcp_close(pcb);
    return false;
  }

  _listen_pcb = lpcb;

  /* Pass 'this' as argument so the static callbacks can reach the instance. */
  tcp_arg(_listen_pcb, this);
  tcp_accept(_listen_pcb, &Arduino_10BASE_T1S_HTTP::onAccept);

  Serial.print("[HTTP] Listening on port ");
  Serial.println(_port);
  return true;
}

void Arduino_10BASE_T1S_HTTP::stop()
{
  if (_listen_pcb) {
    tcp_arg(_listen_pcb, nullptr);
    tcp_accept(_listen_pcb, nullptr);
    tcp_close(_listen_pcb);
    _listen_pcb = nullptr;
    Serial.println("[HTTP] Server stopped");
  }
}

bool Arduino_10BASE_T1S_HTTP::on(const char *path, RouteHandler handler)
{
  if (_route_count >= HTTP_SERVER_MAX_ROUTES) {
    Serial.println("[HTTP] Route table full");
    return false;
  }
  _routes[_route_count].path    = path;
  _routes[_route_count].handler = handler;
  _route_count++;
  return true;
}

bool Arduino_10BASE_T1S_HTTP::onUpload(const char *path, UploadHandler handler)
{
  if (_upload_route_count >= HTTP_SERVER_MAX_UPLOAD_ROUTES) {
    Serial.println("[HTTP] Upload route table full");
    return false;
  }

  _upload_routes[_upload_route_count].path = path;
  _upload_routes[_upload_route_count].handler = handler;
  _upload_route_count++;
  return true;
}

void Arduino_10BASE_T1S_HTTP::setNotFoundHandler(RouteHandler handler)
{
  _not_found_handler = handler;
}

/**************************************************************************************
 * PRIVATE HELPERS
 **************************************************************************************/

Arduino_10BASE_T1S_HTTP::RouteHandler
Arduino_10BASE_T1S_HTTP::findHandler(const char *path) const
{
  for (uint8_t i = 0; i < _route_count; i++) {
    if (_routes[i].path && strcmp(_routes[i].path, path) == 0)
      return _routes[i].handler;
  }
  return nullptr;
}

Arduino_10BASE_T1S_HTTP::UploadHandler
Arduino_10BASE_T1S_HTTP::findUploadHandler(const char *path) const
{
  for (uint8_t i = 0; i < _upload_route_count; i++) {
    if (_upload_routes[i].path && strcmp(_upload_routes[i].path, path) == 0) {
      return _upload_routes[i].handler;
    }
  }
  return nullptr;
}

/* ---- Parse and dispatch ------------------------------------------------ */

void Arduino_10BASE_T1S_HTTP::handleRequest(struct tcp_pcb *tpcb, ConnState *state)
{
  /* ---- Status line / body buffer ---- */
  static char resp_body[HTTP_SERVER_RESP_BUF_SIZE];
  resp_body[0] = '\0';

  RouteHandler handler = findHandler(state->path);
  uint16_t     status_code  = 200;
  const char  *status_text  = "OK";

  if (handler) {
    status_code = handler(state->method, state->path, state->query,
                          resp_body, sizeof(resp_body));
    switch (status_code) {
      case 200: status_text = "OK";                    break;
      case 201: status_text = "Created";               break;
      case 204: status_text = "No Content";            break;
      case 400: status_text = "Bad Request";           break;
      case 403: status_text = "Forbidden";             break;
      case 404: status_text = "Not Found";             break;
      case 500: status_text = "Internal Server Error"; break;
      default:  status_text = "OK";                    break;
    }
  } else if (_not_found_handler) {
    status_code = _not_found_handler(state->method, state->path, state->query,
                                     resp_body, sizeof(resp_body));
    status_text = "Not Found";
  } else {
    status_code = 404;
    status_text = "Not Found";
    snprintf(resp_body, sizeof(resp_body),
             "<html><body><h2>404 Not Found</h2><p>%s</p></body></html>",
             state->path);
  }

  sendResponse(tpcb, status_code, status_text, resp_body);
  /* tcp_shutdown(tx) is called inside sendResponse — nothing else needed here. */
}

/* ---- Build and send the HTTP response ---------------------------------- */

void Arduino_10BASE_T1S_HTTP::sendResponse(struct tcp_pcb *tpcb,
                                           uint16_t        status_code,
                                           const char     *status_text,
                                           const char     *body)
{
  static char header_buf[256];
  size_t body_len = strlen(body);

  int hdr_len = snprintf(header_buf, sizeof(header_buf),
    "HTTP/1.0 %u %s\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Content-Length: %u\r\n"
    "Connection: close\r\n"
    "Cache-Control: no-store\r\n"
    "\r\n",
    (unsigned)status_code, status_text,
    (unsigned)body_len);

  if (hdr_len <= 0 || (size_t)hdr_len >= sizeof(header_buf)) return;

  tcp_write(tpcb, header_buf, (u16_t)hdr_len, TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE);
  if (body_len > 0) {
    tcp_write(tpcb, body, (u16_t)body_len, TCP_WRITE_FLAG_COPY);
  }
  tcp_output(tpcb);

  /* Half-close the TX direction after all data is queued.
   * lwIP sends the FIN only after every queued byte has been transmitted
   * and ACKed, so the response is guaranteed to be fully delivered before
   * the connection tears down.  The browser closing its own side triggers
   * onRecv(p==NULL) which calls closeConn() to free the PCB. */
  tcp_shutdown(tpcb, 0 /* !shut_rx */, 1 /* shut_tx */);
}

/* ---- Close a connection cleanly --------------------------------------- */

void Arduino_10BASE_T1S_HTTP::closeConn(struct tcp_pcb *tpcb, ConnState *state)
{
  tcp_arg (tpcb, nullptr);
  tcp_recv(tpcb, nullptr);
  tcp_sent(tpcb, nullptr);
  tcp_err (tpcb, nullptr);
  tcp_poll(tpcb, nullptr, 0);
  if (state) delete state;
  tcp_close(tpcb);
}

/**************************************************************************************
 * LWIP CALLBACKS (static)
 **************************************************************************************/

/* Called when a new client connects to the listening socket. */
err_t Arduino_10BASE_T1S_HTTP::onAccept(void *arg,
                                        struct tcp_pcb *new_pcb,
                                        err_t err)
{
  if (err != ERR_OK || !new_pcb) return ERR_VAL;

  Arduino_10BASE_T1S_HTTP *self = static_cast<Arduino_10BASE_T1S_HTTP*>(arg);

  /* Allocate per-connection state. */
  ConnState *state = new ConnState();
  if (!state) {
    tcp_abort(new_pcb);
    return ERR_MEM;
  }
  memset(state, 0, sizeof(ConnState));
  state->server = self;
  state->upload_mode = false;
  state->upload_started = false;
  state->content_length = 0;
  state->body_received = 0;
  state->upload_handler = nullptr;

  /* Set priority lower than the listening PCB to avoid starving new accepts. */
  tcp_setprio(new_pcb, TCP_PRIO_MIN);

  tcp_arg (new_pcb, state);
  tcp_recv(new_pcb, &Arduino_10BASE_T1S_HTTP::onRecv);
  tcp_sent(new_pcb, &Arduino_10BASE_T1S_HTTP::onSent);
  tcp_err (new_pcb, &Arduino_10BASE_T1S_HTTP::onError);
  /* Poll every 12 s (24 × 500 ms ticks).
   * 10BASE-T1S over ICS/NAT can be slow to deliver the HTTP GET segment;
   * 2 s was too short and killed valid connections before data arrived. */
  tcp_poll(new_pcb, &Arduino_10BASE_T1S_HTTP::onPoll, 24);

  Serial.println("[HTTP] Connection accepted");
  return ERR_OK;
}

/* Called when data arrives on a connection. */
err_t Arduino_10BASE_T1S_HTTP::onRecv(void *arg,
                                      struct tcp_pcb *tpcb,
                                      struct pbuf *p,
                                      err_t err)
{
  ConnState *state = static_cast<ConnState*>(arg);

  if (!state) {
    if (p) pbuf_free(p);
    return ERR_OK;
  }

  /* p == NULL: peer closed its write side (sent FIN) — we close our side too. */
  if (!p) {
    if (state->upload_mode && state->upload_started && state->upload_handler) {
      Arduino_10BASE_T1S_HTTP::UploadInfo info;
      info.buf         = nullptr;
      info.currentSize = 0;
      info.totalSize   = state->body_received;
      info.contentLength = state->content_length;
      info.path  = state->path;
      info.query = state->query;

      if (state->content_length == 0 && state->body_received > 0) {
        /* Upload with unknown Content-Length: treat peer FIN as end of body. */
        info.status = UPLOAD_FILE_END;
        state->upload_handler(info);
        state->server->handleRequest(tpcb, state); /* send OTA result page */
        return ERR_OK; /* closeConn called after browser ACKs our response */
      } else if (state->body_received < state->content_length) {
        /* Known length but connection closed early — abort. */
        info.status = UPLOAD_FILE_ABORTED;
        state->upload_handler(info);
      }
    }
    Serial.println("[HTTP] Peer closed — closing connection");
    closeConn(tpcb, state);
    return ERR_OK;
  }

  if (err != ERR_OK) {
    pbuf_free(p);
    closeConn(tpcb, state);
    return err;
  }

  Serial.print("[HTTP] RX data bytes="); Serial.println(p->tot_len);

  /* Acknowledge receipt immediately. */
  tcp_recved(tpcb, p->tot_len);

  if (state->headers_done && state->upload_mode && state->upload_handler) {
    struct pbuf *q = p;
    while (q != nullptr) {
      Arduino_10BASE_T1S_HTTP::UploadInfo info;
      info.status = UPLOAD_FILE_WRITE;
      info.buf = static_cast<const uint8_t*>(q->payload);
      info.currentSize = q->len;
      info.totalSize = state->body_received + q->len;
      info.contentLength = state->content_length;
      info.path = state->path;
      info.query = state->query;
      state->upload_handler(info);

      state->body_received += q->len;
      q = q->next;
    }

    pbuf_free(p);

    if (state->content_length > 0 && state->body_received >= state->content_length) {
      Arduino_10BASE_T1S_HTTP::UploadInfo info;
      info.status = UPLOAD_FILE_END;
      info.buf = nullptr;
      info.currentSize = 0;
      info.totalSize = state->body_received;
      info.contentLength = state->content_length;
      info.path = state->path;
      info.query = state->query;
      state->upload_handler(info);
      state->server->handleRequest(tpcb, state);
    }

    return ERR_OK;
  }

  /* Copy pbuf chain into our flat request buffer (header parsing path). */
  uint16_t copy_len = (uint16_t)p->tot_len;
  uint16_t available = (uint16_t)(HTTP_SERVER_REQ_BUF_SIZE - 1 - state->req_len);
  if (copy_len > available) copy_len = available;

  pbuf_copy_partial(p, state->req_buf + state->req_len, copy_len, 0);
  state->req_len += copy_len;
  state->req_buf[state->req_len] = '\0';
  pbuf_free(p);

  /* Check if we have a complete HTTP request (ends with \r\n\r\n or \n\n). */
  if (!state->headers_done) {
    const char *end = mem_find(state->req_buf, state->req_len,
                               "\r\n\r\n", 4);
    uint16_t terminator_len = 4;
    if (!end) {
      end = mem_find(state->req_buf, state->req_len, "\n\n", 2);
      terminator_len = 2;
    }
    if (!end) {
      /* Buffer is full but \r\n\r\n not found — headers exceed our buffer.
       * Respond with 431 so the browser shows an informative error instead
       * of a silent timeout. */
      if (state->req_len >= (uint16_t)(HTTP_SERVER_REQ_BUF_SIZE - 1)) {
        Serial.println("[HTTP] 431 headers too large");
        sendResponse(tpcb, 431, "Request Header Fields Too Large",
          "<html><body><h2>431 Request Header Fields Too Large</h2>"
          "<p>Recompile with a larger HTTP_SERVER_REQ_BUF_SIZE.</p></body></html>");
      }
      return ERR_OK; /* Otherwise just wait for more data. */
    }

    state->headers_done = true;

    /* ------ Parse request line ---------------------------------------- */
    /* Format: METHOD /path?query HTTP/1.x\r\n */
    const char *req = state->req_buf;

    /* Method */
    const char *sp1 = strchr(req, ' ');
    if (!sp1) { closeConn(tpcb, state); return ERR_OK; }
    uint8_t method_len = (uint8_t)(sp1 - req);
    if (method_len >= (uint8_t)sizeof(state->method)) method_len = sizeof(state->method) - 1;
    memcpy(state->method, req, method_len);
    state->method[method_len] = '\0';

    /* Path (+ optional query) */
    const char *uri_start = sp1 + 1;
    const char *sp2 = strchr(uri_start, ' ');
    if (!sp2) sp2 = uri_start + strlen(uri_start);

    const char *q = (const char*)memchr(uri_start, '?', sp2 - uri_start);
    size_t path_len, query_len;

    if (q) {
      path_len  = (size_t)(q - uri_start);
      query_len = (size_t)(sp2 - q - 1);
    } else {
      path_len  = (size_t)(sp2 - uri_start);
      query_len = 0;
    }

    if (path_len >= sizeof(state->path))   path_len  = sizeof(state->path)  - 1;
    if (query_len >= sizeof(state->query)) query_len = sizeof(state->query) - 1;

    memcpy(state->path, uri_start, path_len);
    state->path[path_len] = '\0';

    if (q && query_len > 0) {
      memcpy(state->query, q + 1, query_len);
    }
    state->query[query_len] = '\0';

    state->content_length = parse_content_length(state->req_buf);
    state->upload_handler = state->server->findUploadHandler(state->path);
    state->upload_mode = (strcmp(state->method, "POST") == 0) && (state->upload_handler != nullptr);

    if (state->upload_mode) {
      Arduino_10BASE_T1S_HTTP::UploadInfo info;
      info.status = UPLOAD_START;
      info.buf = nullptr;
      info.currentSize = 0;
      info.totalSize = 0;
      info.contentLength = state->content_length;
      info.path = state->path;
      info.query = state->query;
      state->upload_handler(info);
      state->upload_started = true;

      uint16_t header_len = (uint16_t)((end - state->req_buf) + terminator_len);
      if (state->req_len > header_len) {
        uint16_t body_in_first_packet = (uint16_t)(state->req_len - header_len);
        Arduino_10BASE_T1S_HTTP::UploadInfo d;
        d.status = UPLOAD_FILE_WRITE;
        d.buf = reinterpret_cast<const uint8_t*>(state->req_buf + header_len);
        d.currentSize = body_in_first_packet;
        d.totalSize = body_in_first_packet;
        d.contentLength = state->content_length;
        d.path = state->path;
        d.query = state->query;
        state->upload_handler(d);
        state->body_received = body_in_first_packet;
      }

      /* Only fire END here when content_length is known and satisfied.
       * When content_length == 0 (chunked / parse failure) we wait for the
       * peer FIN (p == NULL path above) to fire END. */
      if (state->content_length > 0 && state->body_received >= state->content_length) {
        Arduino_10BASE_T1S_HTTP::UploadInfo f;
        f.status = UPLOAD_FILE_END;
        f.buf = nullptr;
        f.currentSize = 0;
        f.totalSize = state->body_received;
        f.contentLength = state->content_length;
        f.path = state->path;
        f.query = state->query;
        state->upload_handler(f);
        state->server->handleRequest(tpcb, state);
      }

      return ERR_OK;
    }

    /* Dispatch to user handler. */
    Serial.print("[HTTP] "); Serial.print(state->method);
    Serial.print(" ");     Serial.println(state->path);
    state->server->handleRequest(tpcb, state);
  }

  return ERR_OK;
}

/* Called after data we wrote has been acknowledged by the peer.
 * We use tcp_shutdown() to half-close TX after writing, so no action
 * is needed here — the connection tears down via onRecv(p==NULL). */
err_t Arduino_10BASE_T1S_HTTP::onSent(void *arg,
                                      struct tcp_pcb *tpcb,
                                      u16_t len)
{
  (void)arg; (void)tpcb; (void)len;
  return ERR_OK;
}

/* Called on fatal TCP errors (connection reset, out of memory, …). */
void Arduino_10BASE_T1S_HTTP::onError(void *arg, err_t err)
{
  (void)err;
  ConnState *state = static_cast<ConnState*>(arg);
  /* The PCB is already freed by lwIP before this callback — only free state. */
  if (state) delete state;
}

/* Periodic poll — close connections that never sent a complete request.
 * Fires every 12 s; gives the T1S/ICS/NAT path time to deliver the HTTP GET. */
err_t Arduino_10BASE_T1S_HTTP::onPoll(void *arg, struct tcp_pcb *tpcb)
{
  ConnState *state = static_cast<ConnState*>(arg);
  if (!state || !state->headers_done) {
    /* Timed out waiting for request data. */
    Serial.println("[HTTP] Poll timeout — closing idle connection");
    closeConn(tpcb, state);
  }
  return ERR_OK;
}
