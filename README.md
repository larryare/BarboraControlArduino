# Barbora Control Arduino
- [Barbora Control Arduino](#barbora-control-arduino)
  - [[Changelog]](#changelog)
  - [Credentials](#credentials)
    - [Where to put credentials file](#where-to-put-credentials-file)
    - [How to configure credentials file](#how-to-configure-credentials-file)
    - [Temperature and humidity](#temperature-and-humidity)
## [Changelog]

## Credentials
### Where to put credentials file
Create credentials.h header file inside your project directory
### How to configure credentials file
You can copy and paste this snippet and change the values:
``` h
#define FIREBASE_HOST "YourDB.firebaseio.com"
#define FIREBASE_AUTH "Some secret"

#define DB_PATH "/Parent directory"

#define BASKING_TIME "/Parent directory/Child directory"
#define IR_STATUS "/Parent directory/Child directory"
#define UV_STATUS "/Parent directory/Child directory"
#define START_MINUTE "/Parent directory/Child directory"
#define START_HOUR "/Parent directory/Child directory"
#define OVERRIDE "/Parent directory/Child directory"
#define TEMPERATURE "/temperature"
#define HUMIDITY "/humidity"
```
### Temperature and humidity
Use different parent directory for temperature and humidity, because you don't want your get DB values every time temperature or humidity changes. 

[Changelog]: CHANGELOG.md