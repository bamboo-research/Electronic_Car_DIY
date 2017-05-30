# Electronic Car DIY

A small project for Digital Circuit Course

---
## Hardware

 - Remote controller based on MCU `STC12C5A60S2`, triaxial accelerometer `ADXL345` and bluetooth 2.0 module `HC-05`
 - Raspberry Pi 3 Model B
 - Cradle head made with `two` `SG90` servo
 - `Two` Dual Full-Bridge Motor Drivers `L298N`
 - `Four` 12-V stepping motors
 - Adafruit 16-Channel 12-bit PWM/Servo Driver `PCA9685`
 
## Functions Implemented

 - `send.c` in the folder `Remote Controller` is for the MCU `STC12C5A60S2` to connect and read data from `ADXL345` via `I2C` protocal and then transmit them to bluetooth via `UART`
 - `client.py` is used on PC to read the data receive by bluetooth via a `USB-TTL` serial and send the data via internet to the server on the raspberry Pi
 - `server.py` is used to receive the data and make the decision for the motors and the cradle head.
 - `cradle.py` is used to control the servos on the cradle head
 - `Multiface.py` is used to track the face appear in the camera of Pi and control cradle head via `cradle.py`
   - OpenCV
   - rough tracking method


## Important
 - Except for `cradle.py` and `Multiface.py`, all the python scripts should be run in python 3 or higher.

## TO DO
 - improve the size of pictures from the Pi Camera
 - change the cradle head moving algorithm to PID
 - improve the motors algorithm, add feedback processing
