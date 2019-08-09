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
 * @file HttpMsgComp.h
 *
 * Classes for convenient manipulation of HTTP message components.
 */

/*
Note:  The clases in this include file may be used independently from the rest of the C++ API.
*/

#pragma once

// Use this assert when the check expression has no side effects that are needed in the release build.  Otherwise, use
// TSAssert().
//
#undef assert
#if defined(__OPTIMIZE__)
#define assert(EXPR) static_cast<void>(0)
#else
#define assert TSAssert
#endif

#include <string_view>
#include <utility>
#include <memory>

#include <ts/ts.h>

/*
Note:  These clases are designed to create intances as local variables in functions.  It is expected that compiler
optimization will prevent the use of references from creating run-time overhead (see https://godbolt.org/z/KJZkno ).
In TS API hook handling code, no TS API function may be called after the call to TSHttpTxnReenable() (which is
called by the resume() and error() member functions of atscppapi::Transaction).  TS API functions are called by the
destructors of these classes.  Therefore, blocks containing instances of these claesses must end before the call to
TSHttpTxnReenable().
*/

/*
Note:  void * is used as the formal parameter type in function prototypes when the actual parameter type should be
TSHttpTxn (a transaction handle).  This is for compatability with  atscppapi::Transaction::getAtsHandle().
*/

namespace atscppapi
{
namespace detail
{
  struct CallTSFree {
    void
    operator()(char *p)
    {
      if (p) {
        TSfree(p);
      }
    }
  };
} // end namespace detail

// Class whose instances own char arrays which should be freed with TSfree().
//
class DynamicCharArray
{
public:
  DynamicCharArray(char *arr, int length) : _arr(arr), _length(length) {}

  char *
  data()
  {
    return _arr.get();
  }

  int
  length()
  {
    return _length;
  }

  std::string_view
  asStringView() const
  {
    return std::string_view(_arr.get(), _length);
  }

private:
  std::unique_ptr<char[], detail::CallTSFree> _arr;
  int _length;
};

namespace detail
{
  template <TSReturnCode (*txnRemapUrlGetFuncPtr)(TSHttpTxn, TSMLoc *)>
  DynamicCharArray
  txnRemapUrlStringGet(void *txn)
  {
    // Note:  Generally, TSMLoc instances must be freed with a call to TSHandleMLocRelease().  But currently releasing a
    // URL TSMLoc is a no-op.  And it's impossible to call TSHandleMLocRelease() here because it won't accept a null
    // TSMBuffer.
    //
    TSMLoc urlLoc;
    if (txnRemapUrlGetFuncPtr(static_cast<TSHttpTxn>(txn), &urlLoc) != TS_SUCCESS) {
      return DynamicCharArray(nullptr, 0);
    }
    int length;
    char *str = TSUrlStringGet(nullptr, urlLoc, &length);
    return DynamicCharArray(str, str ? length : 0);
  }
} // end namespace detail

DynamicCharArray
txnRemapFromUrlStringGet(void *txn)
{
  return detail::txnRemapUrlStringGet<TSRemapFromUrlGet>(txn);
}

DynamicCharArray
txnRemapToUrlStringGet(void *txn)
{
  return detail::txnRemapUrlStringGet<TSRemapToUrlGet>(txn);
}

// Note:  The TSUrlXxxGet() functions do not work for the remap to/from URLs.  That is why there are no equivalent
// capability in C++ provided in this header file.

DynamicCharArray
txnEffectiveUrlStringGet(void *txn)
{
  int length;
  char *str = TSHttpTxnEffectiveUrlStringGet(static_cast<TSHttpTxn>(txn), &length);
  return DynamicCharArray(str, str ? length : 0);
}

class MsgBase;

class MimeField
{
public:
  // If loc is TS_NULL_MLOC, instance constructed in empty state.
  //
  explicit MimeField(MsgBase &msg, TSMLoc loc = TS_NULL_MLOC) : _msg(msg), _loc(loc) {}

  // MimeField at (zero-base) index idx in HTTP message.
  //
  MimeField(MsgBase &msg, int idx);

  // MimeField with given name in HTTP message.
  //
  MimeField(MsgBase &msg, std::string_view name);

  TSMLoc
  loc() const
  {
    return _loc;
  }

  MsgBase &
  msg() const
  {
    return _msg;
  }

  // Valid means non-empty.
  //
  bool valid() const;

  // Put instance into empty state, releasing resources as appropriate.
  //
  void reset();

  // Can not copy, only move.

  MimeField(const MimeField &) = delete;
  MimeField &operator=(const MimeField &) = delete;

  MimeField(MimeField &&source);

  // Move assign only allowed if the two instances refer to the same message.
  //
  MimeField &operator=(MimeField &&source);

  void destroy();

  // Next field, returns invalid instance if none.
  //
  MimeField next() const;

  // Next field with same name, return invalid instance if none.
  //
  MimeField nextDup() const;

  // For the given field name, returns the last Mime field with that name in the given message, or an invalid MimeField if the
  // message contains no field by theat name.
  //
  static MimeField lastDup(MsgBase &msg, std::string_view name);

  std::string_view nameGet() const;

