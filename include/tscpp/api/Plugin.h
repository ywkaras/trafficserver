/**
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
/**
 * @file Plugin.h
 *
 * @brief Contains the base interfaces used in creating Global, Session and Transaction plugins.
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending GlobalPlugin, TransactionPlugin, or TransformationPlugin.
 */

#pragma once

#include "tscpp/api/Request.h"
#include "tscpp/api/Transaction.h"
#include "tscpp/api/Session.h"
#include "tscpp/api/VConnection.h"
#include "tscpp/api/noncopyable.h"

namespace atscppapi
{
/**
 * @brief Abstract class for transacton hook handling.
 *
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending GlobalPlugin, SessionPlugin, TransactionPlugin, or TransformationPlugin.
 *
 * @see TransactionPlugin
 * @see SessionPlugin
 * @see GlobalPlugin
 * @see TransformationPlugin
 */
class TransactionEvents : noncopyable
{
public:
  /**
   * A enumeration of the available types of Hooks. These are used with GlobalPlugin::registerHook(),
   * SessionPlugin::registerHook(), and TransactionPlugin::registerHook().
   */
  enum HookType {
    HOOK_READ_REQUEST_HEADERS_PRE_REMAP = 0, /**< This hook will be fired before remap has occured. */
    HOOK_READ_REQUEST_HEADERS_POST_REMAP,    /**< This hook will be fired directly after remap has occured. */
    HOOK_SEND_REQUEST_HEADERS,               /**< This hook will be fired right before request headers are sent to the origin */
    HOOK_READ_RESPONSE_HEADERS, /**< This hook will be fired right after response headers have been read from the origin */
    HOOK_SEND_RESPONSE_HEADERS, /**< This hook will be fired right before the response headers are sent to the client */
    HOOK_OS_DNS,                /**< This hook will be fired right after the OS DNS lookup */
    HOOK_READ_REQUEST_HEADERS,  /**< This hook will be fired after the request is read. */
    HOOK_READ_CACHE_HEADERS,    /**< This hook will be fired after the CACHE hdrs. */
    HOOK_CACHE_LOOKUP_COMPLETE, /**< This hook will be fired after caceh lookup complete. */
    HOOK_SELECT_ALT             /**< This hook will be fired after select alt. */
  };

  /**< Human readable strings for each HookType, you can access them as HOOK_TYPE_STRINGS[HOOK_OS_DNS] for example. */
  static const std::string HOOK_TYPE_STRINGS[];

