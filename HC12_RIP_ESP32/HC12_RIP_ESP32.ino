#include <Arduino.h>
#include <ESP32Servo.h> // 注意：ESP32 建議使用這個庫以獲得更好的 PWM 相容性

// --- 腳位定義 ---
#define HC12_TX 17    // 接 HC-12 的 RX
#define HC12_RX 16    // 接 HC-12 的 TX
#define SET_PIN 22    // HC-12 模式控制
#define SERVO_PIN 21  // MG90 / MG996R 訊號線
#define LED_PIN 2     // ESP32 內建 LED 通常在 GPIO 2

// --- 校準參數 ---
const int CENTER = 1867;   
const int DEADZONE = 100;  
const float SPEED_SCALE = 0.001;

String rxString = "";
int x_val = 1867; 
int btn = 1;

Servo myServo;  
float currentAngle = 90.0;

void parseData(String data);

void setup() {
  Serial.begin(115200);
  
  // ESP32 Serial2 初始化 (RX, TX)
  Serial2.begin(9600, SERIAL_8N1, HC12_RX, HC12_TX); 

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH); 
  
  pinMode(LED_PIN, OUTPUT);

  // ESP32Servo 的 attach
  myServo.setPeriodHertz(50);    // 標準伺服馬達頻率 50Hz
  myServo.attach(SERVO_PIN, 500, 2500); 
  myServo.write(90);   
}

void loop() {
  // 1. 接收與解析
  while (Serial2.available()) {        
    digitalWrite(LED_PIN, HIGH);
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
    digitalWrite(LED_PIN, LOW);
  }

  // 2. 計算偏移量與增量控制
  int offset = x_val - CENTER; 

  if (abs(offset) > DEADZONE) {
    currentAngle += (float)offset * SPEED_SCALE; 
    
    if (currentAngle > 180.0) currentAngle = 180.0;
    if (currentAngle < 0.0) currentAngle = 0.0;

    myServo.write((int)currentAngle);
  }

  // 3. 按鈕歸零
  if (btn == 0) { 
    currentAngle = 90.0;
    myServo.write(90);
  }
  
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