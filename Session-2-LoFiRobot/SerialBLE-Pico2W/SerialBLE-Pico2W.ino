#include <BLE.h>

BLEServiceUART uart;


void setup() {
  delay(5000);
  BLE.begin("PicoUART");
  BLE.server()->addService(&uart);
  BLE.startAdvertising();
  uart.setAutoflush(50);
}

int cnt = 0;
void loop() {
  while (uart.available()) {
    Serial.println(uart.read());
  }
}