  void nameSet(std::string_view);

  void valuesClear();

  // Returns number of values. Values index are from 0 to valuesCount() - 1.
  //
  int valuesCount() const;

  std::string_view valGet(int idx) const;

  // Get comman-separated list of all values.
  //
  std::string_view valuesGet() const;

  void valSet(int idx, std::string_view new_value);

  // Insert a new value at index idx.  All values with index >= idx prior to calling this have their index incremented by one.
  //
  void valInsert(int idx, std::string_view new_value);

  // Append a new value at the end.
  //
  void valAppend(std::string_view new_value);

  ~MimeField();

private:
  MsgBase &_msg;
  TSMLoc _loc;
};

class MsgBuffer
{
public:
  explicit MsgBuffer(TSMBuffer hndl) : _hndl(hndl) {}

  // Returns the handle.
  //
  TSMBuffer
  bufHndl() const
  {
    return _hndl;
  }

  // No copying, in this or derived classes.
  MsgBuffer(const MsgBuffer &) = delete;
  MsgBuffer &operator=(const MsgBuffer &) = delete;

protected:
  MsgBuffer() {}

  void
  set(TSMBuffer hndl)
  {
    _hndl = hndl;
  }

private:
  TSMBuffer _hndl;
};

class MsgBase : public MsgBuffer
{
public:
  TSMLoc
  msgLoc() const
  {
    return _msgLoc;
  }

  bool
  hasMsg() const
  {
    return msgLoc() != TS_NULL_MLOC;
  }

  //  Returns number of mime header lines in HTTP message.  Can only be called if hasMsg() is true.
  //
  int
  mimeFieldsCount() const
  {
    assert(hasMsg());

    return TSMimeHdrFieldsCount(bufHndl(), msgLoc());
  }

  MimeField
  newMimeField(std::string_view name = std::string_view())
  {
    assert(hasMsg());

    TSMLoc loc;
    if (name != std::string_view()) {
      if (TSMimeHdrFieldCreateNamed(bufHndl(), msgLoc(), name.data(), name.length(), &loc) != TS_SUCCESS) {
        loc = TS_NULL_MLOC;
      }
    } else {
      if (TSMimeHdrFieldCreate(bufHndl(), msgLoc(), &loc) != TS_SUCCESS) {
        loc = TS_NULL_MLOC;
      }
    }
    return MimeField(*this, loc);
  }

  ~MsgBase()
  {
    if (_msgLoc && bufHndl()) {
      TSAssert(TSHandleMLocRelease(bufHndl(), TS_NULL_MLOC, _msgLoc) == TS_SUCCESS);
    }
  }

protected:
  explicit MsgBase(TSMBuffer bHndl = nullptr, TSMLoc msgLoc = TS_NULL_MLOC) : MsgBuffer(bHndl), _msgLoc(msgLoc) {}

  void
  setMsgLoc(TSMLoc msgLoc)
  {
    _msgLoc = msgLoc;
  }

private:
  TSMLoc _msgLoc;
};

class ReqMsg : public MsgBase
{
public:
  ReqMsg() {}

  ReqMsg(TSMBuffer bHndl, TSMLoc msgLoc) : MsgBase(bHndl, msgLoc) {}
};

class RespMsg : public MsgBase
{
public:
  RespMsg() {}

  RespMsg(TSMBuffer bHndl, TSMLoc msgLoc) : MsgBase(bHndl, msgLoc) {}
};

template <TSReturnCode (*getInTxn)(TSHttpTxn txnp, TSMBuffer *, TSMLoc *), class ReqOrRespMsgBase>
class TxnMsg : public ReqOrRespMsgBase
{
public:
  TxnMsg() {}

  explicit TxnMsg(void *txnHndl) { _init(txnHndl); }

