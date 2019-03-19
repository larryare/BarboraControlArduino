#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> //idk platform.io requires this
#include <WiFiUdp.h>

//dht
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
double previousTemperature = 0.0;
double previousHumidity = 0.0;

//define firebase
#define FIREBASE_HOST "barbora-f1111.firebaseio.com"
#define FIREBASE_AUTH "7t7kFonYY4bVbZlFfaW9LNQibADYPEmRg3FIDbbj"
#define DB_PATH "/control/"

#define BASKING_TIME "/control/baskingTime"
#define IR_STATUS "/control/irStatus"
#define UV_STATUS "/control/uvStatus"
#define START_MINUTE "/control/startMinute"
#define START_HOUR "/control/startHour"
#define OVERRIDE "/control/overRide"
#define TEMPERATURE "/temperature"
#define HUMIDITY "/humidity"

boolean overRide = false;
boolean uvStatus = false;
boolean irStatus = false;
int baskingTime;
int startHour;
int startMinute;
int endTime;

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
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

  //initialize firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.setBool(OVERRIDE, false);
  baskingTime = Firebase.getInt(BASKING_TIME);
  startHour = Firebase.getInt(START_HOUR);
  startMinute = Firebase.getInt(START_MINUTE);

  Firebase.stream(DB_PATH);

  //dht
  dht.begin();
}

void loop()
{
  //declare timeClient ints
  timeClient.update();
  int clientHour = timeClient.getHours();
  int clientMinute = timeClient.getMinutes();
  Serial.print(F("Current system Time is: "));
  Serial.println(timeClient.getFormattedTime());

  //Firebase stuff
  if (Firebase.failed())
  {
    Serial.println("streaming error");
    Serial.println(Firebase.error());
    delay(1000);
    Firebase.stream(DB_PATH);
    return;
  }

  if (Firebase.available())
  {
    FirebaseObject event = Firebase.readEvent();
    String eventPath = event.getString("path");
    eventPath.toLowerCase();
    Serial.println("DB CHANGE DETECTED!");
    Serial.print("PATH: ");
    Serial.println(eventPath);
    if (eventPath == "/override")
    {
      overRide = Firebase.getBool(OVERRIDE);
      Serial.println("OVERRIDE!");
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
    }

    endTime = startHour + baskingTime;

    if (endTime >= 24)
    {
      endTime = endTime - 24;
    }

    //tominutes
    int startTimeMinutes = startHour * 60 + startMinute;
    int endTimeMinutes = startHour * 60 + startMinute + baskingTime * 60;
    int currentTimeMinutes = clientHour * 60 + clientMinute;

    if (!overRide)
    {
      if (startTimeMinutes <= currentTimeMinutes && currentTimeMinutes <= endTimeMinutes)
      {
        Serial.println("System time is between given values");
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
        Serial.println("System time is NOT between given values");
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
      Serial.println(F("OverRiden"));
    }
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
    Serial.println(F("Failed to read from DHT sensor!"));
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

  //refresh every 1 second
  delay(1000);
}