# Twitter Diary
COMS 3930 Final Project

For this project, I decided to test out hardware and software that we haven't covered through labs which includes a twitter API, OLED, 4 digit 7-segment display and a 4x4 keypad. The end result requires the user to authenticate the ESP32 program by running the twitter_connection.py program which would then allow one to write messages which would then be posted at https://twitter.com/ThatEsp8266. In terms of writing the actual messages, I choose to use the 4x4 keypad as it reminded me of a flip phone number pad. My goal was to make a portable wireless device that can send quick tweets without the use of a phone. I connected this to the art of creative writing as one can use this device in order to share updates when they have a thought.

## Materials
- Raspberry Pi
- Monitor
- Keyboard & Mouse
- ESP32
- 7-segment display
- 74HC595
- Resistor 220Ω x8
- LCD1602
- 4x4 Matrix Keypad
- 4 F-to-M wires
- A crazy amount of jumper wires

## Hardware Configuration
### 7-segment display
- 1: Resistor + 3 (74HC595)
- 2: Resistor + 4 (74HC595)
- 3: Resistor + 15 (74HC595)
- 4: Resistor + 5 (74HC595)
- 5: Resistor + 1 (74HC595)
- 6: GPIO 32
- 7: Resistor + 6 (74HC595)
- 8: GPIO 19
- 9: GPIO 18
- 10: Resistor + 2 (74HC595)
- 11: Resistor + 7 (74HC595)
- 12: GPIO 5

### 74HC595
- 1: 5 (7 segment display)
- 2: 10 (7 segment display)
- 3: 1 (7 segment display)
- 4: 2 (7 segment display)
- 5: 4 (7 segment display)
- 6: 7 (7 segment display)
- 7: 11 (7 segment display)
- 8: GND
- 9: N/A
- 10: 3.3V
- 11: GPIO 4
- 12: GPIO 2
- 13: GND
- 14: GPIO 15
- 15: 3 (7 segment display)
- 16: 3.3V

### LCD1602
- 1: GND
- 2: 5V (VCC)
- 3: GPIO 13 (SDA)
- 4: GPIO 14 (SCL)

### 4x4 Matrix Keypad
- Row 1: GPIO 12
- Row 2: GPIO 27
- Row 3: GPIO 26
- Row 4: GPIO 25
- Col 1: GPIO 33
- Col 2: GPIO 21
- Col 3: GPIO 22
- Col 4: GPIO 23

## Executing Program
Either on a Raspberry Pi or Personal Computer
1. Go to [Arduino Site](https://www.arduino.cc/en/software)
2. Download Linux ARM 32-bit (or one based on correct Operating System)
3. Extract file and install the Arduino IDE in dowloaded directory (eg. Downloads)
- Example for ARM 32-bit
```bash
cd Downloads
tar -xvf arduino-1.8.13-linuxarm.tar.xz
cd arduino-1.8.13
sudo ./install.sh
```
4. Clone twitter-diary
```bash
git clone https://github.com/jxm033f/twitter-diary.git
cd twitter-diary
```
5. Open Arduino IDE -> File -> Open -> hardware-fp.ino <br />
        1. File is located in directory twitter-diary/final-project/hardware-fp
	2. Install the LiquidCrystal_I2C library by Sketch -> Include Library <br />
6. Upload Code which will allow it to run through the ESP32 <br />
        1. Connect to Columbia University wifi (if not change ssid & password in file to one that applies)
        2. After succesfully uploading you can disconnect the ESP32
		1. Plug in LiPo battery or other power supply
		2. Click the RST button on the ESP32 to start program
7. Then run twitter_connection.py on Terminal in final-project directory (change twitter API if needed)
```bash
python3 twitter_connection.py
```

## Images
Enclosure in progress.

## Links
Once blog post is ready will connect.
