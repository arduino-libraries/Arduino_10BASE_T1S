/*
 *  This file is part of the Arduino_10BASE_T1S library.
 *
 *  Copyright (c) 2024 Arduino SA
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

/**************************************************************************************
 * INCLUDES
 **************************************************************************************/

#include <stdint.h>
#include <stddef.h>

#if defined(ARDUINO_ARCH_RP2040)
#include <lwip/tcp.h>
#include <lwip/pbuf.h>
#include <lwip/err.h>
#else
#include "lib/liblwip/include/lwip/tcp.h"
#include "lib/liblwip/include/lwip/pbuf.h"
#include "lib/liblwip/include/lwip/err.h"
#endif

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

/** Maximum number of registered route handlers. */
#define HTTP_SERVER_MAX_ROUTES      8

/** Maximum number of registered upload handlers (POST streaming). */
#define HTTP_SERVER_MAX_UPLOAD_ROUTES 4

/** TODO: It needs to adjust buffer size as fit 
 * to the actual request size and response, microcontroller platform */

/** Receive buffer for one HTTP request (bytes).
 *  Modern browsers (Chrome/Edge) send Sec-Fetch-*, Sec-Ch-Ua-* and other
 *  headers that raise a typical POST to 600-750 bytes.  2 KB covers even
 *  the most verbose browser without wasting significant heap space (ConnState
 *  is heap-allocated, not on the stack). */
#define HTTP_SERVER_REQ_BUF_SIZE    2048

/** Response body buffer (bytes).
 *  Must be large enough for the longest handler response.
 *  The OTA update form HTML + minified JS is ~450 bytes; 1536 gives comfortable
 *  headroom for any user-added pages without wasting significant heap space
 *  (the buffer is static inside handleRequest, not on the stack). */
#define HTTP_SERVER_RESP_BUF_SIZE   1536

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

/**
 * @class Arduino_10BASE_T1S_HTTP
 * @brief Lightweight HTTP/1.0 server built on the lwIP raw-TCP API.
 *
 * Usage
 * -----
 * 1. Instantiate (optionally pass a port, default 80).
 * 2. Register route handlers with on().
 * 3. Call begin() after the network interface is up.
 * 4. No poll() call needed — the server is driven by lwIP callbacks.
 *
 * Handler signature
 * -----------------
 * @code
 * uint16_t myHandler(const char* method,
 *                    const char* path,
 *                    const char* query,
 *                    char*       resp_body,
 *                    size_t      resp_body_len)
 * {
 *     snprintf(resp_body, resp_body_len, "<h1>Hello</h1>");
 *     return 200;   // HTTP status code
 * }
 * @endcode
 *
 * @param method      "GET" or "POST"
 * @param path        URL path, e.g. "/sensors"
 * @param query       Query string after '?', empty string if none
 * @param resp_body   Output buffer — write your HTML/JSON response here
 * @param resp_body_len Size of resp_body buffer
 * @return HTTP status code to send (200, 404, 500, …)
 *
 * Content-Type is always "text/html; charset=utf-8".
 * For JSON, start the body with a recognisable prefix and the client can
 * detect it, or extend the class to carry a content-type out-param.
 */
class Arduino_10BASE_T1S_HTTP
{
public:

  enum UploadStatus {
    UPLOAD_START = 0,
    UPLOAD_FILE_WRITE,
    UPLOAD_FILE_END,
    UPLOAD_FILE_ABORTED
  };

  struct UploadInfo {
    UploadStatus    status;
    const uint8_t*  buf;
    size_t          currentSize;
    size_t          totalSize;
    size_t          contentLength;
    const char*     path;
    const char*     query;
  };

  /**
   * @brief Handler function pointer type.
   *
   * Write the response body into resp_body (null-terminated).
   * Return the HTTP status code (200, 404, 500, …).
   */
  typedef uint16_t (*RouteHandler)(const char* method,
                                   const char* path,
                                   const char* query,
                                   char*       resp_body,
                                   size_t      resp_body_len);

