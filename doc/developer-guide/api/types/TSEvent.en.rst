
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

TSEvent
*******

Synopsis
========

.. code-block:: cpp

    #include <ts/apidefs.h>

.. cpp:type:: TSEvent

Enum typedef defining the possible events which may be passed to a continuation
callback.

Enumeration Members
===================

.. cpp:enumerator:: TS_EVENT_NONE

.. cpp:enumerator:: TS_EVENT_IMMEDIATE

.. cpp:enumerator:: TS_EVENT_TIMEOUT

.. cpp:enumerator:: TS_EVENT_ERROR

.. cpp:enumerator:: TS_EVENT_CONTINUE

.. cpp:enumerator:: TS_EVENT_VCONN_READ_READY

.. cpp:enumerator:: TS_EVENT_VCONN_WRITE_READY

.. cpp:enumerator:: TS_EVENT_VCONN_READ_COMPLETE

.. cpp:enumerator:: TS_EVENT_VCONN_WRITE_COMPLETE

.. cpp:enumerator:: TS_EVENT_VCONN_EOS

.. cpp:enumerator:: TS_EVENT_VCONN_INACTIVITY_TIMEOUT

.. cpp:enumerator:: TS_EVENT_VCONN_ACTIVE_TIMEOUT

.. cpp:enumerator:: TS_EVENT_VCONN_START

   An inbound connection has started.

.. cpp:enumerator:: TS_EVENT_VCONN_CLOSE

   An inbound connection has closed.

.. cpp:enumerator:: TS_EVENT_OUTBOUND_START

   An outbound connection has started.

.. cpp:enumerator:: TS_EVENT_OUTBOUND_CLOSE

   An outbound connection has closed.

.. cpp:enumerator:: TS_EVENT_NET_CONNECT

.. cpp:enumerator:: TS_EVENT_NET_CONNECT_FAILED

.. cpp:enumerator:: TS_EVENT_NET_ACCEPT

.. cpp:enumerator:: TS_EVENT_NET_ACCEPT_FAILED

.. cpp:enumerator:: TS_EVENT_INTERNAL_206

.. cpp:enumerator:: TS_EVENT_INTERNAL_207

.. cpp:enumerator:: TS_EVENT_INTERNAL_208

.. cpp:enumerator:: TS_EVENT_INTERNAL_209

.. cpp:enumerator:: TS_EVENT_INTERNAL_210

.. cpp:enumerator:: TS_EVENT_INTERNAL_211

.. cpp:enumerator:: TS_EVENT_INTERNAL_212

.. cpp:enumerator:: TS_EVENT_HOST_LOOKUP

.. cpp:enumerator:: TS_EVENT_CACHE_OPEN_READ

.. cpp:enumerator:: TS_EVENT_CACHE_OPEN_READ_FAILED

.. cpp:enumerator:: TS_EVENT_CACHE_OPEN_WRITE

.. cpp:enumerator:: TS_EVENT_CACHE_OPEN_WRITE_FAILED

.. cpp:enumerator:: TS_EVENT_CACHE_REMOVE

.. cpp:enumerator:: TS_EVENT_CACHE_REMOVE_FAILED

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN_FAILED

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN_OBJECT

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN_OPERATION_BLOCKED

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN_OPERATION_FAILED

.. cpp:enumerator:: TS_EVENT_CACHE_SCAN_DONE

.. cpp:enumerator:: TS_EVENT_CACHE_LOOKUP

.. cpp:enumerator:: TS_EVENT_CACHE_READ

.. cpp:enumerator:: TS_EVENT_CACHE_DELETE

.. cpp:enumerator:: TS_EVENT_CACHE_WRITE

.. cpp:enumerator:: TS_EVENT_CACHE_WRITE_HEADER

.. cpp:enumerator:: TS_EVENT_CACHE_CLOSE

.. cpp:enumerator:: TS_EVENT_CACHE_LOOKUP_READY

