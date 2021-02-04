/** @file

  Classes for proper mutual exclusion for a single writer and multiple
  readers of a data structure.  If writes are infrequent relative to reads,
  these classes allow reading to generally occur without blocking the
  thread.  The lock() and unlock() member functions below all provide a
  strong memory fence (Sequentially-consistent ordering).  Additioal read
  locks of a particular mutex are not permitted when there is a pending write
  of the mutex.  Based on limited testing, this implementation seems to have
  better performance than ptread rwlocks.  (std::shared_mutex is presumably
  implemented using pthread rwlocks.)

  @section license License

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

#pragma once

#include <mutex>

#include <tscore/ink_rwlock.h>

namespace ts
{
class OneWriterMultiReader
{
protected:
  struct BasicWriteLock // Meets BasicLockable Standard Library requirements.
  {
    BasicWriteLock(OneWriterMultiReader &owmr_) : owmr(owmr_) {}

    void
    lock()
    {
      ink_rwlock_wrlock(&owmr._rwlock);
    }

    void
    unlock()
    {
      ink_rwlock_unlock(&owmr._rwlock);
    }

    OneWriterMultiReader &owmr;

    // Disallow copying because lock ownership is unique.  Disallowing moving since I can't think of a case when
    // it would be a good idea.
    //
    BasicWriteLock(const BasicWriteLock &) = delete;
    BasicWriteLock &operator=(const BasicWriteLock &) = delete;
  };

public:
  class ReadLock // Meets Lockable Standard Library requirements.
  {
  public:
    ReadLock(OneWriterMultiReader &owmr) : _owmr{owmr} { lock(); }

    ReadLock(OneWriterMultiReader &owmr, std::defer_lock_t) : _owmr{owmr} {}

    // bool try_lock();

    void
    lock()
    {
      ink_rwlock_rdlock(&_owmr._rwlock);
      locked = true;
    }

    void
    unlock()
    {
      if (locked) {
        ink_rwlock_unlock(&_owmr._rwlock);
        locked = false;
      }
    }

    bool
    is_locked() const
    {
      return locked;
    }

    ~ReadLock() { unlock(); }

    // Disallow copying because lock ownership is unique.  Disallowing moving since I can't think of a case when
    // it would be a good idea.
    //
    ReadLock(const ReadLock &) = delete;
    ReadLock &operator=(const ReadLock &) = delete;

  private:
    OneWriterMultiReader &_owmr;

    bool locked{false};
  };

  // User code must ensure that, while one thread has a write lock on a OneWriterMultiReader instance, no other
  // thread attempts to get a write lock on the same instance.
  //
  class WriteLock : private BasicWriteLock // Meets BasicLockable Standard Library requirements.
  {
  public:
    WriteLock(OneWriterMultiReader &owmr) : BasicWriteLock{owmr} { lock(); }

    WriteLock(OneWriterMultiReader &owmr, std::defer_lock_t) : BasicWriteLock{owmr} {}

    void
    lock()
    {
      BasicWriteLock::lock();

      locked = true;
    }

    void
    unlock()
    {
      if (locked) {
        BasicWriteLock::unlock();

        locked = false;
      }
    }

    bool
    is_locked() const
    {
      return locked;
    }

    ~WriteLock() { unlock(); }

  private:
    bool locked{false};
  };

  OneWriterMultiReader() { ink_rwlock_init(&_rwlock); }
  ~OneWriterMultiReader() { ink_rwlock_destroy(&_rwlock); }

private:
  ink_rwlock _rwlock;
};

using ExclusiveWriterMultiReader = OneWriterMultiReader;

} // end namespace ts
