#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define SDA 13
#define SCL 14

byte rowPins[4] = {12, 27, 26, 25};
byte colPins[4] = {33, 21, 22, 23}; // Column 3 is cross-wired with Column 1??

int latchPin = 2;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 4;          // Pin connected to SH_CP of 74HC595（Pin11）
int dataPin = 15;          // Pin connected to DS of 74HC595（Pin14）
int comPin[] = {5,18,19,32};// Common pin (anode) of 4 digit 7-segment display

// Define the encoding of characters 0-F of the common-anode 7-Segment Display
byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8,
              0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e};

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char prevKey = 0;
char prevValue = 0;
String tweet = "";
int charLeft = 32;

int x_coor = 0;
int y_coor = 0;

//int xyzPins[] = {6, 7, 8};

char value11[4] = {'.', '!', '?', '1'};
char value12[4] = {'a', 'b', 'c', '2'};
char value13[4] = {'d', 'e', 'f', '3'};
char value21[4] = {'g', 'h', 'i', '4'};
char value22[4] = {'j', 'k', 'l', '5'};
char value23[4] = {'m', 'n', 'o', '6'};
char value31[5] = {'p', 'q', 'r', 's', '7'};
char value32[4] = {'t', 'u', 'v', '8'};
char value33[5] = {'w', 'x', 'y', 'z', '9'};
char value42[2] = {' ', '0'};

int position11 = 0;
int position12 = 0;
int position13 = 0;
int position21 = 0;
int position22 = 0;
int position23 = 0;
int position31 = 0;
int position32 = 0;
int position33 = 0;
int position42 = 0;

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
LiquidCrystal_I2C lcd(0x27,16,2);

const char *ssid     = "Columbia University"; //Enter the router name
const char *password = ""; //Enter the router password
WiFiUDP Udp;
unsigned int localUdpPort = 4210;  //  port to listen on
char incomingPacket[255];  // buffer for incoming packets

void resetPositions() {
  position11 = 0;
  position12 = 0;
  position13 = 0;
  position21 = 0;
  position22 = 0;
  position23 = 0;
  position31 = 0;
  position32 = 0;
  position33 = 0;
  position42 = 0;
}

char currentKeyChar(char keyPressed) {
  if (prevKey) {
    if (prevKey != keyPressed) {
      resetPositions();
    }
  }
  if (keyPressed == '1') {
    if (position11 == (sizeof(value11) / sizeof(value11[0]))) {
      position11 = 0;
    }
    return value11[position11++];
  } else if (keyPressed == '2') {
    if (position12 == (sizeof(value12) / sizeof(value12[0]))) {
      position12 = 0;
    }
    return value12[position12++];
  } else if (keyPressed == 'A') {
    if (position13 == (sizeof(value13) / sizeof(value13[0]))) {
      position13 = 0;
    }
    return value13[position13++];
  } else if (keyPressed == '4') {
    if (position21 == (sizeof(value21) / sizeof(value21[0]))) {
      position21 = 0;
    }
    return value21[position21++];
  } else if (keyPressed == '5') {
    if (position22 == (sizeof(value22) / sizeof(value22[0]))) {
      position22 = 0;
    }
    return value22[position22++];
  } else if (keyPressed == 'B') {
    if (position23 == (sizeof(value23) / sizeof(value23[0]))) {
      position23 = 0;
    }
    return value23[position23++];
  } else if (keyPressed == '7') {
    if (position31 == (sizeof(value31) / sizeof(value31[0]))) {
      position31 = 0;
    }
    return value31[position31++];
  } else if (keyPressed == '8') {
    if (position32 == (sizeof(value32) / sizeof(value32[0]))) {
      position32 = 0;
    }
    return value32[position32++];
  } else if (keyPressed == 'C') {
    if (position33 == (sizeof(value33) / sizeof(value33[0]))) {
      position33 = 0;
    }
    return value33[position33++];
  } else if (keyPressed == '0') {
    if (position42 == (sizeof(value42) / sizeof(value42[0]))) {
      position42 = 0;
    }
    return value42[position42++];
  } else if (keyPressed == 'D') {
    return '#';
  }
  return keyPressed;
}

void setup() {
  Serial.begin(112500);

  int status = WL_IDLE_STATUS;
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  
  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(comPin[i], OUTPUT);
  }

