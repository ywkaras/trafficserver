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
 * @file GlobalPluginAbs.h
 *
 * @brief Contains the (abstract) base interface used in creating Global plugins.
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending GlobalPlugin.
 */

#pragma once

#include "tscpp/api/SessionPluginAbs.h"

namespace atscppapi
{
/**
 * @brief The base interface used when creating a GlobalPlugin.
 *
 * \note This interface can never be implemented directly, it should be implemented
 *   through extending GlobalPlugin.
 *
 * @see GlobalPlugin
 */
class GlobalPluginAbs : public SessionPluginAbs
{
public:
  using Self = GlobalPluginAbs;

  // TODO
  #if 0
  /**
   * A enumeration of the available types of per-transaction Hooks.
   */
  enum HookType {
  };

  /**< Human readable strings for each HookType, you can access them as HOOK_TYPE_STRINGS[HOOK_xxx] for example. */
  static const std::string HOOK_TYPE_STRINGS[];
  #endif

  virtual ~Self(){};

protected:
  /**
   * \note This interface can never be implemented directly, it should be implemented
   *   through extending GlobalPluginAbs.
   *
   * @private
   */
  Self() : SessionPluginAbs() {};
};

} // namespace atscppapi
