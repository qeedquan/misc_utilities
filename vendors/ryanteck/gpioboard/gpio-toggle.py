import time
import sys
from RTk import GPIO

def blink(pin, delay):
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(pin, GPIO.OUT)
    while True:
        GPIO.output(pin, 1)
        time.sleep(delay)
        GPIO.output(pin, 0)
        time.sleep(delay)

def main():
    if len(sys.argv) < 2:
        print("usage: pin")
        exit(1)
    
    blink(int(sys.argv[1]), 1)

main()
