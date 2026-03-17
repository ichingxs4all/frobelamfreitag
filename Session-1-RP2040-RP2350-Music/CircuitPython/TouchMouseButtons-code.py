import time
import board
import touchio
import usb_hid

from adafruit_hid.mouse import Mouse

# ======================================
# SETTINGS
# ======================================

CALIBRATION_SAMPLES = 40
SENSITIVITY = 220          # lower = more sensitive
BASELINE_FOLLOW = 0.002    # drift compensation
DEBOUNCE = 0.03            # seconds

# ======================================
# USB MOUSE
# ======================================

mouse = Mouse(usb_hid.devices)

# ======================================
# TOUCH BUTTON CLASS
# ======================================

class TouchButton:

    def __init__(self, pin, mouse_button):
        self.touch = touchio.TouchIn(pin)
        self.mouse_button = mouse_button

        self.baseline = 0
        self.pressed = False
        self.last_change = 0

    # ---- auto calibration ----
    def calibrate(self):
        total = 0
        for _ in range(CALIBRATION_SAMPLES):
            total += self.touch.raw_value
            time.sleep(0.01)

        self.baseline = total / CALIBRATION_SAMPLES

    # ---- update logic ----
    def update(self):
        now = time.monotonic()
        raw = self.touch.raw_value

        delta = raw - self.baseline
        touched = delta > SENSITIVITY

        # baseline drift correction
        if not touched:
            self.baseline += (raw - self.baseline) * BASELINE_FOLLOW

        # debounce state change
        if touched != self.pressed and (now - self.last_change) > DEBOUNCE:
            self.pressed = touched
            self.last_change = now

            if touched:
                mouse.press(self.mouse_button)
                print("PRESS", self.mouse_button)
            else:
                mouse.release(self.mouse_button)
                print("RELEASE", self.mouse_button)


# ======================================
# CREATE TOUCH BUTTONS
# ======================================

buttons = [
    TouchButton(board.GP0, Mouse.LEFT_BUTTON),
    TouchButton(board.GP1, Mouse.RIGHT_BUTTON),
    TouchButton(board.GP2, Mouse.MIDDLE_BUTTON),
]

# ======================================
# CALIBRATION
# ======================================

print("Calibrating... don't touch pads")

for b in buttons:
    b.calibrate()

print("Mouse ready!")

time.sleep(1)

# ======================================
# MAIN LOOP
# ======================================

while True:
    for b in buttons:
        b.update()

    time.sleep(0.005)
    