  // Must not be called on instance that already has a message.
  //
  bool
  init(void *txnHndl)
  {
    assert(!this->hasMsg());

    _init(txnHndl);

    return (this->hasMsg());
  }

private:
  void
  _init(void *txnHndl)
  {
    TSMBuffer msgBuffer;
    TSMLoc msgLoc;

    if (TS_SUCCESS == getInTxn(static_cast<TSHttpTxn>(txnHndl), &msgBuffer, &msgLoc)) {
      this->set(msgBuffer);
      this->setMsgLoc(msgLoc);
    }
  }
};

using TxnClientReq  = TxnMsg<TSHttpTxnClientReqGet, ReqMsg>;
using TxnClientResp = TxnMsg<TSHttpTxnClientRespGet, RespMsg>;
using TxnServerReq  = TxnMsg<TSHttpTxnServerReqGet, ReqMsg>;
using TxnServerResp = TxnMsg<TSHttpTxnServerRespGet, RespMsg>;
using TxnCachedReq  = TxnMsg<TSHttpTxnCachedReqGet, ReqMsg>;
using TxnCachedResp = TxnMsg<TSHttpTxnCachedRespGet, RespMsg>;

//////////////////// Inline Member Function Implementations //////////////////

inline MimeField::MimeField(MsgBase &msg, int idx) : _msg(msg)
{
  assert(msg.hasMsg());
  assert((idx >= 0) && (idx < msg.mimeFieldsCount()));

  _loc = TSMimeHdrFieldGet(_msg.bufHndl(), _msg.msgLoc(), idx);
}

inline MimeField::MimeField(MsgBase &msg, std::string_view name) : _msg(msg)
{
  assert(msg.hasMsg());

  _loc = TSMimeHdrFieldFind(_msg.bufHndl(), _msg.msgLoc(), name.data(), name.length());
}

inline bool
MimeField::valid() const
{
  assert(_msg.hasMsg());

  return TS_NULL_MLOC != _loc;
}

inline void
MimeField::reset()
{
  if (valid()) {
    TSAssert(TSHandleMLocRelease(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);
  }

  _loc = TS_NULL_MLOC;
}

inline MimeField::MimeField(MimeField &&source) : _msg(source._msg)
{
  assert(_msg.hasMsg());

  _loc        = source._loc;
  source._loc = TS_NULL_MLOC;
}

// Move assign only allowed if the two instances refer to the same message.
//
inline MimeField &
MimeField::operator=(MimeField &&source)
{
  assert(&_msg == &source._msg);

  // Be OCD and block mf = std::move(mf) in debug loads.
  //
  assert(_loc != &source._loc);

  if (valid()) {
    TSAssert(TSHandleMLocRelease(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);
  }

  _loc        = source._loc;
  source._loc = TS_NULL_MLOC;

  return *this;
}

inline void
MimeField::destroy()
{
  assert(valid());

  TSAssert(TSMimeHdrFieldDestroy(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);

  TSAssert(TSHandleMLocRelease(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);

  _loc = TS_NULL_MLOC;
}

inline MimeField
MimeField::next() const
{
  assert(valid());

  return MimeField(_msg, TSMimeHdrFieldNext(_msg.bufHndl(), _msg.msgLoc(), _loc));
}

inline MimeField
MimeField::nextDup() const
{
  assert(valid());

  return MimeField(_msg, TSMimeHdrFieldNextDup(_msg.bufHndl(), _msg.msgLoc(), _loc));
}

inline MimeField
MimeField::lastDup(MsgBase &msg, std::string_view name)
{
  assert(msg.hasMsg());

  MimeField f(msg, name);

  if (f.valid()) {
    MimeField fd = f.nextDup();
    while (fd.valid()) {
      f  = std::move(fd);
      fd = f.nextDup();
    }
  }
  return f;
}

inline std::string_view
MimeField::nameGet() const
{
  assert(valid());

  int length;
  const char *s = TSMimeHdrFieldNameGet(_msg.bufHndl(), _msg.msgLoc(), _loc, &length);

  return std::string_view(s, length);
}

inline void
MimeField::nameSet(std::string_view new_name)
{
  assert(valid());

  TSAssert(TSMimeHdrFieldNameSet(_msg.bufHndl(), _msg.msgLoc(), _loc, new_name.data(), new_name.length()) == TS_SUCCESS);
}

inline void
MimeField::valuesClear()
{
  assert(valid());

  TSAssert(TSMimeHdrFieldValuesClear(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);
}

inline int
MimeField::valuesCount() const
{
  assert(valid());

  return TSMimeHdrFieldValuesCount(_msg.bufHndl(), _msg.msgLoc(), _loc);
}

inline std::string_view
MimeField::valGet(int idx) const
{
  assert(valid());

  assert((idx >= 0) && (idx < valuesCount()));

  int length;
  const char *s = TSMimeHdrFieldValueStringGet(_msg.bufHndl(), _msg.msgLoc(), _loc, idx, &length);

  return std::string_view(s, length);
}

inline std::string_view
MimeField::valuesGet() const
{
  assert(valid());

  int length;
  const char *s = TSMimeHdrFieldValueStringGet(_msg.bufHndl(), _msg.msgLoc(), _loc, -1, &length);

  return std::string_view(s, length);
}

inline void
MimeField::valSet(int idx, std::string_view new_value)
{
  assert(valid());

  assert((idx >= 0) && (idx < valuesCount()));

  TSAssert(TSMimeHdrFieldValueStringSet(_msg.bufHndl(), _msg.msgLoc(), _loc, idx, new_value.data(), new_value.length()) ==
           TS_SUCCESS);
}

inline void
MimeField::valInsert(int idx, std::string_view new_value)
{
  assert(valid());

  assert((idx >= 0) && (idx < valuesCount()));

  TSAssert(TSMimeHdrFieldValueStringInsert(_msg.bufHndl(), _msg.msgLoc(), _loc, idx, new_value.data(), new_value.length()) ==
           TS_SUCCESS);
}

inline void
MimeField::valAppend(std::string_view new_value)
{
  assert(valid());

  TSAssert(TSMimeHdrFieldValueStringInsert(_msg.bufHndl(), _msg.msgLoc(), _loc, -1, new_value.data(), new_value.length()) ==
           TS_SUCCESS);
}

MimeField::~MimeField()
{
  if (valid()) {
    TSAssert(TSHandleMLocRelease(_msg.bufHndl(), _msg.msgLoc(), _loc) == TS_SUCCESS);
  }
}

} // end namespace atscppapi
