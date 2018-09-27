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
 * @file HookedTransaction.h
 * @brief Helps to create classes whose instances correspond to HTTP transactions.  When a hook is triggered for the
 * transaction, it will call a member function for the intance.
 */

#pragma once

#include <vector>
#include <utility>
#include <algorithm>

#include <ts/ts.h>
#include <atscppapi/transaction.h>

namespace atscppapi
{
using HttpHookID = TSHttpHookID;

namespace detail
{
class AbsBaseHookedTransaction
{
protected:
  AbsBaseHookedTransaction() {};

  virtual void run(TSEvent) = 0;

  static TSEvent hookEvent(HttpHookID);

  void hookAdd(HttpHookId);
};

} // end namespace detail

template <class Derived>
class BaseHookedTransaction : private detail::AbsBaseHookedTransaction
{
protected:
  BaseHookedTransaction()
  {
    // This generates a compile-time error if 'Derived' is not derived from 'BaseHookedTransaction<Derived>'.
    //
    static_cast<void>(static_cast<Derived *>(static_cast<BaseHookedTransaction *>(nullptr)));

    // This generates a compile-time error if 'Derived' is not derived from 'Transaction'
    //
    static_cast<void>(static_cast<Derived *>(static_cast<Transaction *>(nullptr)));
  }

  static void setHookHandler(HttpHookID hookID, bool (Derived::*handler)())

  using AbsBaseHookedTransaction::hookAdd;

private:
  typedef std::vector<std::pair<TSEvent, bool (Derived::*)()> > HandlerTableT;

  static HandlerTableT & handlerTable()
  {
    static HandlerTableT t;
    return t;
  }

  void run(TSEvent event) override
  {
  }
};

} // end namespace atscppapi
