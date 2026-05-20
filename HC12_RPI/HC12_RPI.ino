#include <Arduino.h>

#define TX2 4
#define RX2 5
#define SET_PIN 22 

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

String rxString = "";
int x_val = 1867; 
int y_val = 1867; 
int btn = 1;

void parseData(String data);

void setup() {
  Serial.begin(115200);
  for(int i=0; i<10; i++) {
    delay(100);
  }

  // 初始化 Pico 2 的硬體 UART1
  Serial2.setTX(TX2);  
  Serial2.setRX(RX2);
  Serial2.begin(9600); 

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH); // 進入通訊模式
  
  pinMode(LED_BUILTIN, OUTPUT);
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
      rxString = "";
    } 
    else {
      if (isDigit(c) || c == ',') {
        rxString += c;
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  // 2. 輸出給 Serial Plotter 的專用格式
  // 格式：標籤1:數值1,標籤2:數值2,標籤3:數值3 (最後換行)
  Serial.print("RX_X:");
  Serial.print(x_val);
  Serial.print(",");
  
  Serial.print("RX_Y:");
  Serial.print(y_val);
  Serial.print(",");
  
  // 將按鈕狀態放大 1000 倍，沒按是 1000，按下是 0，這樣在圖表上才看得清楚波形
  Serial.print("RX_Btn:");
  Serial.println(btn * 2000); 

  delay(30); // 縮短延遲至 30ms，讓繪圖器的滾動曲線更流暢
}

// 解析來自 MKR1000 的 <X,Y,Btn> 封包
void parseData(String data) {
  data.trim();
  
  int firstComma = data.indexOf(',');
  int secondComma = data.lastIndexOf(',');
  
  if (firstComma != -1 && secondComma != -1 && firstComma != secondComma) {
    String xStr = data.substring(0, firstComma);
    String yStr = data.substring(firstComma + 1, secondComma);
    String btnStr = data.substring(secondComma + 1);
    
    x_val = xStr.toInt();
    y_val = yStr.toInt();
    btn = btnStr.toInt();
  }
}