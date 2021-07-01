import serial
import time

arduino = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=.1)
def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)

if __name__ == '__main__':
    while True:
        command = input("Enter command: ") # Taking input from user
        command = command + '\n'
        write_read(command)
