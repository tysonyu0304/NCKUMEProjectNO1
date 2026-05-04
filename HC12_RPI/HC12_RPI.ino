#include <Arduino.h>
#include <Servo.h>

#define TX2 4
#define RX2 5
#define SET_PIN 22 

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

// --- 修正 1：將參數移到最外面成為全局變數 ---
const int CENTER = 1867;   
const int DEADZONE = 100;  
const float SPEED_SCALE = 0.001;

String rxString = "";
int x_val = 1867; // 初始設為中心值，避免開機時 offset 亂跳
int btn = 1;

Servo myServo;  
float currentAngle = 90.0;

// 函數預宣告
void parseData(String data);

void setup() {
  Serial.begin(115200);
  for(int i=0; i<10; i++) {
    delay(100);
  }

  Serial2.setTX(TX2);  
  Serial2.setRX(RX2);
  Serial2.begin(9600); 

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH); 
  
  pinMode(LED_BUILTIN, OUTPUT);
  myServo.attach(21, 500, 2500);  
  myServo.write(90);   
}

void loop() {
  // 1. 接收與解析邏輯
  while (Serial2.available()) {        
    digitalWrite(LED_BUILTIN, HIGH);
    char c = Serial2.read();
    
    if (c == '<') {       
      rxString = "";    
    } 
    else if (c == '>') {  
      parseData(rxString); 

      Serial.print(">>> Received -> X: "); 
      Serial.print(x_val);
      Serial.print(" | Button: "); 
      Serial.println(btn < 1 ? "Pressed" : "No");

      rxString = "";
    } 
    else {
      if (isDigit(c) || c == ',') {
        rxString += c;
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  // --- 修正 2：將控制邏輯移出 while 迴圈外，確保隨時更新 ---
  // 2. 計算偏移量
  int offset = x_val - CENTER; 

  // 3. 增量邏輯
  if (abs(offset) > DEADZONE) {
    currentAngle += (float)offset * SPEED_SCALE; 
    
    // 邊界保護
    if (currentAngle > 180.0) currentAngle = 180.0;
    if (currentAngle < 0.0) currentAngle = 0.0;

    myServo.write((int)currentAngle);
  }

  // 4. 按鈕歸零功能
  if (btn == 0) { 
    currentAngle = 90.0;
    myServo.write(90);
  }
  
  // 稍微小延遲，讓 loop 不要跑太快吃掉所有資源
  delay(5);
}

void parseData(String data) {
  data.trim();
  int commaIndex = data.indexOf(',');
  if (commaIndex != -1) {
    String xStr = data.substring(0, commaIndex);
    String btnStr = data.substring(commaIndex + 1);
    
    x_val = xStr.toInt();
    btn = btnStr.toInt();
  }
}