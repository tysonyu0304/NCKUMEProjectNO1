#include <HardwareSerial.h>

// 定義接線腳位
#define HC12_RX 16
#define HC12_TX 17
#define HC12_SET 18  // 建議將 SET 接在 GPIO 18，方便自動切換模式

HardwareSerial HC12(2); // 使用硬體串口 2

void setup() {
  Serial.begin(115200);    // 與電腦通訊的波特率
  
  // HC-12 預設波特率通常為 9600
  HC12.begin(9600, SERIAL_8N1, HC12_RX, HC12_TX);
  
  pinMode(HC12_SET, OUTPUT);
  
  // 進入 AT 模式：將 SET 拉低
  // digitalWrite(HC12_SET, LOW);
  // 進入通訊模式
  digitalWrite(HC12_SET, HIGH);
  delay(100); // 給模組一點反應時間
  
  // Serial.println("--- HC-12 AT Mode Test Start ---");
  // Serial.println("請在監控器輸入 'AT' (記得選 Both NL & CR)");
} 
void loop() {
  while (HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
}