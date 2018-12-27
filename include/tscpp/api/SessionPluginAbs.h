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
 * @file SessionPluginAbs.h
 *
 * @brief Contains the (abstract) base interface used in creating Session plugins.
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending SessionPlugin.
 */

#pragma once

#include "tscpp/api/Transaction.h"
#include "tscpp/api/TransactionPluginAbs.h"

namespace atscppapi
{
/**
 * @brief The base interface used when creating a SessionPlugin.
 *
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending SessionPlugin.
 *
 * @see SessionPlugin
 */
class SessionPluginAbs : public TransactionPluginAbs
{
public:
  using Self = SessionPluginAbs;

  /**
   * A enumeration of the available types of per-Transaction Hooks.
   */
  enum HookType {
    HOOK_START_TRANSACTION = 0, /**< This hook will be fired after a transaction has started. */
  };

  /**< Human readable strings for each HookType, you can access them as HOOK_TYPE_STRINGS[HOOK_START_TRANSACTION] for example. */
  static const std::string HOOK_TYPE_STRINGS[];

  /**
   * This method must be implemented when you hook HOOK_START_TRANSACTION
   */
  virtual void
  handleTransactionStart(Transaction &transaction)
  {
    transaction.resume();
  };

  virtual ~Self(){};

protected:
  /**
   * \note This interface can never be implemented directly, it should be implemented
   *   through extending SessionPluginAbs.
   *
   * @private
   */
  Self() : TransactionPluginAbs() {};
};

} // namespace atscppapi
