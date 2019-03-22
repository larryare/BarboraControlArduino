#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include "credentials.h"

#define DHTPIN D7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
double previousTemperature = 0.0;
double previousHumidity = 0.0;

boolean overRide = false;
boolean uvStatus = false;
boolean irStatus = false;

int baskingTime;
int startHour;
int startMinute;
int endTime;

int startTimeMinutes;
int endTimeMinutes;

const int uvPin = D5;
const int irPin = D6;

const char *ssid = "Waifas 2.4";
const char *password = "lsmustud";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

void setup()
{
  pinMode(uvPin, OUTPUT);
  pinMode(irPin, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  timeClient.begin();

  //initialize firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.setBool(OVERRIDE, false);
  baskingTime = Firebase.getInt(BASKING_TIME);
  startHour = Firebase.getInt(START_HOUR);
  startMinute = Firebase.getInt(START_MINUTE);

  startTimeMinutes = startHour * 60 + startMinute;
  endTimeMinutes = startHour * 60 + startMinute + baskingTime * 60;

  Firebase.stream(DB_PATH);

  dht.begin();
}

void loop()
{
  timeClient.update();
  int clientHour = timeClient.getHours();
  int clientMinute = timeClient.getMinutes();
  int currentTimeMinutes = clientHour * 60 + clientMinute;

  if (Firebase.failed())
  {
    delay(1000);
    Firebase.stream(DB_PATH);
    return;
  }

  if (Firebase.available())
  {
    FirebaseObject event = Firebase.readEvent();
    String eventPath = event.getString("path");
    eventPath.toLowerCase();
    if (eventPath == "/override")
    {
      overRide = Firebase.getBool(OVERRIDE);
    }
    if (eventPath == "/uvstatus")
    {
      uvStatus = Firebase.getBool(UV_STATUS);
    }
    if (eventPath == "/irstatus")
    {
      irStatus = Firebase.getBool(IR_STATUS);
    }
    if (eventPath == "/baskingtime" || eventPath == "/starthour" || eventPath == "/startminute")
    {
      baskingTime = Firebase.getInt(BASKING_TIME);
      startHour = Firebase.getInt(START_HOUR);
      startMinute = Firebase.getInt(START_MINUTE);

      endTime = startHour + baskingTime;

      startTimeMinutes = startHour * 60 + startMinute;
      endTimeMinutes = startHour * 60 + startMinute + baskingTime * 60;

      if (endTime >= 24)
      {
        endTime = endTime - 24;
      }
    }
  }

  if (!overRide)
  {
    if (startTimeMinutes <= currentTimeMinutes && currentTimeMinutes <= endTimeMinutes)
    {
      if (!uvStatus)
      {
        Firebase.setBool(UV_STATUS, true);
        uvStatus = true;
      }
      if (!irStatus)
      {
        Firebase.setBool(IR_STATUS, true);
        irStatus = true;
      }
    }
    else
    {
      if (uvStatus)
      {
        Firebase.setBool(UV_STATUS, false);
        uvStatus = false;
      }
      if (irStatus)
      {
        Firebase.setBool(IR_STATUS, false);
        irStatus = false;
      }
    }
  }
  else
  {
  }

  if (uvStatus)
  {
    digitalWrite(uvPin, LOW);
  }
  else
  {
    digitalWrite(uvPin, HIGH);
  }

  if (irStatus)
  {
    digitalWrite(irPin, LOW);
  }
  else
  {
    digitalWrite(irPin, HIGH);
  }

  double t = dht.readTemperature();
  double h = dht.readHumidity();

  if (isnan(h) || isnan(t))
  {
  }
  else
  {
    if (h != previousHumidity)
    {
      previousTemperature = h;
      Firebase.setFloat(HUMIDITY, h);
    }
    if (t != previousTemperature)
    {
      previousTemperature = t;
      Firebase.setFloat(TEMPERATURE, t);
    }
  }
  delay(1000);
}