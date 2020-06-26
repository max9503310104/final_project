import serial

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

while 1:
    c = s.readline()
    print(c)