//  pinMode(xyzPins[2], INPUT_PULLUP);

  lcd.print("Waiting for");
  lcd.setCursor(0,1);
  lcd.print("authentication");
  lcd.setCursor(0,0);

  // we recv one packet from the remote so we can know its IP and port
  bool readPacket = false;
  while (!readPacket) {
    int packetSize = Udp.parsePacket();
    if (packetSize)
     {
      // receive incoming UDP packets
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
      int len = Udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      readPacket = true;
    } 
  }

  lcd.clear();
  lcd.print("Press key for");
  lcd.setCursor(0,1);
  lcd.print("new Tweet");
  lcd.setCursor(0,0);
}

void loop() {
//  int xVal = analogRead(xyzPins[0]);
//  int yVal = analogRead(xyzPins[1]);
//  int zVal = digitalRead(xyzPins[2]);
//
//  if ((xVal == -1 || xVal == 1) && (yVal == -1 || yVal == 1) && zVal == HIGH) {
//    lcd.setCursor(xVal,yVal);
//  }
  
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    lcd.clear();
    lcd.print("Press key for");
    lcd.setCursor(0,1);
    lcd.print("new Tweet");
    lcd.setCursor(0,0);
    charLeft = 32;
  }
  
  char keyPressed = myKeypad.getKey();
  if (keyPressed) {
    lcd.clear();
    char currentValue = currentKeyChar(keyPressed);
    if (prevValue && currentValue == '#') {
      if ((prevValue == '#' || prevValue == '*') && tweet != "") {
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.printf(tweet.c_str());
        Udp.endPacket();
        tweet = "";
        lcd.print("Sending Tweet...");
      } else if (prevValue != '#' && prevValue != '*') {
        if (charLeft > 0) {
          tweet += prevValue;
          charLeft -= 1;
          if (tweet.length() > 16) {
            String part1 = tweet.substring(0, 16);
            String part2 = tweet.substring(16, tweet.length());
            lcd.print(part1);
            lcd.setCursor(0,1);
            lcd.print(part2);
            lcd.setCursor(0,0);
          } else {
            lcd.print(tweet);
          }
        } else {
          lcd.print("Can't Add");
          lcd.setCursor(0,1);
          lcd.print("Exceeded Limit");
          lcd.setCursor(0,0);
        }
      } else if ((prevValue == '#' || prevValue == '*') && tweet == "") {
        lcd.print("Can't Send");
        lcd.setCursor(0,1);
        lcd.print("Empty String");
        lcd.setCursor(0,0);
      }
    } else if (currentValue != '#' && currentValue != '*') {
      String temp = tweet;
      if (currentValue == ' ') {
        temp += "_";
      } else  {
        temp += currentValue;
      }
      if (temp.length() > 16) {
        String part1 = temp.substring(0, 16);
        String part2 = temp.substring(16, temp.length());
        lcd.print(part1);
        lcd.setCursor(0,1);
        lcd.print(part2);
        lcd.setCursor(0,0);
      } else {
        lcd.print(temp);
      }
    } else if (currentValue == '*' && tweet != "") {
      tweet = tweet.substring(0, tweet.length()-1);
      charLeft += 1;
      if (tweet.length() > 16) {
        String part1 = tweet.substring(0, 16);
        String part2 = tweet.substring(16, tweet.length());
        lcd.print(part1);
        lcd.setCursor(0,1);
        lcd.print(part2);
        lcd.setCursor(0,0);
      } else {
        lcd.print(tweet);
      }
    }
    
    prevValue = currentValue;
    prevKey = keyPressed;
  }

  if (charLeft > 10) {
    int tens = charLeft / 10;
    int ones = charLeft % 10;
    electDigitalDisplay(2);
    writeData(num[tens]);
    writeData(0xff);
    electDigitalDisplay(3);
    writeData(num[ones]);
    writeData(0xff);
  } else {
    electDigitalDisplay(3);
    writeData(num[charLeft]);
    writeData(0xff);
  }
}

void electDigitalDisplay(byte com) {
  // Close all single 7-segment display
  for (int i = 0; i < 4; i++) {
    digitalWrite(comPin[i], LOW);
  }
  // Open the selected single 7-segment display
  digitalWrite(comPin[com], HIGH);
}

void writeData(int value) {
  // Make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);  // Make latchPin output high level
// Make latchPin output high level, then 74HC595 will update data to parallel output
  digitalWrite(latchPin, HIGH);
}
