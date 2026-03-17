import time
import board
import touchio
import usb_hid

from adafruit_hid.mouse import Mouse

# ==================================================
# SETTINGS
# ==================================================

CAL_SAMPLES = 40
SENSITIVITY = 220
BASELINE_FOLLOW = 0.002

MOVE_SPEED = 6
SCROLL_SPEED = 1

SCAN_DELAY = 0.01

mouse = Mouse(usb_hid.devices)

# ==================================================
# TOUCH SENSOR CLASS
# ==================================================

class TouchSensor:

    def __init__(self, pin):
        self.touch = touchio.TouchIn(pin)
        self.baseline = 0

    def calibrate(self):
        total = 0
        for _ in range(CAL_SAMPLES):
            total += self.touch.raw_value
            time.sleep(0.01)
        self.baseline = total / CAL_SAMPLES

    def strength(self):
        raw = self.touch.raw_value
        delta = raw - self.baseline

        touched = delta > SENSITIVITY

        if not touched:
            # drift compensation
            self.baseline += (raw - self.baseline) * BASELINE_FOLLOW
            return 0

        return delta


# ==================================================
# CREATE SENSORS
# ==================================================

left  = TouchSensor(board.GP0)
right = TouchSensor(board.GP1)
up    = TouchSensor(board.GP2)
down  = TouchSensor(board.GP3)
click = TouchSensor(board.GP4)

sensors = [left, right, up, down, click]

# ==================================================
# CALIBRATION
# ==================================================

print("Calibrating touchpad... don't touch")
for s in sensors:
    s.calibrate()

print("Trackpad ready!")

time.sleep(1)

click_pressed = False

# ==================================================
# MAIN LOOP
# ==================================================

while True:

    L = left.strength()
    R = right.strength()
    U = up.strength()
    D = down.strength()
    C = click.strength()

    # ---------------------------
    # CLICK HANDLING
    # ---------------------------
    if C and not click_pressed:
        mouse.press(Mouse.LEFT_BUTTON)
        click_pressed = True

    if not C and click_pressed:
        mouse.release(Mouse.LEFT_BUTTON)
        click_pressed = False

    # ---------------------------
    # MOVEMENT VECTOR
    # ---------------------------
    dx = (R - L) / 200
    dy = (D - U) / 200

    dx = int(dx * MOVE_SPEED)
    dy = int(dy * MOVE_SPEED)

    # ---------------------------
    # SCROLL GESTURE
    # (two fingers = up+down)
    # ---------------------------
    scrolling = (U and D)

    if scrolling:
        wheel = int((D - U) / 300 * SCROLL_SPEED)
        if wheel != 0:
            mouse.move(wheel=wheel)
    else:
        if dx != 0 or dy != 0:
            mouse.move(dx, dy)

    time.sleep(SCAN_DELAY)