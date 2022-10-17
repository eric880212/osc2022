import sys
import os
import time
#import serial
#print('abcd')

with open('/dev/pts/5', "wb",115200) as tty:
    tty.write(b'load\n')
    with open('/home/isipsw/Desktop/HYC/OSDI/lab02/rasbootcom/kernel8.img', "rb") as kernel:
        kernel_size = len(kernel.read()) # 1672
        tty.write((kernel_size).to_bytes(4,"little")) # '1672'
        num = 0
#        tty.write(kernel.read())
#for i in range(kernel_size):
#            tty.write(kernel.read(1))
#            num+=1
        while (num < kernel_size):
            kernel.seek(num, 0)
            tty.write(kernel.read(1))
            time.sleep(0.001)
            num += 1
        print("finish", num , "bytes")
    
