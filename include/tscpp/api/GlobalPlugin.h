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
 * @file GlobalPlugin.h
 * @brief Contains the interface used in creating Global plugins.
 */

#pragma once

#include "tscpp/api/Plugin.h"

namespace atscppapi
{
struct GlobalPluginState;

/**
 * @brief The interface used when creating a GlobalPlugin.
 *
 * A GlobalPlugin is a Plugin that will fire for a given transaction hook on all transactions, a given session hook for all
 * sessions, and global hooks as they occur.
 *
 * Depending on the
 * type of hook you choose to build you will implement one or more callback methods.
 * Here is a simple example of a GlobalPlugin:
 *
 * \code
 * class GlobalHookPlugin : public GlobalPlugin {
 * public:
 *  GlobalHookPlugin() {
 *   registerHook(HOOK_READ_REQUEST_HEADERS_PRE_REMAP);
 *  }
 *  virtual void handleReadRequestHeadersPreRemap(Transaction &transaction) {
 *    std::cout << "Hello from handleReadRequesHeadersPreRemap!" << std::endl;
 *    transaction.resume();
 *  }
 * };
 * \endcode
 * @see Plugin
 */
class GlobalPlugin : public GlobalEvents
{
public:
  /**
   * registerHook is the mechanism used to attach a global hook.
   *
   * \note Whenever you register a hook you must have the appropriate callback definied in your GlobalPlugin
   *  see HookType and GlobalEvents for the correspond HookTypes and callback methods. If you fail to implement the
   *  callback, a default implmentation will be used that will only resume.
   */
  void registerHook(TransactionEvents::HookType);
  void registerHook(SessionEvents::HookType);
  void registerHook(GlobalEvents::HookType);

  ~GlobalPlugin() override;

protected:
  /**
   * Constructor.
   *
   * @param ignore_internal_sessions When true, all hooks registered by this plugin are ignored
   *                                 for internal sessions (internal sessions are created
   *                                 when other plugins create requests).
   *
   * @param ignore_internal_transactions When true, all hooks registered by this plugin are ignored
   *                                     for internal transactions (internal transactions are created
   *                                     when other plugins create requests).
   */
  GlobalPlugin(bool ignore_internal_sessions = false, bool ignore_internal_transactions = ignore_internal_sessions);

private:
  GlobalPluginState *state_; /**< Internal state tied to a GlobalPlugin */
};

} // namespace atscppapi
