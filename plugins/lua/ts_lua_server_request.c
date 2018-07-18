/*
  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "ts/ink_platform.h"
#include <netinet/in.h>
#include "ts_lua_util.h"

#define TS_LUA_CHECK_SERVER_REQUEST_HDR(http_ctx)                                                                                \
  do {                                                                                                                           \
    if (!http_ctx->server_request_hdrp) {                                                                                        \
      if (TSHttpTxnServerReqGet(http_ctx->txnp, &http_ctx->server_request_bufp, &http_ctx->server_request_hdrp) != TS_SUCCESS) { \
        return 0;                                                                                                                \
      }                                                                                                                          \
    }                                                                                                                            \
  } while (0)

#define TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx)                                                                         \
  do {                                                                                                                    \
    if (!http_ctx->server_request_url) {                                                                                  \
      TS_LUA_CHECK_SERVER_REQUEST_HDR(http_ctx);                                                                          \
      if (TSHttpHdrUrlGet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, &http_ctx->server_request_url) != \
          TS_SUCCESS) {                                                                                                   \
        return 0;                                                                                                         \
      }                                                                                                                   \
    }                                                                                                                     \
  } while (0)

static void ts_lua_inject_server_request_server_addr_api(lua_State *L);

static void ts_lua_inject_server_request_socket_api(lua_State *L);
static void ts_lua_inject_server_request_header_api(lua_State *L);
static void ts_lua_inject_server_request_headers_api(lua_State *L);
static void ts_lua_inject_server_request_get_header_size_api(lua_State *L);
static void ts_lua_inject_server_request_get_body_size_api(lua_State *L);
static void ts_lua_inject_server_request_uri_api(lua_State *L);
static void ts_lua_inject_server_request_uri_args_api(lua_State *L);
static void ts_lua_inject_server_request_uri_params_api(lua_State *L);
static void ts_lua_inject_server_request_url_api(lua_State *L);

static int ts_lua_server_request_header_get(lua_State *L);
static int ts_lua_server_request_header_set(lua_State *L);
static int ts_lua_server_request_get_headers(lua_State *L);
static int ts_lua_server_request_get_header_size(lua_State *L);
static int ts_lua_server_request_get_body_size(lua_State *L);
static int ts_lua_server_request_get_uri(lua_State *L);
static int ts_lua_server_request_set_uri(lua_State *L);
static int ts_lua_server_request_set_uri_args(lua_State *L);
static int ts_lua_server_request_get_uri_args(lua_State *L);
static int ts_lua_server_request_set_uri_params(lua_State *L);
static int ts_lua_server_request_get_uri_params(lua_State *L);
static int ts_lua_server_request_get_url_host(lua_State *L);
static int ts_lua_server_request_set_url_host(lua_State *L);
static int ts_lua_server_request_get_url_scheme(lua_State *L);
static int ts_lua_server_request_set_url_scheme(lua_State *L);

static int ts_lua_server_request_server_addr_get_ip(lua_State *L);
static int ts_lua_server_request_server_addr_get_port(lua_State *L);
static int ts_lua_server_request_server_addr_get_addr(lua_State *L);
static int ts_lua_server_request_server_addr_set_addr(lua_State *L);
static int ts_lua_server_request_server_addr_get_outgoing_port(lua_State *L);
static int ts_lua_server_request_server_addr_set_outgoing_addr(lua_State *L);
static int ts_lua_server_request_server_addr_get_nexthop_addr(lua_State *L);

void
ts_lua_inject_server_request_api(lua_State *L)
{
  lua_newtable(L);

  ts_lua_inject_server_request_socket_api(L);
  ts_lua_inject_server_request_header_api(L);
  ts_lua_inject_server_request_headers_api(L);
  ts_lua_inject_server_request_get_header_size_api(L);
  ts_lua_inject_server_request_get_body_size_api(L);

  ts_lua_inject_server_request_uri_api(L);
  ts_lua_inject_server_request_uri_args_api(L);
  ts_lua_inject_server_request_uri_params_api(L);

  ts_lua_inject_server_request_url_api(L);

  lua_setfield(L, -2, "server_request");
}

static void
ts_lua_inject_server_request_socket_api(lua_State *L)
{
  ts_lua_inject_server_request_server_addr_api(L);
}

static void
ts_lua_inject_server_request_server_addr_api(lua_State *L)
{
  lua_newtable(L);

  lua_pushcfunction(L, ts_lua_server_request_server_addr_get_ip);
  lua_setfield(L, -2, "get_ip");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_get_port);
  lua_setfield(L, -2, "get_port");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_get_addr);
  lua_setfield(L, -2, "get_addr");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_set_addr);
  lua_setfield(L, -2, "set_addr");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_get_outgoing_port);
  lua_setfield(L, -2, "get_outgoing_port");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_set_outgoing_addr);
  lua_setfield(L, -2, "set_outgoing_addr");

  lua_pushcfunction(L, ts_lua_server_request_server_addr_get_nexthop_addr);
  lua_setfield(L, -2, "get_nexthop_addr");

  lua_setfield(L, -2, "server_addr");

  lua_pushinteger(L, AF_INET);
  lua_setglobal(L, "TS_LUA_AF_INET");

  lua_pushinteger(L, AF_INET6);
  lua_setglobal(L, "TS_LUA_AF_INET6");
}

static void
ts_lua_inject_server_request_header_api(lua_State *L)
{
  lua_newtable(L); /* .header */

  lua_createtable(L, 0, 2); /* metatable for .header */

  lua_pushcfunction(L, ts_lua_server_request_header_get);
  lua_setfield(L, -2, "__index");
  lua_pushcfunction(L, ts_lua_server_request_header_set);
  lua_setfield(L, -2, "__newindex");

  lua_setmetatable(L, -2);

  lua_setfield(L, -2, "header");
}