  /**
   * This method should be overridden when you hook HOOK_READ_REQUEST_HEADERS_PRE_REMAP
   */
  virtual void
  handleReadRequestHeadersPreRemap(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_READ_REQUEST_HEADERS_POST_REMAP
   */
  virtual void
  handleReadRequestHeadersPostRemap(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_SEND_REQUEST_HEADERS
   */
  virtual void
  handleSendRequestHeaders(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_READ_RESPONSE_HEADERS
   */
  virtual void
  handleReadResponseHeaders(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_SEND_RESPONSE_HEADERS
   */
  virtual void
  handleSendResponseHeaders(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_OS_DNS
   */
  virtual void
  handleOsDns(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_READ_REQUEST_HEADERS
   */
  virtual void
  handleReadRequestHeaders(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_READ_CACHE_HEADERS
   */
  virtual void
  handleReadCacheHeaders(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_CACHE_LOOKUP_COMPLETE
   */
  virtual void
  handleReadCacheLookupComplete(Transaction &transaction)
  {
    transaction.resume();
  };

  /**
   * This method should be overridden when you hook HOOK_SELECT_ALT
   */
  virtual void handleSelectAlt(const Request &clientReq, const Request &cachedReq, const Response &cachedResp){};

  virtual ~TransactionEvents(){};

protected:
  /**
   * \note This interface can never be implemented directly, it should be implemented
   *   through extending GlobalPlugin, SessionPlugin, TransactionPlugin, or TransformationPlugin.
   *
   * @private
   */
  TransactionEvents(){};
};

/**
 * @brief Abstract class for session hook handling.
 *
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending GlobalPlugin or SessionPlugin.
 *
 * @see SessionPlugin
 * @see GlobalPlugin
 */
class SessionEvents : public TransactionEvents
{
public:
  /**
   * A enumeration of the available types of Hooks. These are used with GlobalPlugin::registerHook(),
   * and SessionPlugin::registerHook().
   */
  enum HookType {
    HOOK_TXN_START = 0 /**< This hook will be fired right after a transaction has started. */
  };

  /**< Human readable strings for each HookType, indexed by HookType values */
  static const std::string HOOK_TYPE_STRINGS[];

  /**
   * This method should be overridden when you hook HOOK_TXN_START.
   *
   * Overrides of this method should return true to cause transaction.resume() to be called, false to cause
   * transaction.error() to be called.
   */
  virtual bool
  handleTransactionStart(Transaction &transaction);

protected:
  /**
   * \note This interface can never be implemented directly, it should be implemented
   *   through extending GlobalPlugin or SessionPlugin.
   *
   * @private
   */
  SessionEvents() = default;
};

/**
 * @brief Abstract class for global hook handling.
 *
 * \note This interface can never be implemented directly, it should be implemented through extending GlobalPlugin.
 *
 * @see GlobalPlugin
 */
class GlobalEvents : public SessionEvents
{
public:
  /**
   * A enumeration of the available types of Hooks. These are used with GlobalPlugin::registerHook().
   *
   * NOTE:  As of November 2018 the VCONN start hook is only triggered to TCP connections that are TLS encrypted.
   */
  enum HookType {
    HOOK_VCONN_START = 0, /**< This hook will be fired right after a virtual connection has started. */
    HOOK_SSL_SERVERNAME,  /**< This hook will be fired if the client provides SNI information in the SSL handshake.
                               If called it will always be called after HOOK_VCONN_START. */
    HOOK_SSL_CERT,        /**< This hook will be fired as the server certificate is selected for the TLS handshake. */
    HOOK_SESSION_START    /**< This hook will be fired right after a session has started. */
  };

  /**< Human readable strings for each HookType, indexed by HookType values */
  static const std::string HOOK_TYPE_STRINGS[];

  /**
   * This method should be overridden when you hook HOOK_VCONN_START.  If vconn is not made persistent by this function,
   * it will be destroyed after this function returns.
   *
   * Overrides of this method should return true to cause vconn.resume() to be called, false to cause vconn.error() to be called.
   */
  virtual bool
  handleVConnectionStart(VConnection &vconn);

  /**
   * This method should be overridden when you hook HOOK_SSL_SERVERNAME.
   *
   * Overrides of this method should return true to cause vconn.resume() to be called, false to cause vconn.error() to be called.
   */
  virtual bool
  handleSslServername(VConnection &vconn);

  /**
   * This method should be overridden when you hook HOOK_SSL_CERT.
   *
   * Overrides of this method should return true to cause vconn.resume() to be called, false to cause vconn.error() to be called.
   */
  virtual bool
  handleVSslCert(VConnection &vconn);

  /**
   * This method should be overridden when you hook HOOK_SESSION_START.  If the session is not made persistent by this
   * function, it will be destroyed when this function returns.
   *
   * Overrides of this method should return true to cause session.resume() to be called, false to cause session.error() to be
   * called.
   */
  virtual bool
  handleSessionStart(Session &session);

protected:
  /**
   * \note This interface can never be implemented directly, it should be implemented
   *   through extending GlobalPlugin or SessionPlugin.
   *
   * @private
   */
  GlobalEvents() = default;
};

bool RegisterGlobalPlugin(const char *name, const char *vendor, const char *email);
inline bool
RegisterGlobalPlugin(std::string const &name, std::string const &vendor, std::string const &email)
{
  return RegisterGlobalPlugin(name.c_str(), vendor.c_str(), email.c_str());
}

} // namespace atscppapi
