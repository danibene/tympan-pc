#!/usr/bin/python3

import os
import sys
import serial
import time

from piclap import *

sys.path.append(os.path.abspath('.'))

class Config(Settings):
    '''Describes custom configurations and action methods to be executed based
    on the number of claps detected.
    '''

    def __init__(self):
        Settings.__init__(self)
        self.method.value = 10000
        self.arduino = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=.1)

    def serialWrite(self, x):
        self.arduino.write(bytes(x, 'utf-8'))
        time.sleep(0.05)

    def on1Claps(self):
        print("To be sent to Tympan")
        self.serialWrite("One clap detected"+"\n")

    def on2Claps(self):
        '''Custom action for 2 claps'''
        print("Light flashed on pin 4")

    def on3Claps(self):
        '''Custom action for 3 claps'''
        print("Light toggled on pin 6")


def main():
    config = Config()
    listener = Listener(config=config, calibrate=False)
    listener.start()


if __name__ == '__main__':
    main()
