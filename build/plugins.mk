#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

# plugins.mk: Common automake build variables for Traffic Server plugins.

GCC_VER_GET = $(CC) --version | tr ' ' '\\n' | grep --color=auto -F . | head -1
CLANG_VER_GET = $(CC) -dumpversion
# Assume clang if not gcc
COMP_NAME := $(shell bash -c "if $(CC) --version | grep --color=auto -F gcc ; then echo gcc ; else echo clang ; fi")
COMP_FULL_VER := $(shell bash -c "if $(CC) --version | grep --color=auto -F gcc ; then $(GCC_VER_GET) ; else $(CLANG_VER_GET) ; fi")
COMP_MAJOR_VER := $(shell bash -c "echo $(COMP_FULL_VER) | tr '.' '\n' | head -1")
COMP_MINOR_VER := $(shell bash -c "echo $(COMP_FULL_VER) | tr '.' '\n' | head -2 | tail -1")

# Default plugin LDFLAGS. We don't use TS_PLUGIN_LDFLAGS because
# that is an automake canonical variable name.
TS_PLUGIN_LD_FLAGS = \
  -module \
  -shared \
  -avoid-version \
  -export-symbols-regex '^(TSRemapInit|TSRemapDone|TSRemapDoRemap|TSRemapNewInstance|TSRemapDeleteInstance|TSRemapOSResponse|TSPluginInit_@TS_VERSION_MAJOR@)_$(COMP_NAME)_$(COMP_MAJOR_VER)_$(COMP_MINOR_VER)$$'

TS_PLUGIN_CPPFLAGS = \
  -I$(abs_top_builddir)/proxy/api \
  -I$(abs_top_srcdir)/proxy/api \
  -I$(abs_top_srcdir)/include/cppapi/include \
  -I$(abs_top_builddir)/lib/cppapi/include \
  -I$(abs_top_srcdir)/include \
  -I$(abs_top_srcdir)/lib

# Provide a default AM_CPPFLAGS. Automake handles this correctly, but libtool
# throws an error if we try to do the same with AM_LDFLAGS. Hence, we provide
# TS_PLUGIN variables that can be used to construct the necessary automake
# syntax.
AM_CPPFLAGS += $(TS_PLUGIN_CPPFLAGS)
pkglibdir = $(pkglibexecdir)
