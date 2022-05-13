#! /usr/bin/python3

import sys
import struct
import serial
import os
import time
if len(sys.argv) < 4:
    print(sys.argv[0], "<tty device>", "<kernel image>", "<base address>")
    sys.exit(-1)

tty_name = sys.argv[1]
kernel_name = sys.argv[2]
base_addr = int(sys.argv[3], 16)

ser = serial.Serial(
    # Serial Port to read the data from
    port = tty_name,
    #Rate at which the information is shared to the communication channel
    baudrate = 115200,
    timeout = .1
)


def readline():
    while 1:
        c = ser.read(1)
        if c == '\n':
            os.write(1, b'\n')
            return
        elif c != '\n':
            os.write(1, c)




            
with open(kernel_name, "rb",) as kernel:
        kernel_buf = kernel.read()
        kernel_size = len(kernel_buf) 
        
        # start upload kernel to rasberry pi 3b+ by uart interface
        # start header
        ser.write(b"A")
        ser.flush()
        readline()

        print("Start sending kernle")
        # transfer kernel base address and kernel size ( both are 8 bytes ) in little endian
        ser.write(struct.pack("<Q", base_addr))
        ser.flush()
        readline()

        print("kernel base address is:", hex(base_addr))
        ser.write(struct.pack("<Q", kernel_size))
        ser.flush()
        readline()

        print("kernel size is:", hex(kernel_size))
        # start transfer the whole kernel image
        for i in kernel_buf:
            ser.write(i.to_bytes(1, byteorder='little'))
            ser.flush()
        readline()

        print("Finish upload kernel")
        debug()

ser.close()

