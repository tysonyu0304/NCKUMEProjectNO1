#include <Arduino.h>

const int buttonPins[8] = {0, 1, 2, 3, 4, 6, 7, 9};
#define HC12_SET 8

// 兩邊嚴格統一：6組類比(12 Bytes) + 8組按鈕(8 Bytes) + 起迄碼(2 Bytes) = 22 Bytes
struct __attribute__((__packed__)) RobotControlPacket {
  char startMarker = '<';     
  uint16_t analogSignals[6];  
  uint8_t btnState[8];        // 這裡直接用 8 個 Byte 存 8 顆按鈕，完全對應接收端
  char endMarker = '>';       
};

RobotControlPacket txPacket;  

void setup() {
  Serial.begin(115200);       
  Serial1.begin(9600);        
  analogReadResolution(12);   
  
  pinMode(HC12_SET, OUTPUT);
  digitalWrite(HC12_SET, HIGH);
  
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  txPacket.analogSignals[0] = analogRead(A1);
  txPacket.analogSignals[1] = analogRead(A2);
  txPacket.analogSignals[2] = analogRead(A3);
  txPacket.analogSignals[3] = analogRead(A4);
  txPacket.analogSignals[4] = analogRead(A5);
  txPacket.analogSignals[5] = analogRead(A6);

  // 直接讀取並寫入陣列，0代表放開，1代表按下
  for (int i = 0; i < 8; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      txPacket.btnState[i] = 1; 
    } else {
      txPacket.btnState[i] = 0;
    }
  }

  Serial1.write((uint8_t*)&txPacket, sizeof(txPacket));
  delay(18); // 55 Hz
}