# Change Log
All notable changes to this project will be documented in this file.

## [Unreleased]
### 

## [1.0.0] - 2016-11-06
### Added
- Reimplemented node.native components to copy into Qt classes such as QByteArray instead of std::stream @supamii
- SwaggerUI integration @supamii
- Tutorials for Action and Routing @supamii

### Changed
- All std::function callbacks are wrapped with qttp::Action

## [0.1.0] - 2016-10-01
### Added
- Basic http routing features and qt integration @supamii
- QEvents generated from libuv/node.native to the main qt thread/application @supamii

[Unreleased]: https://github.com/supamii/QttpServer/compare/QTTPv1.0.0...supamii:master
[1.0.0]: https://github.com/supamii/QttpServer/compare/QTTPv0.1.0...supamii:QTTPv1.0.0
[0.1.0]: https://github.com/supamii/QttpServer/compare/v0.1.0...supamii:QTTPv0.1.0
