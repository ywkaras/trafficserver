/** @file

    Class for handling "views" of a buffer. Views presume the memory for the
    buffer is managed elsewhere and allow efficient access to segments of the
    buffer without copies. Views are read only as the view doesn't own the
    memory. Along with generic buffer methods are specialized methods to support
    better string parsing, particularly token based parsing.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one or more
    contributor license agreements.  See the NOTICE file distributed with this
    work for additional information regarding copyright ownership.  The ASF
    licenses this file to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
    License for the specific language governing permissions and limitations under
    the License.
*/
#include <tscpp/util/test_vague_linkage.h>

#include <tscore/ink_assert.h>

#define ALWAYS_ASSERT(EX) \
  ink_release_assert(EX); \
  ink_assert(EX)

namespace vague_linkage
{
void
callback()
{
  ALWAYS_ASSERT(vague_linkage::dflt == 2);
  ALWAYS_ASSERT(vague_linkage::inited == 3);
  ALWAYS_ASSERT(vague_linkage::dflt_f() == 2);
  ALWAYS_ASSERT(vague_linkage::inited_f() == 3);
}
} // namespace vague_linkage
