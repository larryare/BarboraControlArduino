# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
## Released
## [0.1.9] - 2019-03-19
### Added
- [CHANGELOG.md]
- [README.md]
### Removed
- changelog from main.cpp
## Unreleased
## 0.1.8 - 2017-03-18
### Fixed
- Uploads temperature and humidity values to DB only if they are different from previous reading
- Parses only changed values, not all of them at once
### Added
- VCS using Git
## 0.1.7 - 2019-03-18
### Added
- Readded temperature and humidity tracking using DHT22
## 0.1.6 - 2019-03-18
### Fixed 
- Only reads from database if data was changed
### Added
- Override flag
## 0.1.5 - 2019-03-17
### Fixed
- Serial output is now readable
### Removed
- DHT sensor
## 0.1.4 - 2019-03-16
### Changed
- Automatically decides if bulbs are in operating range time
- Calculates time based on given start time and preffered operation lenght
## 0.1.2 - 2019-03-15
### Added
- Some Firebase functionality
## 0.0.2 - 2019-03-14
### Added
- Temperature and humidity tracking using DHT22
## 0.0.1 - 2019-03-13
- Initial release

[CHANGELOG.md]: CHANGELOG.md
[README.md]: README.md
[0.1.9]: https://github.com/larryare/BarboraControlArduino/tree/0.1.9
