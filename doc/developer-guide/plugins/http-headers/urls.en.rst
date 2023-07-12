.. Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.

.. include:: ../../../common.defs

.. _developer-plugins-http-headers-urls:

URLs
****

API URL functions provide access to URL data stored in marshal buffers.
The URL functions can create, copy, retrieve or delete entire URLs; they
can also retrieve or modify parts of URLs, such as port or scheme
information.

The general form of an Internet URL is::

       scheme://user:password@host:port/stuff

The URL data structure includes support for two specific types of
internet URLs. HTTP URLs have the form::

       http://user:password@host:port/path;params?query#fragment

The URL port is stored as integer. All remaining parts of the URL
(scheme, user, etc.) are stored as strings. Traffic Server URL functions
are named according to the portion of the URL on which they operate. For
instance, the function that retrieves the host portion of a URL is named
``TSUrlHostGet``.

To facilitate fast comparisons and reduce storage size, Traffic Server
defines several preallocated scheme names.

"file"
   .. cpp:var:: char const * TS_URL_SCHEME_FILE
   .. cpp:var:: int TS_URL_LEN_FILE

"ftp"
   .. cpp:var:: char const * TS_URL_SCHEME_FTP
   .. cpp:var:: int TS_URL_LEN_FTP

"gopher"
   .. cpp:var:: char const * TS_URL_SCHEME_GOPHER
   .. cpp:var:: int TS_URL_LEN_GOPHER

"http"
   .. cpp:var:: char const * TS_URL_SCHEME_HTTP
   .. cpp:var:: int TS_URL_LEN_HTTP

"https"
   .. cpp:var:: char const * TS_URL_SCHEME_HTTPS
   .. cpp:var:: int TS_URL_LEN_HTTPS

"mailto"
   .. cpp:var:: char const * TS_URL_SCHEME_MAILTO
   .. cpp:var:: int TS_URL_LEN_MAILTO

"news"
   .. cpp:var:: char const * TS_URL_SCHEME_NEWS
   .. cpp:var:: int TS_URL_LEN_NEWS

"nntp"
   .. cpp:var:: char const * TS_URL_SCHEME_NNTP
   .. cpp:var:: int TS_URL_LEN_NNTP

"prospero"
   .. cpp:var:: char const * TS_URL_SCHEME_PROSPERO
   .. cpp:var:: int TS_URL_LEN_PROSPERO

"telnet"
   .. cpp:var:: char const * TS_URL_SCHEME_TELNET
   .. cpp:var:: int TS_URL_LEN_TELNET

"wais"
   .. cpp:var:: char const * TS_URL_SCHEME_WAIS
   .. cpp:var:: int TS_URL_LEN_WAIS

"ws"
   .. cpp:var:: char const * TS_URL_SCHEME_WS
   .. cpp:var:: int TS_URL_LEN_WS

"wss"
   .. cpp:var:: char const * TS_URL_SCHEME_WSS
   .. cpp:var:: int TS_URL_LEN_WSS

The scheme names above are defined in ``apidefs.h``. When Traffic Server sets the scheme portion of
the URL (or any portion for that matter), it quickly checks to see if the new value is one of the
known values. If it is, then it stores a pointer into a global table (instead of storing the known
value in the marshal buffer). The scheme values listed above are also pointers into this table. This
allows simple pointer comparison of the value returned from ``TSUrlSchemeGet`` or
``TSUrlRawSchemeGet`` with one of the values listed above. You should use the Traffic Server-defined
values when referring to one of the known schemes, since doing so can prevent the possibility of
spelling errors.

Traffic Server **URL functions** are listed below:

:cpp:func:`TSUrlClone`
:cpp:func:`TSUrlCopy`
:cpp:func:`TSUrlCreate`
:cpp:func:`TSUrlPrint`
:cpp:func:`TSUrlFtpTypeGet`
:cpp:func:`TSUrlFtpTypeSet`
:cpp:func:`TSUrlHostGet`
:cpp:func:`TSUrlHostSet`
:cpp:func:`TSUrlHttpFragmentGet`
:cpp:func:`TSUrlHttpFragmentSet`
:cpp:func:`TSUrlHttpParamsGet`
:cpp:func:`TSUrlHttpParamsSet`
:cpp:func:`TSUrlHttpQueryGet`
:cpp:func:`TSUrlHttpQuerySet`
:cpp:func:`TSUrlLengthGet`
:cpp:func:`TSUrlParse`
:cpp:func:`TSUrlPasswordGet`
:cpp:func:`TSUrlPasswordSet`
:cpp:func:`TSUrlPathGet`
:cpp:func:`TSUrlPathSet`
:cpp:func:`TSUrlPortGet`
:cpp:func:`TSUrlRawPortGet`
:cpp:func:`TSUrlPortSet`
:cpp:func:`TSUrlSchemeGet`
:cpp:func:`TSUrlSchemeSet`
:cpp:func:`TSUrlStringGet`
:cpp:func:`TSUrlUserGet`
:cpp:func:`TSUrlUserSet`
