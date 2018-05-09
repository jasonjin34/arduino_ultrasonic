import serial
import time
import pyautogui

ArduinoSerial = serial.Serial('com3',115200) #create serial object call arduinoserial
time.sleep(1); #wait for 2 seconds for the communication to get established

while 1:
    incoming = ArduinoSerial.readline()
    print(incoming)

    #y_mouse = float(incoming);
    #current_position = pyautogui.position();
    #current_position[1] += y_mouse;

    #pyautogui.moveTo(current_position[0],current_position[1],1);

    