  typedef void (*UploadHandler)(const UploadInfo& upload_info);

           Arduino_10BASE_T1S_HTTP(uint16_t port = 80);
  virtual ~Arduino_10BASE_T1S_HTTP();

  /**
   * @brief Start the HTTP server.
   * Call this after the 10BASE-T1S network interface is up (i.e., after
   * Arduino_10BASE_T1S_PHY_Interface::begin() returns successfully).
   * @return true  Server is listening.
   * @return false Failed to create / bind the TCP PCB.
   */
  bool begin();

  /** @brief Stop the server and release all TCP resources. */
  void stop();

  /**
   * @brief Register a URL path handler.
   *
   * @param path     URL path to match, e.g. "/", "/data", "/led"
   *                 Exact match only (no wildcards).
   * @param handler  Function called when the path is requested.
   * @return true    Handler registered.
   * @return false   Route table full (HTTP_SERVER_MAX_ROUTES).
   */
  bool on(const char* path, RouteHandler handler);

  bool onUpload(const char* path, UploadHandler handler);

  /**
   * @brief Set a catch-all handler for paths with no registered route.
   * If not set, unmatched paths receive a plain 404 response.
   */
  void setNotFoundHandler(RouteHandler handler);

  /** @brief Returns true if the server is currently listening. */
  bool isRunning() const { return _listen_pcb != nullptr; }

private:

  /* ------------------------------------------------------------------ */
  /* Internal types                                                       */
  /* ------------------------------------------------------------------ */

  struct Route {
    const char*  path;
    RouteHandler handler;
  };

  struct UploadRoute {
    const char*   path;
    UploadHandler handler;
  };

  /** Per-connection state allocated on the heap when a client connects. */
  struct ConnState {
    Arduino_10BASE_T1S_HTTP* server;
    char     req_buf[HTTP_SERVER_REQ_BUF_SIZE];
    uint16_t req_len;
    bool     headers_done;
    char     method[8];
    char     path[128];
    char     query[64];
    bool     close_after_send;
    bool     upload_mode;
    bool     upload_started;
    uint32_t content_length;
    uint32_t body_received;
    UploadHandler upload_handler;
  };

  /* ------------------------------------------------------------------ */
  /* Data members                                                         */
  /* ------------------------------------------------------------------ */

  uint16_t     _port;
  tcp_pcb*     _listen_pcb;
  Route        _routes[HTTP_SERVER_MAX_ROUTES];
  UploadRoute  _upload_routes[HTTP_SERVER_MAX_UPLOAD_ROUTES];
  uint8_t      _route_count;
  uint8_t      _upload_route_count;
  RouteHandler _not_found_handler;

  /* ------------------------------------------------------------------ */
  /* Private helpers                                                      */
  /* ------------------------------------------------------------------ */

  RouteHandler findHandler(const char* path) const;
  UploadHandler findUploadHandler(const char* path) const;
  void         handleRequest(struct tcp_pcb* tpcb, ConnState* state);

  static void  sendResponse(struct tcp_pcb* tpcb,
                            uint16_t        status_code,
                            const char*     status_text,
                            const char*     body);
  static void  closeConn(struct tcp_pcb* tpcb, ConnState* state);

  /* ------------------------------------------------------------------ */
  /* lwIP raw-TCP callbacks (must be static)                              */
  /* ------------------------------------------------------------------ */

  static err_t onAccept(void* arg, struct tcp_pcb* new_pcb, err_t err);
  static err_t onRecv  (void* arg, struct tcp_pcb* tpcb,
                        struct pbuf* p, err_t err);
  static err_t onSent  (void* arg, struct tcp_pcb* tpcb, u16_t len);
  static void  onError (void* arg, err_t err);
  static err_t onPoll  (void* arg, struct tcp_pcb* tpcb);
};
