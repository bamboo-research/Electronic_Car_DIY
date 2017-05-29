# -*- coding: utf-8 -*-
# Simple demo of of the PCA9685 PWM servo/LED controller library.
# This will move channel 0 from min to max position repeatedly.
# Author: Tony DiCola
# License: Public Domain
from __future__ import division
import time, threading

# Import the PCA9685 module.
import Adafruit_PCA9685


class servo:
    # Uncomment to enable debug output.
    #import logging
    #logging.basicConfig(level=logging.DEBUG)
    h_angle=0
    v_angle=0
    delay = 0.01
    # Alternatively specify a different address and/or bus:
    #pwm = Adafruit_PCA9685.PCA9685(address=0x41, busnum=2)

    # Configure min and max servo pulse lengths
    servo_min = 103  # Min pulse length out of 4096
    servo_max = 512  # Max pulse length out of 4096
    #vertical_max = 610
    #vertical_min = 180
    up_status = 0 # 0 stop 1 up 2 down
    down_status = 0 # 0 stop 1 left 2 right

    def __init__(self):
        global pwm
        pwm = Adafruit_PCA9685.PCA9685()
        pwm.set_pwm_freq(60)
        self.init_position()
        # Initialise the PCA9685 using the default address (0x40)


    def init_position(self):
        self.h_angle=0
        self.v_angle=40
        pwm.set_pwm(0, 0, 325)
        i=int(round(180 + 430 * self.v_angle / 170))
        pwm.set_pwm(1, 0, i)

        time.sleep(0.5)

    def setpwm(self,p):
        pwm.set_pwm(1, 0, p)

    def turnleft(self,angle):
        if self.down_status == 0 or self.down_status == 2:
            down_thread = threading.Thread(target=self.turnleft_,args =(angle,))
            self.down_status = 1
            down_thread.start()


    def turnright(self,angle):
        if self.down_status == 0 or self.down_status == 1:
            down_thread = threading.Thread(target=self.turnright_,args =(angle,))
            self.down_status = 2
            down_thread.start()


    def turnup(self,angle):
        if self.up_status !=1:
            up_thread = threading.Thread(target=self.turnup_,args =(angle,))
            self.up_status = 1
            up_thread.start()

    def turndown(self,angle):
        if self.up_status != 2:
            up_thread = threading.Thread(target=self.turndown_,args =(angle,))
            self.up_status = 2
            up_thread.start()


    def stop_d(self):
        i= int(round(365 - 225 * self.h_angle / 90))
        pwm.set_pwm(0, 0, i)
        time.sleep(0.2)
        self.down_status = 0


    def stop_u(self):
        i= int(round(130 + 240 * self.v_angle / 90))
        pwm.set_pwm(1, 0, i)
        time.sleep(0.2)
        self.down_status = 0







    def turnleft_(self,angle):
        #print(angle)
        for m in range(angle):
            if self.h_angle > -90:
                #print(self.h_angle)
                i= int(round(325 - 225 * self.h_angle / 90))
                pwm.set_pwm(0, 0, i)
                self.h_angle -= 1    #减小当前角度
                time.sleep(self.delay)
                m +=1
                if self.down_status != 1:
                    break
        self.down_status=0


    def turnright_(self,angle):
        #print(angle)
        for m in range(angle):
            if self.h_angle < 90:
                #print(self.h_angle)
                i= int(round(325 - 225 * self.h_angle / 90))
                pwm.set_pwm(0, 0, i)
                self.h_angle += 1    #减小当前角度
                time.sleep(self.delay)
                if self.down_status != 2:
                    break
        self.down_status=0




    def turnup_(self,angle):
        for m in range(angle):
            if self.v_angle >0:
                #print(self.v_angle,self.up_status)
                i=int(round(180 + 430 * self.v_angle / 170))
                pwm.set_pwm(1, 0, i)
                self.v_angle -= 1 #减小当前角度
                time.sleep(self.delay)
                if self.up_status != 1:
                    break
        self.up_status = 0


    def turndown_(self,angle):
        for m in range(angle):
            if self.v_angle<170:
                #print(self.v_angle,self.up_status )
                i=int(round(180 + 430 * self.v_angle / 170))
                pwm.set_pwm(1, 0, i)
                self.v_angle += 1 #减小当前角度
                time.sleep(self.delay)
                if self.up_status != 2:
                    break
        self.up_status = 0


