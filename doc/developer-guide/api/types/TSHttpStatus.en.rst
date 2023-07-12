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

TSHttpStatus
************

Synopsis
========

.. code-block:: cpp

    #include <ts/apidefs.h>

.. cpp:enum:: TSHttpStatus

This set of enums represents the possible HTTP types that can be
assigned to an HTTP header.

When a header is created with :func:`TSHttpHdrCreate`, it is
automatically assigned a type of :cpp:enumerator:`TS_HTTP_TYPE_UNKNOWN`.  You
can modify the HTTP type ONCE after it the header is created, using
:func:`TSHttpHdrTypeSet`.  After setting the HTTP type once, you
cannot set it again.  Use :func:`TSHttpHdrTypeGet` to obtain the
:type:`TSHttpType` of an HTTP header.

Enumeration Members
===================

.. cpp:enumerator:: TS_HTTP_STATUS_NONE

.. cpp:enumerator:: TS_HTTP_STATUS_CONTINUE

.. cpp:enumerator:: TS_HTTP_STATUS_SWITCHING_PROTOCOL

.. cpp:enumerator:: TS_HTTP_STATUS_EARLY_HINTS

.. cpp:enumerator:: TS_HTTP_STATUS_OK

.. cpp:enumerator:: TS_HTTP_STATUS_CREATED

.. cpp:enumerator:: TS_HTTP_STATUS_ACCEPTED

.. cpp:enumerator:: TS_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION

.. cpp:enumerator:: TS_HTTP_STATUS_NO_CONTENT

.. cpp:enumerator:: TS_HTTP_STATUS_RESET_CONTENT

.. cpp:enumerator:: TS_HTTP_STATUS_PARTIAL_CONTENT

.. cpp:enumerator:: TS_HTTP_STATUS_MULTI_STATUS

.. cpp:enumerator:: TS_HTTP_STATUS_ALREADY_REPORTED

.. cpp:enumerator:: TS_HTTP_STATUS_IM_USED

.. cpp:enumerator:: TS_HTTP_STATUS_MULTIPLE_CHOICES

.. cpp:enumerator:: TS_HTTP_STATUS_MOVED_PERMANENTLY

.. cpp:enumerator:: TS_HTTP_STATUS_MOVED_TEMPORARILY

.. cpp:enumerator:: TS_HTTP_STATUS_SEE_OTHER

.. cpp:enumerator:: TS_HTTP_STATUS_NOT_MODIFIED

.. cpp:enumerator:: TS_HTTP_STATUS_USE_PROXY

.. cpp:enumerator:: TS_HTTP_STATUS_TEMPORARY_REDIRECT

.. cpp:enumerator:: TS_HTTP_STATUS_PERMANENT_REDIRECT

.. cpp:enumerator:: TS_HTTP_STATUS_BAD_REQUEST

.. cpp:enumerator:: TS_HTTP_STATUS_UNAUTHORIZED

.. cpp:enumerator:: TS_HTTP_STATUS_PAYMENT_REQUIRED

.. cpp:enumerator:: TS_HTTP_STATUS_FORBIDDEN

.. cpp:enumerator:: TS_HTTP_STATUS_NOT_FOUND

.. cpp:enumerator:: TS_HTTP_STATUS_METHOD_NOT_ALLOWED

.. cpp:enumerator:: TS_HTTP_STATUS_NOT_ACCEPTABLE

.. cpp:enumerator:: TS_HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED

.. cpp:enumerator:: TS_HTTP_STATUS_REQUEST_TIMEOUT

.. cpp:enumerator:: TS_HTTP_STATUS_CONFLICT

.. cpp:enumerator:: TS_HTTP_STATUS_GONE

.. cpp:enumerator:: TS_HTTP_STATUS_LENGTH_REQUIRED

.. cpp:enumerator:: TS_HTTP_STATUS_PRECONDITION_FAILED

.. cpp:enumerator:: TS_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE

.. cpp:enumerator:: TS_HTTP_STATUS_REQUEST_URI_TOO_LONG

.. cpp:enumerator:: TS_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE

.. cpp:enumerator:: TS_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE

.. cpp:enumerator:: TS_HTTP_STATUS_EXPECTATION_FAILED

.. cpp:enumerator:: TS_HTTP_STATUS_UNPROCESSABLE_ENTITY

.. cpp:enumerator:: TS_HTTP_STATUS_LOCKED

.. cpp:enumerator:: TS_HTTP_STATUS_FAILED_DEPENDENCY

.. cpp:enumerator:: TS_HTTP_STATUS_UPGRADE_REQUIRED

.. cpp:enumerator:: TS_HTTP_STATUS_PRECONDITION_REQUIRED

.. cpp:enumerator:: TS_HTTP_STATUS_TOO_MANY_REQUESTS

.. cpp:enumerator:: TS_HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE

.. cpp:enumerator:: TS_HTTP_STATUS_INTERNAL_SERVER_ERROR

.. cpp:enumerator:: TS_HTTP_STATUS_NOT_IMPLEMENTED

.. cpp:enumerator:: TS_HTTP_STATUS_BAD_GATEWAY

.. cpp:enumerator:: TS_HTTP_STATUS_SERVICE_UNAVAILABLE

.. cpp:enumerator:: TS_HTTP_STATUS_GATEWAY_TIMEOUT

.. cpp:enumerator:: TS_HTTP_STATUS_HTTPVER_NOT_SUPPORTED

.. cpp:enumerator:: TS_HTTP_STATUS_VARIANT_ALSO_NEGOTIATES

.. cpp:enumerator:: TS_HTTP_STATUS_INSUFFICIENT_STORAGE

.. cpp:enumerator:: TS_HTTP_STATUS_LOOP_DETECTED

.. cpp:enumerator:: TS_HTTP_STATUS_NOT_EXTENDED

.. cpp:enumerator:: TS_HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED

Description
===========
