import serial # Serial imported for Serial Communication
import time # use for delay functions
import pyautogui

ArduinoSerial = serial.Serial('com10',9600) #create serial object call arduinoserial
time.sleep(2); #wait for 2 seconds for the communication to get established

while 1:
    incoming = str (ArduinoSerial.readline())
    print (incoming)

    if 'Volumeup' in incoming:
        pyautogui.hotkey('up')

    if 'Volumedown' in incoming:
        pyautogui.hotkey('down')
  
