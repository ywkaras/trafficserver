'''
Verify support of external log rotation via SIGUSR2.
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


class Sigusr2Test:
    """
    Handle this test-specific Traffic Server configuration.
    """

    __ts_counter = 1
    __server = None

    def __init__(self):
        self.server = self.__configure_server()
        self.ts = self.__configure_traffic_manager()

    def __configure_traffic_manager(self):
        name = "ts{}".format(Sigusr2Test.__ts_counter)
        Sigusr2Test.__ts_counter += 1
        self.ts = Test.MakeATSProcess(name, command="traffic_manager")
        self.ts.Disk.records_config.update({
            'proxy.config.http.wait_for_cache': 1,
            'proxy.config.diags.debug.enabled': 1,
            'proxy.config.diags.debug.tags': 'log',

            # All log rotation should be handled externally.
            'proxy.config.log.rolling_enabled': 0,
            'proxy.config.log.auto_delete_rolled_files': 0,
            })

        # For this test, more important than the listening port is the existence of the
        # log files. In particular, it can take a few seconds for traffic_manager to
        # open diags.log.
        self.diags_log = self.ts.Disk.diags_log.AbsPath
        self.ts.Ready = When.FileExists(self.diags_log)

        # Add content handles for the rotated logs.
        self.rotated_diags_log = self.diags_log + "_old"
        self.ts.Disk.File(self.rotated_diags_log, id="diags_log_old")

        self.log_dir = os.path.dirname(self.diags_log)
        self.manager_log = os.path.join(self.log_dir, "manager.log")
        self.rotated_manager_log = self.manager_log + "_old"
        self.ts.Disk.File(self.rotated_manager_log, id="manager_log_old")

        self.ts.Disk.remap_config.AddLine(
            'map http://127.0.0.1:{0} http://127.0.0.1:{1}'.format(
                self.ts.Variables.port, self.server.Variables.Port)
        )
        self.ts.Disk.logging_yaml.AddLine('''
            logging:
              formats:
                - name: has_path
                  format: "%<cqu>: %<sssc>"
              logs:
                - filename: test_rotation
                  format: has_path
            ''')
        self.configured_log = os.path.join(self.log_dir, "test_rotation.log")
        self.ts.Disk.File(self.configured_log, id="configured_log")

        self.rotated_configured_log = self.configured_log + "_old"
        self.ts.Disk.File(self.rotated_configured_log, id="configured_log_old")
        self.ts.StartBefore(self.server)
        return self.ts

    def __configure_server(self):
        if Sigusr2Test.__server:
            return Sigusr2Test.__server
        server = Test.MakeOriginServer("server")
        Sigusr2Test.__server = server
        for path in ['/first', '/second', '/third']:
            request_header = {"headers": "GET {} HTTP/1.1\r\n"
                              "Host: does.not.matter\r\n\r\n".format(path),
                              "timestamp": "1469733493.993", "body": ""}
            response_header = {"headers": "HTTP/1.1 200 OK\r\n"
                               "Connection: close\r\n"
                               "Cache-control: max-age=85000\r\n\r\n",
                               "timestamp": "1469733493.993", "body": "xxx"}
            server.addResponse("sessionlog.json", request_header, response_header)
        return server


Test.Summary = '''
Verify support of external log rotation via SIGUSR2.
'''
#
# Test 1: Verify SIGUSR2 behavior for system logs.
#
tr1 = Test.AddTestRun("Verify system logs (manager.log, etc.) can be rotated")

# Configure Traffic Manager/Server.
diags_test = Sigusr2Test()

# Configure our rotation processes.
rotate_diags_log = tr1.Processes.Process("rotate_diags_log", "mv {} {}".format(
    diags_test.diags_log, diags_test.rotated_diags_log))
rotate_manager_log = tr1.Processes.Process("rotate_manager_log", "mv {} {}".format(
    diags_test.manager_log, diags_test.rotated_manager_log))

# When traffic_manager runs traffic_server, it uses the full path name, which includes a directory with the name
# passed to MakeATSProcess().  This command uses this to return a bash command substitution string that will
# expand to the process ID of traffic_manager.
#
def tm_pid_cmd(ts_name):
    return ( r"$$( ps -ef | grep -Fv grep | grep -F /" + ts_name +
        r"/ | grep -F /traffic_server | sed 's/[ \t][ \t]*/\t/g' | cut -f3 )"
    )

# Configure the signaling of SIGUSR2 to traffic_manaager.
tr1.Processes.Default.Command = "kill -USR2 " + tm_pid_cmd("ts1")
tr1.Processes.Default.Return = 0
tr1.Processes.Default.Ready = When.FileExists(diags_test.diags_log)

# Configure process order.
tr1.Processes.Default.StartBefore(rotate_diags_log)
rotate_diags_log.StartBefore(rotate_manager_log)
rotate_manager_log.StartBefore(diags_test.ts)
tr1.StillRunningAfter = diags_test.ts
tr1.StillRunningAfter = diags_test.server

# manager.log should have been rotated. Check for the expected content in the
# old file and the newly created file.
diags_test.ts.Disk.manager_log_old.Content += Testers.ContainsExpression(
        "received SIGUSR2, rotating the logs",
        "manager.log_old should explain that SIGUSR2 was passed to it")

diags_test.ts.Disk.manager_log.Content += Testers.ContainsExpression(
        "opened.*manager.log",
        "The new manager.log should indicate the newly opened manager.log")

# diags.log should have been rotated. The old one had the reference to traffic
# server running, this new one shouldn't. But it should indicate that the new
# diags.log was opened.
diags_test.ts.Disk.diags_log.Content += Testers.ExcludesExpression(
        "traffic server running",
        "The new diags.log should not reference the running traffic server")

diags_test.ts.Disk.diags_log.Content += Testers.ContainsExpression(
        "opened.*diags.log",
        "The new diags.log should indicate the newly opened diags.log")

#
# Test 2: Verify SIGUSR2 isn't needed for rotated configured logs.
#
tr2 = Test.AddTestRun("Verify yaml.log logs can be rotated")
configured_test = Sigusr2Test()

first_curl = tr2.Processes.Process(
        "first_curl",
        'curl "http://127.0.0.1:{0}/first" --verbose'.format(configured_test.ts.Variables.port))
first_curl_ready = tr2.Processes.Process("first_curl_ready", 'sleep 30')
# In the autest enironment, it can take more than 10 seconds for the log file to be created.
first_curl_ready.StartupTimeout = 30
first_curl_ready.Ready = When.FileExists(configured_test.configured_log)

rotate_log = tr2.Processes.Process("rotate_log_file", "mv {} {}".format(
    configured_test.configured_log, configured_test.rotated_configured_log))

second_curl = tr2.Processes.Process(
        "second_curl",
        'curl "http://127.0.0.1:{0}/second" --verbose'.format(configured_test.ts.Variables.port))

second_curl_ready = tr2.Processes.Process("second_curl_ready", 'sleep 30')
# In the autest enironment, it can take more than 10 seconds for the log file to be created.
second_curl_ready.StartupTimeout = 30
second_curl_ready.Ready = When.FileModified(configured_test.rotated_configured_log)

send_pkill = tr2.Processes.Process("Send SIGUSR2", "kill -USR2 " + tm_pid_cmd("ts2"))
send_pkill_ready = tr2.Processes.Process("send_pkill_ready", 'sleep 30')
send_pkill_ready.StartupTimeout = 30
send_pkill_ready.Ready = When.FileExists(configured_test.configured_log)

third_curl = tr2.Processes.Process(
        "third_curl",
        'curl "http://127.0.0.1:{0}/third" --verbose'.format(configured_test.ts.Variables.port))
third_curl_ready = tr2.Processes.Process("third_curl_ready", 'sleep 30')
# In the autest enironment, it can take more than 10 seconds for the log file to be created.
third_curl_ready.StartupTimeout = 30
third_curl_ready.Ready = When.FileExists(configured_test.configured_log)

tr2.Processes.Default.Command = "sleep 10"
tr2.Processes.Default.Return = 0

# Configure process order:
#   1. curl /first. The entry should be logged to current log which will be _old.
#   2. mv the log to _old.
#   3. curl /second. The entry should end up in _old log.
#   4. Send a SIGUSR2 to traffic_manager. The log should be recreated.
#   5. curl /third. The entry should end up in the new, non-old, log file.
#
tr2.Processes.Default.StartBefore(third_curl_ready)
third_curl_ready.StartBefore(third_curl)
third_curl.StartBefore(send_pkill_ready)
send_pkill_ready.StartBefore(send_pkill)
send_pkill.StartBefore(second_curl_ready)
second_curl_ready.StartBefore(second_curl)
second_curl.StartBefore(rotate_log)
rotate_log.StartBefore(first_curl_ready)
first_curl_ready.StartBefore(first_curl)
first_curl.StartBefore(configured_test.ts)
tr2.StillRunningAfter = configured_test.ts

# Verify that the logs are in the correct files.
configured_test.ts.Disk.configured_log.Content += Testers.ExcludesExpression(
        "/first",
        "The new test_rotation.log should not have the first GET retrieval in it.")
configured_test.ts.Disk.configured_log.Content += Testers.ExcludesExpression(
        "/second",
        "The new test_rotation.log should not have the second GET retrieval in it.")
configured_test.ts.Disk.configured_log.Content += Testers.ContainsExpression(
        "/third",
        "The new test_rotation.log should have the third GET retrieval in it.")

configured_test.ts.Disk.configured_log_old.Content += Testers.ContainsExpression(
        "/first",
        "test_rotation.log_old should have the first GET retrieval in it.")
configured_test.ts.Disk.configured_log_old.Content += Testers.ContainsExpression(
        "/second",
        "test_rotation.log_old should have the second GET retrieval in it.")
configured_test.ts.Disk.configured_log_old.Content += Testers.ExcludesExpression(
        "/third",
        "test_rotation.log_old should not have the third GET retrieval in it.")
