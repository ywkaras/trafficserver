.. Licensed to the Apache Software Foundation (ASF) under one or more
   contributor license agreements.  See the NOTICE file distributed
   with this work for additional information regarding copyright
   ownership.  The ASF licenses this file to you under the Apache
   License, Version 2.0 (the "License"); you may not use this file
   except in compliance with the License.  You may obtain a copy of
   the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
   implied.  See the License for the specific language governing
   permissions and limitations under the License.

.. include:: ../../../common.defs

.. default-domain:: cpp

.. _ts-lifecycle-hook-add:

TSLifecycleHookAdd
******************

Synopsis
========

.. code-block:: cpp

    #include <ts/ts.h>

.. function:: void TSLifecycleHookAdd(TSLifecycleHookID id, TSCont contp)

Description
===========

:func:`TSLifecycleHookAdd` adds :arg:`contp` to the list of lifecycle hooks
specified by :arg:`id` (see :cpp:enum:`TSLifecycleHookID`). Lifecycle hooks are based on the Traffic Server
process, not on any specific transaction or session. These will typically be
called only once during the execution of the Traffic Server process and
therefore should be added in :func:`TSPluginInit` (which could itself be
considered a lifecycle hook). Unlike other hooks, lifecycle hooks may not have a
well defined ordering and use of them should not assume that one of the hooks
is always called before another unless specifically mentioned.

Types
=====

.. cpp:struct:: TSPluginMsg

   The data for the plugin message event :cpp:enumerator:`TS_EVENT_LIFECYCLE_MSG`.

   .. cpp:var:: const char * tag

      The tag of the message. This is a null terminated string.

   .. cpp:var:: const void * data

      Message data (payload). This is a raw slab of bytes - no structure is guaranteed.

   .. cpp:var:: size_t data_size

      The number of valid bytes pointed at by :var:`data`.

Ordering
========

:cpp:enumerator:`TSLifecycleHookID::TS_LIFECYCLE_PORTS_INITIALIZED_HOOK` will always be called before
:cpp:enumerator:`TSLifecycleHookID::TS_LIFECYCLE_PORTS_READY_HOOK`.

Examples
========

The following example demonstrates how to correctly use
:func:`TSNetAcceptNamedProtocol`, which requires the proxy ports to be
initialized and therefore does not work if called from :func:`TSPluginInit`
directly.

.. code-block:: cpp

   #include <ts/ts.h>

   #define SSL_PROTOCOL_NAME "whatever"

   static int
   ssl_proto_handler(TSCont contp, TSEvent event, void* data)
   {
      /// Do named protocol handling.
   }

   static int
   local_ssl_init(TSCont contp, TSEvent event, void * edata)
   {
      if (TS_EVENT_LIFECYCLE_PORTS_INITIALIZED == event) { // just to be safe.
         TSNetAcceptNamedProtocol(
            TSContCreate(ssl_proto_handler, TSMutexCreate()),
            SSL_PROTOCOL_NAME
         );
      }
      return 0;
   }

   void
   TSPluginInit (int argc, const char * argv[])
   {
      TSLifecycleHookAdd(TS_LIFECYCLE_PORTS_INITIALIZED_HOOK, TSContCreate(local_ssl_init, NULL));
   }

History
=======

Lifecycle hooks were introduced to solve process initialization ordering issues
(TS-1487). Different API calls required different modules of |TS| to be
initialized for the call to work, but others did not work that late in
initialization, which was problematic because all of them could effectively
only be called from :func:`TSPluginInit` . The solution was to move
:func:`TSPluginInit` as early as possible in the process initialization and
provide hooks for API calls that needed to be invoked later which served
essentially as additional plugin initialization points.

See Also
========

:manpage:`TSAPI(3ts)`, :manpage:`TSContCreate(3ts)`
