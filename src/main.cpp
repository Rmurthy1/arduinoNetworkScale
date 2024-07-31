//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "HX711.h"

const byte rxPin = 2;
const byte txPin = 3;

// Set up a new SoftwareSerial object
SoftwareSerial mySerial(rxPin, txPin);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

HX711 scale;

const int tareButtonPin = 6; // TARE BUTTON


const int _DoutPin = A3;
const int _SckPin = A2;
long _offset = 0; // tare value
int _scale = 500;


int period = 500;
unsigned long time_now = 0;
unsigned long currentTime = 0;

// function declarations
void prepareScale();
long getValue();
float readWeight();
void updateScaleAndLCD();
void clearSecondRowAndPositionCursor();

float readWeight() {
  long val = (getValue() - _offset);
  return (float) val / _scale;
}

long getValue() {
  uint8_t data[3];
    long ret;
    while (digitalRead(_DoutPin)); // wait until _dout is low
    for (uint8_t j = 0; j < 3; j++)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            digitalWrite(_SckPin, HIGH);
            bitWrite(data[2 - j], 7 - i, digitalRead(_DoutPin));
            digitalWrite(_SckPin, LOW);
        }
    }

    digitalWrite(_SckPin, HIGH);
    digitalWrite(_SckPin, LOW);
    ret = (((long) data[2] << 16) | ((long) data[1] << 8) | (long) data[0])^0x800000;
    return ret;
}

void prepareScale() {
    pinMode(_SckPin, OUTPUT);
    pinMode(_DoutPin, INPUT);

    digitalWrite(_SckPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(_SckPin, LOW);
    _offset = getValue();
}



void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  prepareScale();
  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  pinMode(tareButtonPin, INPUT);
}


String message = "";
bool messageReady = false;

void loop() {

  updateScaleAndLCD();
  /*while (Serial.available()) { 
    message = Serial.readString();
    messageReady = true;
    Serial.println("wrote");
  }
  if (messageReady == true) {
    //do stuff
    messageReady = false;
    message = "<" + message + ";0" + ">";
    mySerial.write(message.c_str());
  }*/
}

void updateScaleAndLCD() {
  long scaleValue;
   if (millis() > time_now + period) {
    time_now = millis();
    scaleValue = -1 * readWeight();
    String writeValue = String(scaleValue);
    writeValue = "<" + writeValue + ">";
    mySerial.println(writeValue.c_str());
    //Serial.println(scaleValue);
    clearSecondRowAndPositionCursor();
    lcd.print(scaleValue);
    lcd.setCursor(8, 1);
    lcd.print("g");
   } 
  
  int tareButtonState = digitalRead(tareButtonPin);
  if (tareButtonState == HIGH) {
    _offset = getValue();
  }
}

void clearSecondRowAndPositionCursor() {
  lcd.setCursor(0,1);
  for (int i = 0; i < 16; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
}