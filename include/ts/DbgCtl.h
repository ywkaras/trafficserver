/** @file

  DbgCtl class header file.

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

class DiagsConfigState;

// For use with TSDbg() or Dbg().
//
class DbgCtl
{
public:
  // Tag is a debug tag.  Debug output associated with this control will be output when debug output
  // is enabled globally, and the tag matches the configured debug tag regular expression.
  //
  DbgCtl(char const *tag) : _ptr{_new_reference(tag)} {}

  ~DbgCtl() { _rm_reference(); }

  bool
  on() const
  {
    return _ptr->on != 0;
  }

  char const *
  tag() const
  {
    return _ptr->tag;
  }

  static bool
  global_on()
  {
    return _global_on;
  }

  // Call this when the compiled regex to enable tags may have changed.  Should not be called in plugins.
  //
  static void update();

  // For use in Dbg() macro only.
  //
  static void print(const char *tag, const char *format_str, ...);

private:
  struct _Data {
    char volatile on; // Flag
    char const *tag;
  };

  Data const *const _ptr;

  static const Data *_new_reference(char const *tag);

  static void _rm_reference();

  static bool _global_on;

  class _RegistryAccessor;
};
