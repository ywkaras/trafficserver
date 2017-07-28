#if !defined TS_BUFFERWRITER_H_
#define TS_BUFFERWRITER_H_

/** @file

    Utilities for generating character sequences.

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

#include <utility>
#include <cstring>

#include <ts/string_view.h>
#include <ts/ink_assert.h>

namespace ts
{
// Abstract class.
//
class BufferWriter
{
protected:
  // The _pushBack() functions "add" characters at the end.  If these functions discard any characters, this must put the instance
  // in an error state (indicated by the override of error() ).  Derived classes must not assume the _pushBack() functions will
  // not be called when the instance is in an error state.

  virtual void _pushBack(char c) = 0;

  virtual void
  _pushBack(string_view sV)
  {
    for (size_t i = 0; i < sV.size(); ++i) {
      _pushBack(sV[i]);
    }
  }

public:
  // Returns true if the instance is in an error state.
  //
  virtual bool error() const = 0;

  BufferWriter &
  c(const char c)
  {
    _pushBack(c);

    return *this;
  }

  // Forward arugments to string_view constructor and then write the resulting string_view.
  //
  template <typename... Args>
  BufferWriter &
  sV(Args &&... args)
  {
    string_view localSv(std::forward<Args>(args)...);

    _pushBack(localSv);

    return *this;
  }

  // Block calls to sV() template with no parameters.
  //
  BufferWriter &sV() = delete;

  // Force the use of nT() or l() for nul-terminated strings.
  //
  BufferWriter &sV(const char *) = delete;

  // Write a string literal.
  //
  template <size_t N>
  BufferWriter &
  l(const char (&s)[N])
  {
    _pushBack(string_view(s, N - 1));

    return *this;
  }

  // This function is redundant with sV().  It should be used to make it explicit that the raw char pointer is presumed to
  // point to a nul-terminated string, that will be converted to a string view.
  //
  BufferWriter &
  nT(const char *s)
  {
    _pushBack(string_view(s));

    return *this;
  }

  // Returns pointer to an auxiliary buffer (or nullptr if none is available).  Succeeding calls to non-const member functions,
  // other than auxBuffer(), must be presumed to invalidate the current auxiliary buffer (contents and address).
  //
  virtual char *
  auxBuffer()
  {
    return nullptr;
  }

  // Capacity of auxiliary buffer (number of chars, zero if no auxiliary buffer available).
  //
  virtual size_t
  auxCapacity() const
  {
    return 0;
  }

  // Write the first n characters that have been placed in the auxiliary buffer.  This call invalidates the auxiliary buffer.
  // This function should not be called if no auxiliary buffer is available.
  //
  virtual void
  auxWrite(size_t n)
  {
  }

  // Make destructor virtual.
  //
  virtual ~BufferWriter() {}
};

// This template instantiates to a concrete class.  'BufferWriter' must be a (direct or indirect) base class of 'Base'.  For 'b' an
// instance of 'Base', with protected or public access, the expression 'b._buf' must return a (non-const) char pointer to the
// buffer (array of chars) to write to.  For 'b' an instance of 'Base', with public access, the expression b.capacity() must
// return the sizeof the buffer.
//
// Typically you will want to use FixedBufferWriter or LocalBufferWriter, defined below, which are based on this.
//
template <class Base> class ConcreteBufferWriter : public Base
{
public:
  // Except for size, forward all constructor parameters to Base.
  //
  template <typename... Args> ConcreteBufferWriter(size_t size, Args &&... args) : Base(std::forward<Args>(args)...), _size(size) {}

  string_view
  view() const
  {
    return string_view(this->_buf, _size);
  }

  operator string_view() const { return view(); }

  size_t
  size() const
  {
    return _size;
  }

  // Discard characters currently at the end of the buffer.
  //
  void
  resize(size_t smaller_size)
  {
    ink_assert(smaller_size <= _size);
    _size = smaller_size;
  }

  char *
  auxBuffer() override
  {
    return error() ? nullptr : this->_buf + _size;
  }

  size_t
  auxCapacity() const override
  {
    return error() ? 0 : this->capacity() - _size;
  }

  void
  auxWrite(size_t n) override
  {
    ink_assert(n <= this->capacity());
    _size += n;
  }

  bool
  error() const override
  {
    return _size > this->capacity();
  }

protected:
  void
  _pushBack(char c) override
  {
    if (_size >= this->capacity()) {
      // Overflow error.
      //
      _size = this->capacity() + 1;

    } else {
      this->_buf[_size++] = c;
    }
  }

  void
  _pushBack(string_view sV) override
  {
    if ((_size + sV.size()) > this->capacity()) {
      // Overflow error.
      //
      _size = this->capacity() + 1;

    } else {
      std::memcpy(this->_buf + _size, sV.data(), sV.size());
      _size += sV.size();
    }
  }

  size_t _size;
};

namespace _private_
{
  class FixedBufferWriterBase : public BufferWriter
  {
  public:
    size_t
    capacity() const
    {
      return _capacity;
    }

  protected:
    FixedBufferWriterBase(char *buf, size_t capacity) : _buf(buf), _capacity(capacity) {}

    // No copying. (No move constructor/assignment because none for base class.)
    FixedBufferWriterBase(const FixedBufferWriterBase &) = delete;
    FixedBufferWriterBase &operator=(const FixedBufferWriterBase &) = delete;

    char *const _buf;
    const size_t _capacity;
  };

  template <size_t N> class LocalBufferWriterBase : public BufferWriter
  {
  public:
    size_t
    capacity() const
    {
      return N;
    }

  protected:
    char _buf[N];
  };

} // end namespace _private_

// A buffer writer that writes to an array of char that is external to the writer instance.
//
class FixedBufferWriter : public ConcreteBufferWriter<_private_::FixedBufferWriterBase>
{
public:
  // 'buf' is a pointer to the external array of char to write to.  'capacity' is the number of bytes in the array.
  //
  FixedBufferWriter(char *buf, size_t capacity) : ConcreteBufferWriter<_private_::FixedBufferWriterBase>(0, buf, capacity) {}
};

// A buffer writer that writes to an array of char (of fixed dimension N) that is internal to the writer instance.
// It's called 'local' because instances are typically declared as stack-allocated, local function variables.
//
template <size_t N> class LocalBufferWriter : public ConcreteBufferWriter<_private_::LocalBufferWriterBase<N>>
{
private:
  using Base = ConcreteBufferWriter<_private_::LocalBufferWriterBase<N>>;

public:
  LocalBufferWriter() : Base(0) {}

  LocalBufferWriter(const LocalBufferWriter &that) : Base(that._size) { std::memcpy(this->_buf, that._buf, this->_size); }

  LocalBufferWriter &
  operator=(const LocalBufferWriter &that)
  {
    if (this != &that) {
      this->_size = that._size;

      std::memcpy(this->_buf, that._buf, this->_size);
    }

    return (*this);
  }

  // Move construction/assignment intentionally defaulted to copying.
};

} // end namespace ts

#endif // include once
