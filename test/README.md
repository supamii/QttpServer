# QttpServer Tests

## Quick Start

The QttpServer framework can help you test your API.  Along with using QtTest,
additional hooks and MACROs are also provided to further increase testability.

## Test Results from QttpTest

Take a peek at [qttptest](./qttptest/) to see how QttpServer tests executed.

```
Starting qttptest...
********* Start testing of QttpTest *********
Config: Using QtTest library 5.7.0
PASS   : QttpTest::initTestCase()
PASS   : QttpTest::testGET_EchoResponse()
PASS   : QttpTest::testGET_NoEchoResponse()
PASS   : QttpTest::testPOST_EchoBodyResponse()
PASS   : QttpTest::testPOST_InvalidEchoBodyResponse()
PASS   : QttpTest::testGET_DefaultResponse()
PASS   : QttpTest::testGET_RandomLocalhostUrl()
PASS   : QttpTest::testPOST_RandomLocalhostUrl()
PASS   : QttpTest::testGET_TestResponse()
PASS   : QttpTest::testGET_TestConfigResponse()
PASS   : QttpTest::testGET_SampleResponse()
PASS   : QttpTest::testGET_SampleWithParamResponse()
PASS   : QttpTest::testGET_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPOST_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPUT_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testDELETE_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPOST_InvalidTerminateResponse()
PASS   : QttpTest::testGET_TerminateResponse()
PASS   : QttpTest::testPUT_TerminateResponse()
PASS   : QttpTest::testDELETE()
PASS   : QttpTest::testDEL()
PASS   : QttpTest::cleanupTestCase()
Totals: 22 passed, 0 failed, 0 skipped, 0 blacklisted, 7565ms
********* Finished testing of QttpTest *********
qttptest exited with code 0
```

## Monkey Tests

With QttpServer, we explore registered URL paths and routes to produce [Monkey-Tests](./monkeytest/) 
that can provide a *certain* level of confidence before running unit and integration 
tests.