# Qttp Server Tests

1. Launch test.pro in QtCreator
2. Build & Run

### Sample output

```
********* Start testing of TestHttpServer *********
Config: Using QtTest library 5.4.0, Qt 5.4.0 (x86_64-little_endian-lp64 shared (dynamic) release build; by Clang 6.0 (clang-600.0.54) (Apple))
PASS   : TestHttpServer::initTestCase()
QDEBUG : TestHttpServer::testGET_DefaultResponse() Server running at 0.0.0.0 8080
PASS   : TestHttpServer::testGET_DefaultResponse()
PASS   : TestHttpServer::testGET_RandomLocalhostUrl()
PASS   : TestHttpServer::testPOST_RandomLocalhostUrl()
PASS   : TestHttpServer::cleanupTestCase()
Totals: 5 passed, 0 failed, 0 skipped, 0 blacklisted
********* Finished testing of TestHttpServer *********
```