static int
ts_lua_server_request_header_get(lua_State *L)
{
  const char *key;
  const char *val;
  int val_len;
  size_t key_len;
  int count;

  TSMLoc field_loc, next_field_loc;
  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  /*  we skip the first argument that is the table */
  key = luaL_checklstring(L, 2, &key_len);

  if (!http_ctx->server_request_hdrp) {
    if (TSHttpTxnServerReqGet(http_ctx->txnp, &http_ctx->server_request_bufp, &http_ctx->server_request_hdrp) != TS_SUCCESS) {
      lua_pushnil(L);
      return 1;
    }
  }

  if (key && key_len) {
    field_loc = TSMimeHdrFieldFind(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, key, key_len);

    if (field_loc != nullptr) {
      count = 0;
      while (field_loc != nullptr) {
        val = TSMimeHdrFieldValueStringGet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc, -1, &val_len);
        next_field_loc = TSMimeHdrFieldNextDup(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
        lua_pushlstring(L, val, val_len);
        count++;
        // multiple headers with the same name must be semantically the same as one value which is comma seperated
        if (next_field_loc != nullptr) {
          lua_pushlstring(L, ",", 1);
          count++;
        }
        TSMimeHdrFldRelease(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
        field_loc = next_field_loc;
      }
      lua_concat(L, count);
    } else {
      lua_pushnil(L);
    }

  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
ts_lua_server_request_header_set(lua_State *L)
{
  const char *key;
  const char *val;
  size_t val_len;
  size_t key_len;
  int remove;
  int first;

  TSMLoc field_loc, tmp;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  remove = 0;
  val    = NULL;

  /*   we skip the first argument that is the table */
  key = luaL_checklstring(L, 2, &key_len);
  if (lua_isnil(L, 3)) {
    remove = 1;
  } else {
    val = luaL_checklstring(L, 3, &val_len);
  }

  if (!http_ctx->server_request_hdrp) {
    if (TSHttpTxnServerReqGet(http_ctx->txnp, &http_ctx->server_request_bufp, &http_ctx->server_request_hdrp) != TS_SUCCESS) {
      return 0;
    }
  }

  field_loc = TSMimeHdrFieldFind(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, key, key_len);

  if (remove) {
    while (field_loc != nullptr) {
      tmp = TSMimeHdrFieldNextDup(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      TSMimeHdrFieldDestroy(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      TSMimeHdrFldRelease(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      field_loc = tmp;
    }
  } else if (field_loc != nullptr) {
    first = 1;
    while (field_loc != nullptr) {
      tmp = TSMimeHdrFieldNextDup(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      if (first) {
        first = 0;
        TSMimeHdrFieldValueStringSet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc, -1, val, val_len);
      } else {
        TSMimeHdrFieldDestroy(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      }
      TSMimeHdrFldRelease(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      field_loc = tmp;
    }
  } else if (TSMimeHdrFieldCreateNamed(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, key, key_len, &field_loc) !=
             TS_SUCCESS) {
    TSError("[ts_lua][%s] TSMimeHdrFieldCreateNamed error", __FUNCTION__);
    return 0;

  } else {
    TSMimeHdrFieldValueStringSet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc, -1, val, val_len);
    TSMimeHdrFieldAppend(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
  }

static int
ts_lua_server_request_get_body_size(lua_State *L)
{
  int64_t body_size;
  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  body_size = TSHttpTxnServerReqBodyBytesGet(http_ctx->txnp);
  lua_pushnumber(L, body_size);

  return 1;
}

static void
ts_lua_inject_server_request_uri_api(lua_State *L)
{
  lua_pushcfunction(L, ts_lua_server_request_set_uri);
  lua_setfield(L, -2, "set_uri");

  lua_pushcfunction(L, ts_lua_server_request_get_uri);
  lua_setfield(L, -2, "get_uri");
}

static int
ts_lua_server_request_get_uri(lua_State *L)
{
  const char *path;
  int path_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  path = TSUrlPathGet(http_ctx->server_request_bufp, http_ctx->server_request_url, &path_len);

  lua_pushlstring(L, "/", 1);
  lua_pushlstring(L, path, path_len >= TS_LUA_MAX_URL_LENGTH - 1 ? TS_LUA_MAX_URL_LENGTH - 2 : path_len);
  lua_concat(L, 2);

  return 1;
}

static int
ts_lua_server_request_set_uri(lua_State *L)
{
  const char *path;
  size_t path_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  path = luaL_checklstring(L, 1, &path_len);

  if (*path == '/') {
    path++;
    path_len--;
  }

  TSUrlPathSet(http_ctx->server_request_bufp, http_ctx->server_request_url, path, path_len);

  return 0;
}

static void
ts_lua_inject_server_request_uri_args_api(lua_State *L)
{
  lua_pushcfunction(L, ts_lua_server_request_set_uri_args);
  lua_setfield(L, -2, "set_uri_args");

  lua_pushcfunction(L, ts_lua_server_request_get_uri_args);
  lua_setfield(L, -2, "get_uri_args");
}

static int
ts_lua_server_request_set_uri_args(lua_State *L)
{
  const char *param;
  size_t param_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  param = luaL_checklstring(L, 1, &param_len);
  TSUrlHttpQuerySet(http_ctx->server_request_bufp, http_ctx->server_request_url, param, param_len);

  return 0;
}

static int
ts_lua_server_request_get_uri_args(lua_State *L)
{
  const char *param;
  int param_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  param = TSUrlHttpQueryGet(http_ctx->server_request_bufp, http_ctx->server_request_url, &param_len);

  if (param && param_len > 0) {
    lua_pushlstring(L, param, param_len);

  } else {
    lua_pushnil(L);
  }

  return 1;
}

static void
ts_lua_inject_server_request_uri_params_api(lua_State *L)
{
  lua_pushcfunction(L, ts_lua_server_request_set_uri_params);
  lua_setfield(L, -2, "set_uri_params");

  lua_pushcfunction(L, ts_lua_server_request_get_uri_params);
  lua_setfield(L, -2, "get_uri_params");
}

static int
ts_lua_server_request_set_uri_params(lua_State *L)
{
  const char *param;
  size_t param_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  param = luaL_checklstring(L, 1, &param_len);
  TSUrlHttpParamsSet(http_ctx->server_request_bufp, http_ctx->server_request_url, param, param_len);

  return 0;
}

static int
ts_lua_server_request_get_uri_params(lua_State *L)
{
  const char *param;
  int param_len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  param = TSUrlHttpParamsGet(http_ctx->server_request_bufp, http_ctx->server_request_url, &param_len);

  if (param && param_len > 0) {
    lua_pushlstring(L, param, param_len);

  } else {
    lua_pushnil(L);
  }

  return 1;
}

static void
ts_lua_inject_server_request_url_api(lua_State *L)
{
  lua_pushcfunction(L, ts_lua_server_request_get_url_host);
  lua_setfield(L, -2, "get_url_host");
  lua_pushcfunction(L, ts_lua_server_request_set_url_host);
  lua_setfield(L, -2, "set_url_host");

  lua_pushcfunction(L, ts_lua_server_request_get_url_scheme);
  lua_setfield(L, -2, "get_url_scheme");
  lua_pushcfunction(L, ts_lua_server_request_set_url_scheme);
  lua_setfield(L, -2, "set_url_scheme");
}

static int
ts_lua_server_request_get_url_host(lua_State *L)
{
  const char *host;
  int len = 0;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  host = TSUrlHostGet(http_ctx->server_request_bufp, http_ctx->server_request_url, &len);

  if (len == 0) {
    char *key   = "Host";
    char *l_key = "host";
    int key_len = 4;

    TSMLoc field_loc;

    field_loc = TSMimeHdrFieldFind(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, key, key_len);
    if (field_loc) {
      host = TSMimeHdrFieldValueStringGet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc, -1, &len);
      TSMimeHdrFldRelease(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);

    } else {
      field_loc = TSMimeHdrFieldFind(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, l_key, key_len);
      if (field_loc) {
        host = TSMimeHdrFieldValueStringGet(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc, -1, &len);
        TSMimeHdrFldRelease(http_ctx->server_request_bufp, http_ctx->server_request_hdrp, field_loc);
      }
    }
  }

  lua_pushlstring(L, host, len);

  return 1;
}

static int
ts_lua_server_request_set_url_host(lua_State *L)
{
  const char *host;
  size_t len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);
  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  host = luaL_checklstring(L, 1, &len);

  TSUrlHostSet(http_ctx->server_request_bufp, http_ctx->server_request_url, host, len);

  return 0;
}

static int
ts_lua_server_request_get_url_scheme(lua_State *L)
{
  const char *scheme;
  int len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);
  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  scheme = TSUrlSchemeGet(http_ctx->server_request_bufp, http_ctx->server_request_url, &len);

  lua_pushlstring(L, scheme, len);

  return 1;
}

static int
ts_lua_server_request_set_url_scheme(lua_State *L)
{
  const char *scheme;
  size_t len;

  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);
  TS_LUA_CHECK_SERVER_REQUEST_URL(http_ctx);

  scheme = luaL_checklstring(L, 1, &len);

  TSUrlSchemeSet(http_ctx->server_request_bufp, http_ctx->server_request_url, scheme, len);

  return 0;
}

static int
ts_lua_server_request_server_addr_get_ip(lua_State *L)
{
  struct sockaddr const *server_ip;
  char sip[128];
  ts_lua_http_ctx *http_ctx;

  GET_HTTP_CONTEXT(http_ctx, L);

  server_ip = TSHttpTxnServerAddrGet(http_ctx->txnp);

  if (server_ip == NULL) {
    lua_pushnil(L);

  } else {
    if (server_ip->sa_family == AF_INET) {
      inet_ntop(AF_INET, (const void *)&((struct sockaddr_in *)server_ip)->sin_addr, sip, sizeof(sip));
    } else {
      inet_ntop(AF_INET6, (const void *)&((struct sockaddr_in6 *)server_ip)->sin6_addr, sip, sizeof(sip));
    }

    lua_pushstring(L, sip);
  }

  return 1;
}

static int
ts_lua_server_request_server_addr_get_port(lua_State *L)
{
  struct sockaddr const *server_ip;
  ts_lua_http_ctx *http_ctx;
  int port;

  GET_HTTP_CONTEXT(http_ctx, L);

  server_ip = TSHttpTxnServerAddrGet(http_ctx->txnp);

  if (server_ip == NULL) {
    lua_pushnil(L);

  } else {
    if (server_ip->sa_family == AF_INET) {
      port = ((struct sockaddr_in *)server_ip)->sin_port;
    } else {
      port = ((struct sockaddr_in6 *)server_ip)->sin6_port;
    }

    lua_pushnumber(L, ntohs(port));
  }

  return 1;
}

static int
ts_lua_server_request_server_addr_get_outgoing_port(lua_State *L)
{
  struct sockaddr const *outgoing_addr;
  ts_lua_http_ctx *http_ctx;
  int port;

  GET_HTTP_CONTEXT(http_ctx, L);

  outgoing_addr = TSHttpTxnOutgoingAddrGet(http_ctx->txnp);

  if (outgoing_addr == NULL) {
    lua_pushnil(L);

  } else {
    if (outgoing_addr->sa_family == AF_INET) {
      port = ((struct sockaddr_in *)outgoing_addr)->sin_port;
    } else {
      port = ((struct sockaddr_in6 *)outgoing_addr)->sin6_port;
    }

    lua_pushnumber(L, ntohs(port));
  }

  return 1;
}

static int
ts_lua_server_request_server_addr_get_addr(lua_State *L)
{
  struct sockaddr const *server_ip;
  ts_lua_http_ctx *http_ctx;
  int port;
  int family;
  char sip[128];

  GET_HTTP_CONTEXT(http_ctx, L);

  server_ip = TSHttpTxnServerAddrGet(http_ctx->txnp);

  if (server_ip == NULL) {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);

  } else {
    if (server_ip->sa_family == AF_INET) {
      port = ntohs(((struct sockaddr_in *)server_ip)->sin_port);
      inet_ntop(AF_INET, (const void *)&((struct sockaddr_in *)server_ip)->sin_addr, sip, sizeof(sip));
      family = AF_INET;
    } else {
      port = ntohs(((struct sockaddr_in6 *)server_ip)->sin6_port);
      inet_ntop(AF_INET6, (const void *)&((struct sockaddr_in6 *)server_ip)->sin6_addr, sip, sizeof(sip));
      family = AF_INET6;
    }

    lua_pushstring(L, sip);
    lua_pushnumber(L, port);
    lua_pushnumber(L, family);
  }

  return 3;
}

static int
ts_lua_server_request_server_addr_get_nexthop_addr(lua_State *L)
{
  struct sockaddr const *server_ip;
  ts_lua_http_ctx *http_ctx;
  int port;
  int family;
  char sip[128];

  GET_HTTP_CONTEXT(http_ctx, L);

  server_ip = TSHttpTxnNextHopAddrGet(http_ctx->txnp);

  if (server_ip == NULL) {
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);

  } else {
    if (server_ip->sa_family == AF_INET) {
      port = ntohs(((struct sockaddr_in *)server_ip)->sin_port);
      inet_ntop(AF_INET, (const void *)&((struct sockaddr_in *)server_ip)->sin_addr, sip, sizeof(sip));
      family = AF_INET;
    } else {
      port = ntohs(((struct sockaddr_in6 *)server_ip)->sin6_port);
      inet_ntop(AF_INET6, (const void *)&((struct sockaddr_in6 *)server_ip)->sin6_addr, sip, sizeof(sip));
      family = AF_INET6;
    }

    lua_pushstring(L, sip);
    lua_pushnumber(L, port);
    lua_pushnumber(L, family);
  }

  return 3;
}

static int
ts_lua_server_request_server_addr_set_addr(lua_State *L)
{
  union {
    struct sockaddr_in sin4;
    struct sockaddr_in6 sin6;
    struct sockaddr sa;
  } addr;
  ts_lua_http_ctx *http_ctx;
  int n;
  int port;
  int family;
  const char *sip;
  size_t sip_len;

  GET_HTTP_CONTEXT(http_ctx, L);

  n = lua_gettop(L);

  if (n == 3) {
    sip    = luaL_checklstring(L, 1, &sip_len);
    port   = luaL_checknumber(L, 2);
    family = luaL_checknumber(L, 3);

    if (family == AF_INET) {
      addr.sin4.sin_family = AF_INET;
      addr.sin4.sin_port   = htons(port);
      if (!inet_pton(family, sip, &addr.sin4.sin_addr)) {
        return luaL_error(L, "invalid ipv4 address");
      }
    } else {
      addr.sin6.sin6_family = AF_INET6;
      addr.sin6.sin6_port   = htons(port);
      if (!inet_pton(family, sip, &addr.sin6.sin6_addr)) {
        return luaL_error(L, "invalid ipv6 address");
      }
    }

    TSHttpTxnServerAddrSet(http_ctx->txnp, &addr.sa);
  } else {
    return luaL_error(L, "incorrect # of arguments to ts.server_request.addr.set_addr, receiving %d instead of 3", n);
  }

  return 0;
}

static int
ts_lua_server_request_server_addr_set_outgoing_addr(lua_State *L)
{
  union {
    struct sockaddr_in sin4;
    struct sockaddr_in6 sin6;
    struct sockaddr sa;
  } addr;
  ts_lua_http_ctx *http_ctx;
  int n;
  int port;
  int family;
  const char *sip;
  size_t sip_len;

  GET_HTTP_CONTEXT(http_ctx, L);

  n = lua_gettop(L);

  if (n == 3) {
    sip    = luaL_checklstring(L, 1, &sip_len);
    port   = luaL_checknumber(L, 2);
    family = luaL_checknumber(L, 3);

    if (family == AF_INET) {
      addr.sin4.sin_family = AF_INET;
      addr.sin4.sin_port   = htons(port);
      if (!inet_pton(family, sip, &addr.sin4.sin_addr)) {
        return luaL_error(L, "invalid ipv4 address");
      }
    } else {
      addr.sin6.sin6_family = AF_INET6;
      addr.sin6.sin6_port   = htons(port);
      if (!inet_pton(family, sip, &addr.sin6.sin6_addr)) {
        return luaL_error(L, "invalid ipv6 address");
      }
    }

    TSHttpTxnOutgoingAddrSet(http_ctx->txnp, &addr.sa);
  } else {
    return luaL_error(L, "incorrect # of arguments to ts.server_request.addr.set_outgoing_addr, receiving %d instead of 3", n);
  }

  return 0;
}
