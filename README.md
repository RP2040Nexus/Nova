# Nova


<img src="https://github.com/user-attachments/assets/89f43916-1a61-4253-98a4-8ea27cf4493b" alt="2" width="500"/>


Nova is a super tiny [RP2040](https://www.raspberrypi.com/products/rp2040/) board featuring an integrated 7x10 addressable LED matrix. This project aims to provide an easy-to-use platform for creating interactive and visually engaging projects.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
- [Arduino IDE Installation](#arduino-ide-installation)
- [Pinouts](#pinouts)

## Features

- Compact size: Easy to integrate into various projects
- [RP2040](https://www.raspberrypi.com/products/rp2040/) microcontroller: Powerful and efficient
- Integrated 7x10 addressable LED matrix ([WS2812-1010](https://www.mouser.com/datasheet/2/744/WS2812B_1010_DATASHEET-3314306.pdf?srsltid=AfmBOoo3wt47VzInp2GmgZcu0IhygLrT_vyxBrGF-6aGd_k5DdkaCIo3))

## Getting Started

To get started with Nova, you'll need the following:

- [Nova Board](link_to_Nova_Product_Page)
- [Arduino IDE](https://www.arduino.cc/en/software)
- Compatible power source (USB Type-C, battery, etc.)

## Pinouts
<img src="https://github.com/user-attachments/assets/c1b9e0ca-4872-408f-9046-8e6d8c238d8b" alt="2" width="800"/>



## Arduino IDE Installation

### Adding Nova Board to Arduino IDE

1. **Open Arduino IDE**: Launch the Arduino IDE on your computer.

2. **Open Preferences**: 
   - Go to `File` -> `Preferences` (or `Arduino IDE` -> `Preferences` on macOS).

3. **Add Board URL**:
   - In the Preferences window, find the `Additional Boards Manager URLs` field. 
   - Add the following URL to the field:
     ```
     https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
     ```
   - If there are already other URLs, separate them with commas.

4. **Open Boards Manager**:
   - Go to `Tools` -> `Board` -> `Boards Manager`.

5. **Install RP2040 Core**:
   - In the Boards Manager window, type `RP2040` in the search box.
   - Look for `Raspberry Pi Pico/RP2040 by Earle Philhower` and click the `Install` button.

6. **Select the Nova Board**:
   - Once the installation is complete, go to `Tools` -> `Board` and select `Raspberry Pi Pico`.

7. ### Install Required Libraries

- It is recommended to use the **FastLED** library for controlling the LED matrix.

#### Installing FastLED Library

1. **Open Library Manager**:
   - Go to `Sketch` -> `Include Library` -> `Manage Libraries...`.

2. **Search for FastLED**:
   - In the Library Manager window, type `FastLED` in the search bar.

3. **Install FastLED**:
   - Find the **FastLED** library ([Repo](https://github.com/FastLED/FastLED)) and click the `Install` button.
  
#### Using FastLED with Nova

Here’s a basic example of how to use the FastLED library to control a single LED on your Nova board:

```cpp
#include <FastLED.h>

#define LED_PIN     22    // Data pin connected to LED0 (GP22)
#define NUM_LEDS    1      // Total number of LEDs (only LED0)

CRGB leds[NUM_LEDS];

void setup() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(10); // Set brightness to 10 out of 255
}

void loop() {
    // Example: Set LED0 to red color
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(1000);
    
    // Example: Turn off LED0
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(1000);
}

