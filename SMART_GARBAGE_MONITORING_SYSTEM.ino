#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5);
#include "ThingSpeak.h"

const int trigPin = D5;
const int echoPin = D6;

const int buz = D8;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

const char *ssid = "";
const char *password = "";

const int total_height = 27;
int bin_lvl = 0;
int x;
int Field_number = 1;

//----- Seconds -----//
int seconds=10; // Data update in seconds.

//----------- Channel Details -------------//
unsigned long Channel_ID = ""; // Channel ID
const char * WriteAPIKey = ""; // Your write API Key

float Latitude, Longitude;
int year, month, date, hour, minute, second;
String DateString, TimeString, LatitudeString, LongitudeString;

WiFiServer server(80);
WiFiClient  client;

void setup()
{
    Serial.begin(9600); // Starts the serial communication
//    Serial1.begin(115200);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(buz, OUTPUT);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("IoT Garbage lvl");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring Sys.");
    delay(2000);
    Serial.begin(9600);
    SerialGPS.begin(9600);
    Serial.println();
    Serial.print("Connecting");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    server.begin();
    Serial.println("Server started");
    Serial.println(WiFi.localIP());
    WiFi.mode(WIFI_STA);
    ThingSpeak.begin(client);
}

void loop()
{
//    while (SerialGPS.available() > 0)
//        if (gps.encode(SerialGPS.read()))
//        {
//            if (gps.location.isValid())
//            {
//                Latitude = ; //gps.location.lat();
//                LatitudeString = String(Latitude, 6);
//                Longitude = ; //gps.location.lng();
//                LongitudeString = String(Longitude, 6);
//            }
//
//            if (gps.date.isValid())
//            {
//                DateString = "";
//                date = gps.date.day();
//                month = gps.date.month();
//                year = gps.date.year();
//
//                if (date < 10)
//                    DateString = '0';
//                DateString += String(date);
//
//                DateString += " / ";
//
//                if (month < 10)
//                    DateString += '0';
//                DateString += String(month);
//                DateString += " / ";
//
//                if (year < 10)
//                    DateString += '0';
//                DateString += String(year);
//            }
//
//            if (gps.time.isValid())
//            {
//                TimeString = "";
//                hour = gps.time.hour() + 5; // adjust UTC
//                minute = gps.time.minute()+30;
//                second = gps.time.second();
//
//                if (hour < 10)
//                    TimeString = '0';
//                TimeString += String(hour);
//                TimeString += " : ";
//
//                if (minute < 10)
//                    TimeString += '0';
//                TimeString += String(minute);
//                TimeString += " : ";
//
//                if (second < 10)
//                    TimeString += '0';
//                TimeString += String(second);
//            }
//        }

    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_VELOCITY/2;
    
    // Convert to inches
    distanceInch = distanceCm * CM_TO_INCH;

    bin_lvl = map(distanceCm, 0, total_height, 100, 0);
    if (bin_lvl > 100) bin_lvl = 100;
    if (bin_lvl < 0) bin_lvl = 0;
  
    // Prints the distance on the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Garbage Level:");
    lcd.setCursor(5, 1);
    lcd.print(bin_lvl);
    lcd.print('%');
    Serial.print("Garbage Level:");
    Serial.print(bin_lvl);
    Serial.println("%");
//    Serial1.println(bin_lvl);
    if (bin_lvl>=80)
    {
      digitalWrite(buz, HIGH);
      delay(2000);
      digitalWrite(buz, LOW);
    }
    
  WiFiClient client = server.available();
//    if (!client)
//    {
//        return;
//    }
    Latitude = ; //gps.location.lat();
    LatitudeString = String(Latitude, 6);
    Longitude = ; //gps.location.lng();
    LongitudeString = String(Longitude, 6);
    upload();
    for (int i = 0; i < seconds; i++)
    {
//    Serial.println("-------------------------");
//    Serial.println("System Standby....");
//      Serial.print(i);
//      Serial.println(" Seconds elapsed.");
      delay(1000);
    }
  
    
    // Response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>NEO-6M GPS Readings</title> <style>";
    s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
    s += "font-size:300%;";
    s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
    s += "<p ALIGN=CENTER style="
         "font-size:150%;"
         "";
    s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
    s += "width:50%";
    s += "> <tr> <th>Latitude</th>";
    s += "<td ALIGN=CENTER >";
    s += LatitudeString;
    s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
    s += LongitudeString;
//    s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
//    s += DateString;
//    s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
//    s += TimeString;
//    s += "</td>  </tr>";
    s += "</td></tr> <tr> <th>Garbage Level</th> <td ALIGN=CENTER >";
    s += bin_lvl;
    s += "%";
    s += "</td>  </tr> </table> ";

    s += "<p align=center><a style="
             "color:RED;font-size:125%;"
             " href="
             "http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
        s += "12.844154298986457";
        s += "+";
        s += "80.15297202538127";
        s += ""
             " target="
             "_top"
             ">Click here</a> to open the location in Google Maps.</p>";

    s += "</body> </html> \n";

    client.print(s);
    delay(2000);
}

void upload()
{
//  internet();
  x = ThingSpeak.writeField(Channel_ID, Field_number, bin_lvl, WriteAPIKey);
  if (x == 200)Serial.println("Data Updated.");
  if (x != 200)
  {
    Serial.println("Data upload failed, retrying....");
    delay(1500);
    upload();
  }
}
