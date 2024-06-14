'''
Test that fails client with modified ja3_fingerprint plugin.
'''
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

import os

Test.Summary = '''
Test that fails client with modified ja3_fingerprint plugin.
'''

server = Test.MakeOriginServer("server")

request_header = {"headers": "GET / HTTP/1.1\r\nHost: does.not,matter\r\n\r\n", "timestamp": "1469733493.993", "body": ""}
response_header = {"headers": "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n", "timestamp": "1469733493.993", "body": ""}
server.addResponse("sessionlog.json", request_header, response_header)

dns = Test.MakeDNServer("dns")

# Disable the cache to make sure each request is forwarded to the origin
# server.
ts = Test.MakeATSProcess("ts", enable_tls=True, enable_cache=False)

ts.addDefaultSSLFiles()

ts.Disk.records_config.update(
    {
        'proxy.config.diags.debug.enabled': 1,
        'proxy.config.diags.debug.tags': 'ja3_fingerprint|http',
        'proxy.config.url_remap.pristine_host_hdr': 1,  # Retain Host header in original incoming client request.
        'proxy.config.proxy_name': 'Poxy_Proxy',  # This will be the server name.
        'proxy.config.ssl.server.cert.path': '{0}'.format(ts.Variables.SSLDir),
        'proxy.config.ssl.server.private_key.path': '{0}'.format(ts.Variables.SSLDir),
        'proxy.config.dns.nameservers': f"127.0.0.1:{dns.Variables.Port}",
        'proxy.config.dns.resolv_conf': "NULL",
    })

ts.Disk.ssl_multicert_config.AddLine('dest_ip=* ssl_cert_name=server.pem ssl_key_name=server.key')

ts.Disk.plugin_config.AddLine('ja3_fingerprint.so')

ts.Disk.remap_config.AddLine('map https://www.yada.com http://127.0.0.1:{0}'.format(server.Variables.Port))

# TLS curl
tr = Test.AddTestRun()
tr.Processes.Default.StartBefore(ts)
tr.Processes.Default.StartBefore(server)
tr.Processes.Default.Command = (
    f'curl --verbose --ipv4 --http1.1 --insecure --header "Host: www.yada.com" https://localhost:{ts.Variables.ssl_port}'
    ' & SPAWN=$$! ; while [[ ! -f /tmp/ja3 ]] ; do sleep 1 ; done ; kill $$SPAWN ; sleep 5 ; rm -rf /tmp/ja3')
tr.Processes.Default.ReturnCode = 0

# TLS curl
tr = Test.AddTestRun()
tr.Processes.Default.Command = (
    f'h2load -n200 -m200 --connect-to=localhost:{ts.Variables.ssl_port} https://www.yada.com/'
    ' & SPAWN=$$! ; while [[ ! -f /tmp/ja3 ]] ; do sleep 1 ; done ; kill -9 $$SPAWN ; sleep 5 ; rm -rf /tmp/ja3')
tr.Processes.Default.ReturnCode = 0
