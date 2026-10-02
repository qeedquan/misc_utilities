from sense_hat import SenseHat
from time import sleep
from math import *

sense = SenseHat()

def compass_magnitude(c):
    x, y, z = c["x"], c["y"], c["z"]
    return sqrt(x * x + y * y + z * z)

while True:
    # uT - micro teslas
    # rH - relative humidity : ratio of partial pressure of water / equilibrium vapor pressure of water
    print("pressure %f (hPA) temperature %f (C)" % (sense.get_pressure(), sense.get_temperature_from_pressure()))
    print("humidity %f (rH) temperature %f (C)" % (sense.get_humidity(), sense.get_temperature_from_humidity()))

    # part of imu, has 3 sensors(gyro, accel, magnetometer)
    # each of the sensors has 3 axis, so 9 dof
    gyro = sense.get_gyroscope()
    gyro_raw = sense.get_gyroscope_raw()
    accel = sense.get_accelerometer()
    accel_raw = sense.get_accelerometer_raw()
    orientation = sense.get_orientation()
    compass = sense.get_compass()
    compass_raw = sense.get_compass_raw()
    print("orientation from gyro (deg)", gyro)
    print("orientation from accel (deg)", accel)
    print("orientation fusion (deg)", orientation)
    print("accel_raw (Gs)", accel_raw)
    print("gyro raw (rad/s)", gyro_raw)
    print("compass direction of north from magnetometer (deg)", compass)
    print("compass raw (uT) ", compass_raw)
    print("compass raw magnitude", compass_magnitude(compass_raw))
    print("\n")
    sleep(1)

