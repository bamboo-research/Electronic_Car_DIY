# -*- coding: utf-8 -*-
# Simple demo of of the PCA9685 PWM servo/LED controller library.
# This will move channel 0 from min to max position repeatedly.
# Author: Tony DiCola
# License: Public Domain
from __future__ import division
import RPi.GPIO as GPIO
import time, threading, Adafruit_PCA9685
'''
left_forth 3
right_forth 4
left_behind 5
right_behind 6
cradle
horizantal 0
vertical 1
'''
class servo:
    def __init__(self):
        self.pwm_control = Adafruit_PCA9685.PCA9685()
        self.pwm_control.set_pwm_freq(60)
        self.numset = set()

        # 0 to 4096
    def set_pwm(self, pwmnum,pwm = 0):
        self.pwm_control.set_pwm(pwmnum, 0, pwm)
        self.numset.add(pwmnum)

    def cleanup(self):
        for num in self.numset:
            self.set_pwm(num,0)

class motor:
    """motors to drive the wheels"""
    def __init__(self, servo):
        self.motor_num = (2, 3, 4, 5)
        self.control_gpio = ((16, 15),(19, 11),(31, 29), (18, 38))
        self.servo = servo
        self.directions = (1, 1, 1, 1)
        self.speeds = (0, 0, 0, 0)
        '''
        self.left_forth = (11, 13)
        self.right_forth = (15, 16)
        self.left_behind = (18, 22)
        self.right_behind = (27, 31)
        '''
        GPIO.setmode(GPIO.BOARD)
        GPIO.setwarnings(False)
        for tuple in self.control_gpio:
            print(tuple)
            GPIO.setup(tuple[0], GPIO.OUT)
            GPIO.setup(tuple[1], GPIO.OUT)


    def change_direction(self,lf = 0, rf =0, lb =0 , rb =0):
        self.__change_direction(lf, rf, lb, rb)
    def __change_direction(self, lf = 1,rf = 1, lb = 1, rb = 1):
        args = (lf, rf, lb, rb)
        for (arg, direction, gpionum) in zip(args, self.directions, self.control_gpio):
            if arg == direction:
                continue
            print(gpionum,arg)
            if arg == 1:
                GPIO.output(gpionum[0], 1)
                GPIO.output(gpionum[1], 0)
            elif arg == -1:
                GPIO.output(gpionum[0], 0)
                GPIO.output(gpionum[1], 1)
            else:
                GPIO.output(gpionum[0], 0)
                GPIO.output(gpionum[1], 0)
        self.directions = args

    def change_speed(self, lf = 0, rf = 0, lb = 0, rb = 0):
        self.__change_speed(lf, rf, lb, rb)

    def __change_speed(self, lf = 0.0, rf = 0.0, lb = 0.0, rb = 0.0):
        args = (lf, rf, lb, rb)
        for (arg, speed, pwmnum) in zip(args, self.speeds, self.motor_num):
            if arg == speed:
                continue
            else:
                self.servo.set_pwm(pwmnum, int(arg*4096))
                print(pwmnum, arg*4096)
        self.speeds = args
        print(self.speeds)

    def forward(self, speed):
        self.__change_direction(1,1,1,1)
        self.__change_speed(speed,speed,speed,speed)

    def backward(self,speed):
        self.__change_direction(-1,-1,-1,-1)
        self.__change_speed(speed,speed,speed,speed)

    def turnleft(self,speed):
        self.__change_direction(0,1,1,1)
        self.__change_speed(speed,speed,0.5*speed,speed)

    def turnright(self,speed):
        self.__change_direction(1,0,1,1)
        self.__change_speed(speed,speed,speed,0.5*speed)

    def backleft(self,speed):
        self.__change_direction(0,-1,-1,-1)
        self.__change_speed(speed,speed,0.5*speed,speed)

    def backright(self,speed):
        self.__change_direction(-1,0,-1,-1)
        self.__change_speed(speed,speed,speed,0.5*speed)

    def turnl_around(self,speed):
        self.__change_direction(lf=-1,rf=1,lb=-1,rb=1)
        self.__change_speed(speed,speed,speed,speed)

    def turnr_around(self,speed):
        self.__change_direction(lf=1,rf=-1,lb=1,rb=-1)
        self.__change_speed(speed,speed,speed,speed)

    def stop(self):
        self.__change_speed(0,0,0,0)
        self.__change_direction(0,0,0,0)

    def cleanup(self):
        self.__change_speed(0,0,0,0)
        GPIO.cleanup()

class cradle:
    def __init__(self, servo):
        self.cradle_mun = [0,1]
        self.servo = servo




