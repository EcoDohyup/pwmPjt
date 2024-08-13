# PWM with ESP-32

## Purpose
1. to establish ESP now connection between two esp-32 boards
2. to control a motor with wireless communication
3. to successfully connect among an esp32, a motor driver, a relay and a motor

## Progress
1. installed Arduino ide and related libraries for ESP-32 firmware
2. installing usb connection between esp-32 and ubuntu 22.04 lts

## Hardships
### establishing USB connection on Ubuntu 22.04 lts
1. tried lsusb to find the bus and the name of the esp32, which seems to be **QinHeng Electronics CH340 serial converter**.
2. Bus 001 Device 006: ID 1a86:7523
3. trying link [esp-idf](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html)
4. turned to use VScode with idf extensions [vscode with idf](https://devdockr.tistory.com/2157?category=1074383)


# from Home

## Progress
1. get the mac address of each esp32 boards