.. cpp:enumerator:: TS_EVENT_CACHE_LOOKUP_COMPLETE

.. cpp:enumerator:: TS_EVENT_CACHE_READ_READY

.. cpp:enumerator:: TS_EVENT_CACHE_READ_COMPLETE

.. cpp:enumerator:: TS_EVENT_INTERNAL_1200

.. cpp:enumerator:: TS_AIO_EVENT_DONE

.. cpp:enumerator:: TS_EVENT_HTTP_CONTINUE

.. cpp:enumerator:: TS_EVENT_HTTP_ERROR

.. cpp:enumerator:: TS_EVENT_HTTP_READ_REQUEST_HDR

.. cpp:enumerator:: TS_EVENT_HTTP_OS_DNS

.. cpp:enumerator:: TS_EVENT_HTTP_SEND_REQUEST_HDR

.. cpp:enumerator:: TS_EVENT_HTTP_READ_CACHE_HDR

.. cpp:enumerator:: TS_EVENT_HTTP_READ_RESPONSE_HDR

.. cpp:enumerator:: TS_EVENT_HTTP_SEND_RESPONSE_HDR

.. cpp:enumerator:: TS_EVENT_HTTP_REQUEST_TRANSFORM

.. cpp:enumerator:: TS_EVENT_HTTP_RESPONSE_TRANSFORM

.. cpp:enumerator:: TS_EVENT_HTTP_SELECT_ALT

.. cpp:enumerator:: TS_EVENT_HTTP_TXN_START

.. cpp:enumerator:: TS_EVENT_HTTP_TXN_CLOSE

.. cpp:enumerator:: TS_EVENT_HTTP_SSN_START

.. cpp:enumerator:: TS_EVENT_HTTP_SSN_CLOSE

.. cpp:enumerator:: TS_EVENT_HTTP_CACHE_LOOKUP_COMPLETE

.. cpp:enumerator:: TS_EVENT_HTTP_PRE_REMAP

.. cpp:enumerator:: TS_EVENT_HTTP_POST_REMAP

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_PORTS_INITIALIZED

   The internal data structures for the proxy ports have been initialized.

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_PORTS_READY

   The proxy ports are now open for inbound connections.

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_CACHE_READY

   The cache is ready.

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_SERVER_SSL_CTX_INITIALIZED

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_CLIENT_SSL_CTX_INITIALIZED

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_MSG

   A message from an external source has arrived.

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_TASK_THREADS_READY

   The ``ET_TASK`` threads are running.

.. cpp:enumerator:: TS_EVENT_LIFECYCLE_SHUTDOWN

   The |TS| process has is shutting down.

.. cpp:enumerator:: TS_EVENT_INTERNAL_60200

.. cpp:enumerator:: TS_EVENT_INTERNAL_60201

.. cpp:enumerator:: TS_EVENT_INTERNAL_60202

.. cpp:enumerator:: TS_EVENT_SSL_CERT

   Preparing to present a server certificate to an inbound TLS connection.

.. cpp:enumerator:: TS_EVENT_SSL_SERVERNAME

   The SNI name for an Inbound TLS connection has become available.

.. cpp:enumerator:: TS_EVENT_SSL_VERIFY_SERVER

   Outbound TLS connection certificate verification (verifying the server certificate).

.. cpp:enumerator:: TS_EVENT_SSL_VERIFY_CLIENT

   Inbound TLS connection certificate verification (verifying the client certificate).

.. cpp:enumerator:: TS_EVENT_MGMT_UPDATE

Description
===========

These are the event types used to drive continuations in the event system.

.. cpp:type:: EventType

   The basic category of an event.

.. cpp:enumerator:: EVENT_NONE

   A non-specific event.

.. cpp:enumerator:: EVENT_IMMEDIATE

   A direct event that is not based on an external event.

.. cpp:enumerator:: EVENT_INTERVAL

   An event generated by a time based event.
