Test.Summary = '''
Test xdebug plugin X-Remap header
'''

server = Test.MakeOriginServer("server")

request_header = {
    "headers": "GET /argh HTTP/1.1\r\nHost: doesnotmatter\r\n\r\n", "timestamp": "1469733493.993", "body": "" }
response_header = {"headers": "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n", "timestamp": "1469733493.993", "body": "" }
server.addResponse("sessionlog.json", request_header, response_header)

ts = Test.MakeATSProcess("ts")

ts.Disk.records_config.update({
    'proxy.config.url_remap.remap_required': 0,
    'proxy.config.diags.debug.enabled': 0,
    'proxy.config.diags.debug.tags': 'http'
})

ts.Disk.plugin_config.AddLine('xdebug.so')

ts.Disk.remap_config.AddLine(
    "map http://one http://127.0.0.1:{0}".format(server.Variables.Port)
)
ts.Disk.remap_config.AddLine(
    "map http://two http://127.0.0.1:{0}".format(server.Variables.Port)
)
ts.Disk.remap_config.AddLine(
    "regex_map http://three[0-9]+ http://127.0.0.1:{0}".format(server.Variables.Port)
)

tr = Test.AddTestRun()
tr.Processes.Default.StartBefore(Test.Processes.ts)
tr.Processes.Default.StartBefore(Test.Processes.server)
tr.Processes.Default.Command = "cp {}/tcp_client.py {}/tcp_client.py".format(
    Test.Variables.AtsTestToolsDir, Test.RunDirectory)
tr.Processes.Default.ReturnCode = 0

tr = Test.AddTestRun()
tr.Processes.Default.Command = (
    "( echo 'GET /argh HTTP/1.1'" +
    " ; echo 'Host: http://127.0.0.1:{}'".format(server.Variables.Port) +
    " ; echo 'X-Debug: X-Remap' ) > {}/none.msg".format(Test.RunDirectory)
)
tr.Processes.Default.ReturnCode = 0

def sendMsg(msgFile):

    tr = Test.AddTestRun()
    tr.Processes.Default.Command = (
        "( python {}/tcp_client.py 127.0.0.1 {} {}/{}.msg".format(
            Test.RunDirectory, ts.Variables.port, Test.TestDirectory, msgFile) +
        " ; echo '======' ) >>  {}/out.log 2>&1 ".format(Test.RunDirectory)
    )
    tr.Processes.Default.ReturnCode = 0

sendMsg('none')
sendMsg('one')
sendMsg('two')
sendMsg('three')

tr = Test.AddTestRun()
tr.Processes.Default.Command = "echo test gold"
tr.Processes.Default.ReturnCode = 0
f = tr.Disk.File("out.log")
f.Content = "out.gold"
