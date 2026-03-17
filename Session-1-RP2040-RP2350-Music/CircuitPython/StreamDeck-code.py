import time
import board
import touchio
import usb_hid

from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# =====================================================
# CONFIGURATION
# =====================================================

CALIBRATION_SAMPLES = 40
BASELINE_FOLLOW = 0.002
DEBOUNCE = 0.03
HOLD_TIME = 0.35

# Layers
BASE = 0
FN = 1

active_layer = BASE

kbd = Keyboard(usb_hid.devices)

# =====================================================
# TOUCH KEY ENGINE
# =====================================================

class TouchKey:

    def __init__(self, pin, sensitivity=220):
        self.touch = touchio.TouchIn(pin)
        self.sensitivity = sensitivity

        self.baseline = 0
        self.pressed = False
        self.press_time = 0
        self.last_change = 0
        self.hold_triggered = False

        self.actions = {}

    # ---------- calibration ----------
    def calibrate(self):
        total = 0
        for _ in range(CALIBRATION_SAMPLES):
            total += self.touch.raw_value
            time.sleep(0.01)
        self.baseline = total / CALIBRATION_SAMPLES

    # ---------- logic ----------
    def update(self):
        global active_layer

        now = time.monotonic()
        raw = self.touch.raw_value

        delta = raw - self.baseline
        touched = delta > self.sensitivity

        # baseline drift correction
        if not touched:
            self.baseline += (raw - self.baseline) * BASELINE_FOLLOW

        if touched != self.pressed and now - self.last_change > DEBOUNCE:
            self.pressed = touched
            self.last_change = now

            if touched:
                self.press_time = now
                self.hold_triggered = False
            else:
                self.on_release()

        # HOLD detection
        if self.pressed and not self.hold_triggered:
            if now - self.press_time > HOLD_TIME:
                self.hold_triggered = True
                self.on_hold()

    # ---------- actions ----------
    def action(self):
        return self.actions.get(active_layer)

    def on_hold(self):
        act = self.action()
        if act and "hold" in act:
            act["hold"]()

    def on_release(self):
        act = self.action()

        if not act:
            return

        # tap only if hold didn't fire
        if not self.hold_triggered and "tap" in act:
            act["tap"]()

        if "release" in act:
            act["release"]()


# =====================================================
# ACTION HELPERS
# =====================================================

def send_key(*keys):
    def action():
        kbd.send(*keys)
    return action


def press_key(key):
    return lambda: kbd.press(key)


def release_key(key):
    return lambda: kbd.release(key)


def momentary_layer(layer):
    def hold():
        global active_layer
        active_layer = layer
        print("Layer ->", layer)

    def release():
        global active_layer
        active_layer = BASE
        print("Layer -> BASE")

    return hold, release


# =====================================================
# CREATE TOUCH KEYS
# =====================================================

keys = [
    TouchKey(board.GP2),
    TouchKey(board.GP3),
    TouchKey(board.GP4),
    TouchKey(board.GP5),
]

# =====================================================
# KEYMAP (LIKE QMK)
# =====================================================

# --- KEY 0 ---
hold, rel = momentary_layer(FN)
keys[0].actions = {
    BASE: {
        "tap": send_key(Keycode.A),
        "hold": hold,
        "release": rel,
    },
    FN: {
        "tap": send_key(Keycode.F1),
    },
}

# --- KEY 1 ---
keys[1].actions = {
    BASE: {"tap": send_key(Keycode.S)},
    FN: {"tap": send_key(Keycode.F2)},
}

# --- KEY 2 ---
keys[2].actions = {
    BASE: {"tap": send_key(Keycode.D)},
    FN: {"tap": send_key(Keycode.F3)},
}

# --- KEY 3 (macro example) ---
keys[3].actions = {
    BASE: {
        "tap": send_key(
            Keycode.CONTROL,
            Keycode.SHIFT,
            Keycode.ESCAPE
        )
    },
    FN: {
        "tap": send_key(Keycode.F4)
    }
}

# =====================================================
# CALIBRATION
# =====================================================

print("Calibrating touch sensors...")
for k in keys:
    k.calibrate()

print("Ready!")

# =====================================================
# MAIN LOOP
# =====================================================

while True:
    for k in keys:
        k.update()

    time.sleep(0.005)