.. Licensed to the Apache Software Foundation (ASF) under one or more
   contributor license agreements.  See the NOTICE file distributed
   with this work for additional information regarding copyright
   ownership.  The ASF licenses this file to you under the Apache
   License, Version 2.0 (the "License"); you may not use this file
   except in compliance with the License.  You may obtain a copy of
   the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
   implied.  See the License for the specific language governing
   permissions and limitations under the License.

.. include:: ../../../common.defs

TSOverridableConfigKey
**********************

Synopsis
========

.. code-block:: cpp

    #include <ts/apidefs.h>

.. cpp:enum:: TSOverridableConfigKey

Enumeration Members
===================

.. cpp:enumerator:: TS_CONFIG_URL_REMAP_PRISTINE_HOST_HDR
.. cpp:enumerator:: TS_CONFIG_HTTP_CHUNKING_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_NEGATIVE_CACHING_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_NEGATIVE_CACHING_LIFETIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_WHEN_TO_REVALIDATE
.. cpp:enumerator:: TS_CONFIG_HTTP_KEEP_ALIVE_ENABLED_IN
.. cpp:enumerator:: TS_CONFIG_HTTP_KEEP_ALIVE_ENABLED_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_KEEP_ALIVE_POST_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_SERVER_SESSION_SHARING_MATCH
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_RECV_BUFFER_SIZE_OUT
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_SEND_BUFFER_SIZE_OUT
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_OPTION_FLAG_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_FORWARD_PROXY_AUTH_TO_PARENT
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_REMOVE_FROM
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_REMOVE_REFERER
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_REMOVE_USER_AGENT
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_REMOVE_COOKIE
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_REMOVE_CLIENT_IP
.. cpp:enumerator:: TS_CONFIG_HTTP_ANONYMIZE_INSERT_CLIENT_IP
.. cpp:enumerator:: TS_CONFIG_HTTP_RESPONSE_SERVER_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_INSERT_SQUID_X_FORWARDED_FOR
.. cpp:enumerator:: TS_CONFIG_HTTP_SEND_HTTP11_REQUESTS
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_HTTP
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_CLIENT_NO_CACHE
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_CLIENT_CC_MAX_AGE
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IMS_ON_CLIENT_NO_CACHE
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_SERVER_NO_CACHE
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_CACHE_RESPONSES_TO_COOKIES
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_AUTHENTICATION
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_CACHE_URLS_THAT_LOOK_DYNAMIC
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_REQUIRED_HEADERS
.. cpp:enumerator:: TS_CONFIG_HTTP_INSERT_REQUEST_VIA_STR
.. cpp:enumerator:: TS_CONFIG_HTTP_INSERT_RESPONSE_VIA_STR
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_HEURISTIC_MIN_LIFETIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_HEURISTIC_MAX_LIFETIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_GUARANTEED_MIN_LIFETIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_GUARANTEED_MAX_LIFETIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_MAX_STALE_AGE
.. cpp:enumerator:: TS_CONFIG_HTTP_KEEP_ALIVE_NO_ACTIVITY_TIMEOUT_IN
.. cpp:enumerator:: TS_CONFIG_HTTP_KEEP_ALIVE_NO_ACTIVITY_TIMEOUT_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_TRANSACTION_NO_ACTIVITY_TIMEOUT_IN
.. cpp:enumerator:: TS_CONFIG_HTTP_TRANSACTION_NO_ACTIVITY_TIMEOUT_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_TRANSACTION_ACTIVE_TIMEOUT_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_ORIGIN_MAX_CONNECTIONS
.. cpp:enumerator:: TS_CONFIG_HTTP_CONNECT_ATTEMPTS_MAX_RETRIES
.. cpp:enumerator:: TS_CONFIG_HTTP_CONNECT_ATTEMPTS_MAX_RETRIES_DOWN_SERVER
.. cpp:enumerator:: TS_CONFIG_HTTP_CONNECT_ATTEMPTS_RR_RETRIES
.. cpp:enumerator:: TS_CONFIG_HTTP_CONNECT_ATTEMPTS_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_POST_CONNECT_ATTEMPTS_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_DOWN_SERVER_CACHE_TIME
.. cpp:enumerator:: TS_CONFIG_HTTP_DOC_IN_CACHE_SKIP_DNS
.. cpp:enumerator:: TS_CONFIG_HTTP_BACKGROUND_FILL_ACTIVE_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_RESPONSE_SERVER_STR
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_HEURISTIC_LM_FACTOR
.. cpp:enumerator:: TS_CONFIG_HTTP_BACKGROUND_FILL_COMPLETED_THRESHOLD
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_PACKET_MARK_OUT
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_PACKET_TOS_OUT
.. cpp:enumerator:: TS_CONFIG_HTTP_INSERT_AGE_IN_RESPONSE
.. cpp:enumerator:: TS_CONFIG_HTTP_CHUNKING_SIZE
.. cpp:enumerator:: TS_CONFIG_HTTP_FLOW_CONTROL_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_FLOW_CONTROL_LOW_WATER_MARK
.. cpp:enumerator:: TS_CONFIG_HTTP_FLOW_CONTROL_HIGH_WATER_MARK
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_RANGE_LOOKUP
.. cpp:enumerator:: TS_CONFIG_HTTP_DEFAULT_BUFFER_SIZE
.. cpp:enumerator:: TS_CONFIG_HTTP_DEFAULT_BUFFER_WATER_MARK
.. cpp:enumerator:: TS_CONFIG_HTTP_REQUEST_HEADER_MAX_SIZE
.. cpp:enumerator:: TS_CONFIG_HTTP_RESPONSE_HEADER_MAX_SIZE
.. cpp:enumerator:: TS_CONFIG_HTTP_NEGATIVE_REVALIDATING_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_NEGATIVE_REVALIDATING_LIFETIME
.. cpp:enumerator:: TS_CONFIG_SSL_HSTS_MAX_AGE
.. cpp:enumerator:: TS_CONFIG_SSL_HSTS_INCLUDE_SUBDOMAINS
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_OPEN_READ_RETRY_TIME
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_MAX_OPEN_READ_RETRIES
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_RANGE_WRITE
.. cpp:enumerator:: TS_CONFIG_HTTP_POST_CHECK_CONTENT_LENGTH_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_GLOBAL_USER_AGENT_HEADER
.. cpp:enumerator:: TS_CONFIG_HTTP_AUTH_SERVER_SESSION_PRIVATE
.. cpp:enumerator:: TS_CONFIG_HTTP_SLOW_LOG_THRESHOLD
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_GENERATION
.. cpp:enumerator:: TS_CONFIG_BODY_FACTORY_TEMPLATE_BASE
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_OPEN_WRITE_FAIL_ACTION
.. cpp:enumerator:: TS_CONFIG_HTTP_NUMBER_OF_REDIRECTIONS
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_MAX_OPEN_WRITE_RETRIES
.. cpp:enumerator:: TS_CONFIG_HTTP_REDIRECT_USE_ORIG_CACHE_KEY
.. cpp:enumerator:: TS_CONFIG_HTTP_REQUEST_BUFFER_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_ATTACH_SERVER_SESSION_TO_CLIENT
.. cpp:enumerator:: TS_CONFIG_HTTP_MAX_PROXY_CYCLES
.. cpp:enumerator:: TS_CONFIG_HTTP_ORIGIN_MAX_CONNECTIONS_QUEUE
.. cpp:enumerator:: TS_CONFIG_WEBSOCKET_NO_ACTIVITY_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_WEBSOCKET_ACTIVE_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_UNCACHEABLE_REQUESTS_BYPASS_PARENT
.. cpp:enumerator:: TS_CONFIG_HTTP_PARENT_PROXY_TOTAL_CONNECT_ATTEMPTS
.. cpp:enumerator:: TS_CONFIG_HTTP_TRANSACTION_ACTIVE_TIMEOUT_IN
.. cpp:enumerator:: TS_CONFIG_SRV_ENABLED
.. cpp:enumerator:: TS_CONFIG_HTTP_FORWARD_CONNECT_METHOD
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_CERT_FILENAME
.. cpp:enumerator:: TS_CONFIG_SSL_CERT_FILEPATH
.. cpp:enumerator:: TS_CONFIG_PARENT_FAILURES_UPDATE_HOSTDB
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_ACCEPT_MISMATCH
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_ACCEPT_LANGUAGE_MISMATCH
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_ACCEPT_ENCODING_MISMATCH
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_ACCEPT_CHARSET_MISMATCH
.. cpp:enumerator:: TS_CONFIG_HTTP_PARENT_PROXY_FAIL_THRESHOLD
.. cpp:enumerator:: TS_CONFIG_HTTP_PARENT_PROXY_RETRY_TIME
.. cpp:enumerator:: TS_CONFIG_HTTP_PER_PARENT_CONNECT_ATTEMPTS
.. cpp:enumerator:: TS_CONFIG_HTTP_PARENT_CONNECT_ATTEMPT_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_NORMALIZE_AE
.. cpp:enumerator:: TS_CONFIG_HTTP_INSERT_FORWARDED
.. cpp:enumerator:: TS_CONFIG_HTTP_ALLOW_MULTI_RANGE
.. cpp:enumerator:: TS_CONFIG_HTTP_ALLOW_HALF_OPEN
.. cpp:enumerator:: TS_CONFIG_HTTP_PER_SERVER_CONNECTION_MAX
.. cpp:enumerator:: TS_CONFIG_HTTP_PER_SERVER_CONNECTION_MATCH
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_VERIFY_SERVER_POLICY
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_VERIFY_SERVER_PROPERTIES
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_SNI_POLICY
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_PRIVATE_KEY_FILENAME
.. cpp:enumerator:: TS_CONFIG_SSL_CLIENT_CA_CERT_FILENAME
.. cpp:enumerator:: TS_CONFIG_HTTP_HOST_RESOLUTION_PREFERENCE
.. cpp:enumerator:: TS_CONFIG_PLUGIN_VC_DEFAULT_BUFFER_INDEX
.. cpp:enumerator:: TS_CONFIG_PLUGIN_VC_DEFAULT_BUFFER_WATER_MARK
.. cpp:enumerator:: TS_CONFIG_NET_SOCK_NOTSENT_LOWAT
.. cpp:enumerator:: TS_CONFIG_BODY_FACTORY_RESPONSE_SUPPRESSION_MODE
.. cpp:enumerator:: TS_CONFIG_NET_DEFAULT_INACTIVITY_TIMEOUT
.. cpp:enumerator:: TS_CONFIG_HTTP_NO_DNS_JUST_FORWARD_TO_PARENT
.. cpp:enumerator:: TS_CONFIG_HTTP_CACHE_IGNORE_QUERY


Description
===========

This is an enumeration for all of the configuration values that are overridable per transaction. See
:ref:`ts-overridable-config` for the relationship between these values and configuration values.
