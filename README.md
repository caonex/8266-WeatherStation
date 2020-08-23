# Overview
This project is the first one that I tackled using platform io for Visual Code. I wanted to try out advance editing capabilities of visual code in conjuction with the plaform io extension, especially when compared to the very simple Arduino IDE. I have to say that platform io did not disappoint.

Also, this is my first attempt at using an IOT cloud service, in this case ThingSpeak, which allows quite a few devices and variables for free. A definite plus when compared to the Arduino Cloud.

My motivation was to create a code to be used in multiple devices to compare the fluctuations through out the day between sections of my place, namely the attic and some common areas. I wanted to see to what degree the attic temperature affected the temperature of the house and operation of my AC unit.

![image-20200820231610744](images/image-20200820231610744.png)

I included a celsius offset for displaying the current offset to better calibrate against another thermometer for consistent comparisons. The offset is stored in the flash to persist through power cycles. The offset is applied to the celsius reading before converting to Fahrenheit.

# Requirements

To build this, you need visual code with platform io, an 8266 microcontroller, a 128x64 oled I2C display, a ThingSpeak account, and either a DHT11 or a DHT22 (the more accurate of the two).

I used the display to be able to read current readings as I moved the device from place to place. I did not want to have to rely on my phone or a computer all of the time.

I separated as much as possible into different project libraries and so you may be able to comment out some of the libraries in case you want to try this out without the display for instance and just with an 8266 and a temperature sensor.

If you look at the secrets.hpp file, you will find predefined constants which you have to replace with the right values in order to compile and make it work. I provided a primary and secondary wifi ssid and pass because I was rotating this device between two different places.

# Steps to use this library

1. Update secrets.hpp with the right information
2. Have the minimum devices required: Sensor and 8266
3. Configure the DHTTYPE in main.h for the right sensor type. This will initialize the DHT library properly.
4. Configure the DHTPIN if D4 is not use. **NOTE**: This is not digital pin 4, like I thought, and found the hard way. This is a 8266 GPIO defined constant.
5. Define the proper screen **width** and **height** for the display you will be using in the DisplayHandler. The default currently is for an OLED 128x64.

# Libraries Used

## Arduino <arduino.h>

This is necessary to use a cpp file or platform io without using **ino** files.

## ArduinoJson <ArduinoJson.h>

This was used to create a document or object that could always be quickly serialized to serve over http.

## ESP8266WebServer.h <ESP8266WebServer.h>

Used to create a web server to serve a client-side app, receive offset via GET, and serve the data via application/json.

## ESP8266mDNS <ESP8266mDNS.h>

This is used for distributed multicast DNS. Many iot devices made by popular companies for discovering services and devices. It avoids having to configure a DNS server and do any form of address reservations. I was not aware of this protocol and was glad to
have learned about it during this exercise.

## EEPROM <EEPROM.h>

This is used to simulate EEPROM in flash memory. I looked into saving directly to the flash, but this library allows directly storing types into memory addresses and just as easily allows to read them back into variables. I did not realize that the operations were not performed immediately and `EEPROM.commit()` is required to do so. I learned this the hard way.

## Adafruit Sensor <Adafruit_Sensor.h>

Used to interact with the DHT temperature sensors. This is a requirement of the following library.

## DHT <DHT.h>

Also, used to interact with the DHT sensors. This is the primary library used to intantiate the class to read from the sensor.

# Webserver

This code makes use of the 8266WebServer library. I used it to serve a simple Google Material Design site which show the current environment readings. The application is completely in html and javascript and is only downloaded once. Any subsequent data updates after every 30 seconds are made possible via the /data endpoint which serves a json file with all of the details for the client side to refresh.

## Endpoints

1. /data
2. /offset?value=n, n could be negative and it could be a float or decimal number.
3. /

# ThingSpeak Data points
1. Temperature
2. Humidity
3. Heat Index
4. IP Address // Used to be able to connect to the portal
5. SSID

# Sample output

![image-20200820231303601](images/image-20200820231303601.png)

# Known Issues

The mDNS library for 8266 has a bug which is preventing this implementation from letting other devices find it based on a name. There is an open bug on the 8266 Arduino github page already.

# Potential Improvements

1. Make use of the taskscheduler library to dispatch concurrent tasks for each of the services.
2. Find a mDns library that works or wait for the bugs to be squashed if I do not make some time to contribute. The intent was to have the devices be reached via weatherstation.local, no matter the network.
3. Make it more modular so that the program is not unnecessarily outputing to **Serial** and **I2C**. A user without a display could simply disable and only compile the desired parts.