#include <BLE.h>
#include <Servo.h>

BLEServiceUART uart;

Servo servo1;
Servo servo2;
Servo servos[2] = { servo1, servo2 };
int servopins[2] = { 2, 3 };

void setup() {
  delay(5000);

  Serial.begin(115200);
  
  BLE.begin("PicoUART_patrick");
  BLE.server()->addService(&uart);
  BLE.startAdvertising();
  uart.setAutoflush(50);

  Serial.println("start");
  pinMode(2, OUTPUT);

  setServo(0,90);
  setServo(1,90);
}

char buffer[11] = {};

/*
  0 = faceX, 
  1 = faceY, 
  2 = faceDistance, 
  3 = faceYaw,
  4 = facePitch,
  5 = mouthOpenness, 
  6 = leftEyeOpenness, 
  7 = rightEyeOpenness, 
  8 = faceRoll ,
  9 = smile, 
  10 = faceVisible
*/

// ASSUMING DATA CAN'T GET LOST 
// AND WE ALWAYAS START AT 0
int cnt = 0;
void loop() {
  while (uart.available()) {
    int c = uart.read();
    buffer[cnt] = c;
    if( cnt == 10 ){
      setServo(0,(int)buffer[3]); // yaw
      setServo(1,(int)buffer[5]); // mouth
      cnt =0;
    }else{
      cnt++;
    }
  }
}

void setServo(int i, int position) {
//  if(i >= 0  && i <= 2){
    servos[i].attach(servopins[i]); //assume we start at pin 2, so servo 0 will be at pin 2
    servos[i].write(position * 1.8);
  //}
}
