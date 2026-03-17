import time
import board
import touchio
import usb_hid

from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# ----------------------------------
# SETTINGS
# ----------------------------------

CALIBRATION_SAMPLES = 50     # startup calibration time
DEBOUNCE_TIME = 0.03         # seconds
BASELINE_FOLLOW = 0.001      # smaller = more stable
DEFAULT_SENSITIVITY = 250    # higher = harder to trigger

# ----------------------------------
# USB Keyboard
# ----------------------------------

kbd = Keyboard(usb_hid.devices)

# ----------------------------------
# Touch Key Class
# ----------------------------------

class TouchKey:
    def __init__(self, pin, keycode, sensitivity=DEFAULT_SENSITIVITY):
        self.touch = touchio.TouchIn(pin)
        self.keycode = keycode
        self.sensitivity = sensitivity

        self.baseline = 0
        self.pressed = False
        self.last_change = 0

    def calibrate(self):
        total = 0
        for _ in range(CALIBRATION_SAMPLES):
            total += self.touch.raw_value
            time.sleep(0.01)

        self.baseline = total / CALIBRATION_SAMPLES

    def update(self):
        now = time.monotonic()
        raw = self.touch.raw_value

        # Difference from baseline
        delta = raw - self.baseline

        # Detect touch
        is_touched = delta > self.sensitivity

        # Slowly follow baseline when NOT touched
        if not is_touched:
            self.baseline += (raw - self.baseline) * BASELINE_FOLLOW

        # Debounced state change
        if is_touched != self.pressed:
            if now - self.last_change > DEBOUNCE_TIME:
                self.pressed = is_touched
                self.last_change = now
                return True   # state changed

        return False


# ----------------------------------
# Create Touch Keys
# ----------------------------------

keys = [
    TouchKey(board.GP2, Keycode.A, sensitivity=220),
    TouchKey(board.GP3, Keycode.S, sensitivity=220),
    TouchKey(board.GP4, Keycode.D, sensitivity=220),
    TouchKey(board.GP5, Keycode.F, sensitivity=220),
]

# ----------------------------------
# AUTO CALIBRATION
# ----------------------------------

print("Calibrating... do NOT touch pads")

for key in keys:
    key.calibrate()

print("Calibration complete!")
time.sleep(1)

# ----------------------------------
# MAIN LOOP
# ----------------------------------

while True:
    for key in keys:

        if key.update():

            if key.pressed:
                print("PRESS", key.keycode)
                kbd.press(key.keycode)
            else:
                print("RELEASE", key.keycode)
                kbd.release(key.keycode)

    time.sleep(0.005)