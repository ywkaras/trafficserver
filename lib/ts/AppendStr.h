/** @file

  Buffer for strings that can grow at the end without heap allocation.

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

#ifndef __LIB_TS_APPENDSTR_H__
#define __LIB_TS_APPENDSTR_H__

#include <cstring>

#include <ts/MemView.h>

#ifndef __LIB_TS_TEST_APPENDSTR_CC__

#include "ts/ink_assert.h"

#endif

namespace util
{

// NOTE! AppendStr is a temporary tool, AMC wants to replace it with a new memory buffer class with a python-esque 'print' member
// function.

// 'Dimension' is maximum storage bytes used by string, including nul at end.
//
template <std::size_t Dimension>
class AppendStr
{
public:

  AppendStr() { clear(); }

  // Returns current string value.
  //
  operator const char * () const { return(_v); }

  const char * str() const { return(_v); }

  // Returns number of bytes in buffer remaining after last character of string (including terminating nul).
  //
  std::size_t left() const { return(Dimension - std::size_t(_stringLen)); }

  // Returns pointer to current nul position.  Additional charaters can be appended here, followed by a nul.
  //
  char * end() { return(_v + _stringLen); }

  // Call this after appending know number of characters.  The number of appended characters, must not execeed left() - 1.
  //
  void newEnd(int numAppendedChars)
  {
    int newLen = _stringLen + numAppendedChars;

    ink_assert((numAppendedChars >= 0) and (newLen < int(Dimension)));

    _stringLen = newLen;

    _v[_stringLen] = '\0';
  };

  // Call this after appending and nul-terminating more characters.  The number of appended characters, inluding the nul, must not
  // execeed left().
  //
  void newEnd()
  {
    ink_assert(!_v[Dimension - 1]);

    _stringLen += std::strlen(_v + _stringLen);
  };

  // Returns number of characters in string, not counting terminating nul.
  //
  int strLen() const { return(_stringLen); }

  // Add a single character.
  //
  template <typename ... Tail>
  void add(char c, Tail ... tail)
  {
    ink_assert(_stringLen < int(Dimension - 1));

    _v[_stringLen] = c;

    ++_stringLen;
    
    add(tail ...);  
  }

  // Add a non-nul-terminated string.
  //
  template <typename ... Tail>
  void add(ts::StringView sV, Tail ... tail)
  {
    int newLen = _stringLen + sV.size();

    ink_assert(newLen < int(Dimension));

    std::memcpy(_v + _stringLen, sV.begin(), sV.size());

    _stringLen = newLen;
    
    add(tail ...);
  }

  // Add a nul-terminated string.
  //
  template <typename ... Tail>
  void add(const char *str, Tail ... tail)
  {
    add(ts::StringView(str, std::size_t(std::strlen(str))), tail ...);  
  }

  // Delete appending charactrs by reverting to previous string length.
  //
  void revert(int prevStrLen)
  {
    ink_assert((prevStrLen >= 0) and (prevStrLen <= _stringLen));

    _stringLen = prevStrLen;
    _v[_stringLen] = '\0';
  }

  // Make empty.
  //
  void clear()
  {
    _stringLen = 0;
    _v[0] = '\0';
    _v[Dimension - 1] = '\0';
  }

private:
  void add()
  {
    _v[_stringLen] = '\0';
  }

  // Data buffer.
  //
  char _v[Dimension];

  int _stringLen;
};

} // end namespace util

#endif // ndef __LIB_TS_APPENDSTR_H__
