import time
from machine import Pin, PWM

# Construct PWM object, with LED on Pin(25).
S0 = PWM(Pin(0))

# Set the PWM frequency.
S0.freq(50) # 20ms
# duty_u16 max 65535
S0.duty_u16(0)

for x in range(3):
    print("time: ", x+1)
    for i in range(1639,8192,20):  
      S0.duty_u16(i)
      time.sleep(0.04)
    time.sleep(0.1)
    for i in range(8192,1639,-20):
      S0.duty_u16(i)
      time.sleep(0.04)
    time.sleep(0.1)
print("PWM down")
S0.duty_u16(0)





