#include <Arduino.h>

#define TX1_PIN 4
#define RX1_PIN 5
#define SET_PIN 22 

// 結構體定義必須與發射端一模一樣
struct __attribute__((__packed__)) RobotControlPacket {
  char startMarker;           
  uint16_t analogSignals[6];  
  uint8_t btnState[8];        // 與發射端完美對齊的 8 顆按鈕陣列
  char endMarker;             
};

RobotControlPacket rxPacket;  

unsigned long lastPlotTime = 0;
const unsigned long PLOT_INTERVAL = 25; // 40 Hz 繪圖器

void setup() {
  Serial.begin(115200);       
  Serial1.setTX(TX1_PIN);  
  Serial1.setRX(RX1_PIN);
  Serial1.begin(9600);        

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH); 
}

void loop() {
  // 檢查是否有完整的 22 位元組封包
  while (Serial1.available() >= sizeof(RobotControlPacket)) {
    if (Serial1.peek() != '<') {
      Serial1.read(); 
      continue;
    }

    Serial1.readBytes((uint8_t*)&rxPacket, sizeof(RobotControlPacket));

    if (rxPacket.endMarker == '>') {
      // 成功收到！現在你要用按鈕只需要直接呼叫 rxPacket.btnState[0] 到 [7] 即可
    }
  }

  if (millis() - lastPlotTime >= PLOT_INTERVAL) {
    lastPlotTime = millis();

    Serial.print("Analog_A1:");
    Serial.print(rxPacket.analogSignals[0]);
    Serial.print(",");
    
    Serial.print("Analog_A2:");
    Serial.print(rxPacket.analogSignals[1]);
    Serial.print(",");

    // 直接拿結構體內的陣列數值乘以 1000 輸出給繪圖器
    Serial.print("Btn_1_x1000:");
    Serial.print(rxPacket.btnState[0] * 1000);
    Serial.print(",");

    Serial.print("Btn_2_x1000:");
    Serial.println(rxPacket.btnState[1] * 1000); 
  }
}