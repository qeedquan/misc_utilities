from sense_hat import SenseStick

stick = SenseStick()
while True:
    print(stick.wait_for_event())

