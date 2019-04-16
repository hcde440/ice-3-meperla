/*ICE #3.1
 * 
   DHT22 temperature and humidity sensor demo.
    
   brc 2018
*/

// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//include for MPL115A2
#include <Adafruit_MPL115A2.h>

Adafruit_MPL115A2 mpl115a2;

//include for ssd1306 128x32 i2c
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *pressure = io.feed("pressure");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  //setup for the 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initailize with the i2c addre 0x3C
  display.clearDisplay();                    //Clears any existing images
  display.setTextSize(1);                    //Set text size
  display.setTextColor(WHITE);               //Set text color to white
  display.setCursor(0,0);                    //Puts cursor back on top left corner
  display.println("Starting up...");         //Test and write up
  display.display();                         //Displaying the display
  
    
  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();

  // initializing MPL115A2
  mpl115a2.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;
  float pressureKPA = 0;

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(fahrenheit);

  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");
  Serial.println();

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);
  
  pressureKPA = mpl115a2.getPressure();  
  Serial.print("Pressure (kPa): "); 
  Serial.print(pressureKPA, 4); 
  Serial.println(" kPa");
  Serial.println();
  // save pressure to Adafruit IO
  pressure->save(pressureKPA);

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(5000);

  //clear display
  display.clearDisplay();

  //display temperature
  display.setCursor(0,0); 
  display.print("Temperature: ");
  display.println(fahrenheit);
  //display humidity
  display.print("Humidity: ");
  display.print(event.relative_humidity);
  display.println("%");
  //display pressure
  display.print("Pressure: ");
  display.print(pressureKPA);
  display.print("kPa");
  display.display();
  delay(2000);

  display.clearDisplay();
}
