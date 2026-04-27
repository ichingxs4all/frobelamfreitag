import bluetooth
import time
from ble_uart import BLEUART
from PWMServos import PWMServos

bleName = "PicoUART_patrick"
ble = bluetooth.BLE()
uart = BLEUART(ble, name=bleName)  # this is the BLE name
print("Advertising as " + bleName)

servos = PWMServos()
_last_connected = False  # simple state flag

def getIntFromString(s):
    return int(s.strip());

def map_range(value, in_min, in_max, out_min, out_max):

    # Validate input range
    if in_min == in_max:
        raise ValueError("Input range cannot be zero (in_min == in_max).")

    # Clamp value to input range to avoid overshoot
    if value < in_min:
        value = in_min
    elif value > in_max:
        value = in_max

    # Perform mapping
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def handle_rx(data: bytes):
    """Called whenever data arrives over BLE."""
    try:
        text = data.decode("utf-8", "ignore")
    except Exception as e:
        print("Decode error:", e)
        return

#   print("RX from PC:", repr(text))

    x = getIntFromString(text[0:2])
    y = getIntFromString(text[2:4])
    z = getIntFromString(text[4:6])
    yaw = getIntFromString( text[6:8])
    pitch =  getIntFromString(text[8:10])
    mouth = getIntFromString( text[10:12])
    left_eye = getIntFromString(text[12:14])
    right_eye = getIntFromString(text[14:16])
    roll = getIntFromString(text[16:17])
    smile = getIntFromString(text[17:18])
    face_visible = getIntFromString(text[18:19])

    mouth = map_range(mouth,0,100,0,180)
    servos.goToPosition(1,mouth)
    servos.goToPosition(2,yaw)

    print("Mouth :", text[6:8])

uart.on_rx(handle_rx)

loop_count = 0
while True:
    loop_count += 1

    # Just a heartbeat so you know it's alive
    if loop_count % 20 == 0:
        print("loop heartbeat, connected =", uart.is_connected())

    connected = uart.is_connected()
    if connected != _last_connected:
        print("Connection state changed. Connected:", connected)
        _last_connected = connected

    time.sleep(0.2)
