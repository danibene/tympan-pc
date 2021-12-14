#!/usr/bin/python3

import os
import sys
import serial
import time
import serial.tools.list_ports

from piclap import *

sys.path.append(os.path.abspath('.'))

class Config(Settings):
    '''Describes custom configurations and action methods to be executed based
    on the number of claps detected.
    '''

    def __init__(self, sampling_rate):
        Settings.__init__(self)
        self.method.value = 16000
        self.rate = sampling_rate
        self.chunk_size = 512
        print('Search...')
        ports = serial.tools.list_ports.comports(include_links=False)
        for port in ports :
            print('Find port '+ port.device)
        if(ports == []):
            print("No serial devices found")
            exit(0)
        ser = serial.Serial("COM7")
        if ser.isOpen():
            ser.close()

        print('Connect ' + ser.name)
        self.arduino = serial.Serial("COM7", baudrate=115200, timeout=.1)

    def serialWrite(self, x):
        self.arduino.write(bytes(x, 'utf-8'))
        time.sleep(0.05)

    def on1Claps(self):
        '''Custom action for 1 clap'''
        print("Nothing sent to Tympan")
        # print("To be sent to Tympan")
        # self.serialWrite("m"+"\n")

    def on2Claps(self):
        '''Custom action for 2 claps'''
        print("To be sent to Tympan")
        self.serialWrite("m"+"\n")
        # print("Light flashed on pin 4")

    def on3Claps(self):
        '''Custom action for 3 claps'''
        print("Light toggled on pin 6")


def main():
    config = Config(44117)
    listener = Listener(config=config, calibrate=False)
    listener.start()


if __name__ == '__main__':
    main()
