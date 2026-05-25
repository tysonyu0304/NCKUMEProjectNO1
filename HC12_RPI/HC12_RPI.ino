#include <Arduino.h>

#define TX1_PIN 4
#define RX1_PIN 5
#define SET_PIN 22 

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

// 1. 定義與發射端完全相同的二進位結構體（大小必須嚴格等於 15 Bytes）
struct __attribute__((__packed__)) RobotControlPacket {
  char startMarker;           // 1 Byte
  uint16_t analogSignals[6];  // 12 Bytes
  uint8_t digitalBits;        // 1 Byte
  char endMarker;             // 1 Byte
};

RobotControlPacket rxPacket;  // 建立接收緩衝封包
bool btnState[8] = {0};       // 儲存拆解後的 8 顆按鈕狀態 (0=放開, 1=按下)

// 2. 限制 Serial Plotter 輸出頻率的計時器變數
unsigned long lastPlotTime = 0;
const unsigned long PLOT_INTERVAL = 25; // 25ms 對應約 40 Hz 刷新率

void setup() {
  Serial.begin(115200);       // 電腦端 Serial Plotter 通訊
  
  // 初始化 Pico 2W 的硬體 UART1
  Serial1.setTX(TX1_PIN);  
  Serial1.setRX(RX1_PIN);
  Serial1.begin(9600);        // 配合 HC-12 預設波特率

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH); // 進入正常通訊模式
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // 核心步驟 1：高速檢查序列埠緩衝區是否有完整的 15 字节封包
  while (Serial1.available() >= sizeof(RobotControlPacket)) {
    // 檢查首碼是否為 '<'，若不是則清除 1 Byte 錯位資料，重新對齊
    if (Serial1.peek() != '<') {
      Serial1.read(); 
      continue;
    }

    // 讀取完整的二進位資料並直接寫入結構體記憶體
    Serial1.readBytes((uint8_t*)&rxPacket, sizeof(RobotControlPacket));

    // 驗證尾碼，確認資料沒有中途漏失或錯位
    if (rxPacket.endMarker == '>') {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // 閃爍 LED 代表成功收包
      
      // 核心步驟 2：使用位元遮罩（Bitmask）拆解單一 Byte 中的 8 顆按鈕狀態
      for (int i = 0; i < 8; i++) {
        // 利用 (1 << i) 檢查第 i 個位元是否為 1
        if ((rxPacket.digitalBits & (1 << i)) != 0) {
          btnState[i] = 1; // 按下
        } else {
          btnState[i] = 0; // 放開
        }
      }
    }
  }

  // 核心步驟 3：分流計時器，將 Serial Plotter 的輸出控制在穩定的 40 Hz
  if (millis() - lastPlotTime >= PLOT_INTERVAL) {
    lastPlotTime = millis();

    // 輸出類比訊號 A1、A2 (例如搖桿數值)
    Serial.print("Analog_A1:");
    Serial.print(rxPacket.analogSignals[0]);
    Serial.print(",");
    
    Serial.print("Analog_A2:");
    Serial.print(rxPacket.analogSignals[1]);
    Serial.print(",");

    // 輸出按鈕 1 與 按鈕 2 的狀態 (放大 1000 倍方便在圖表上與類比訊號一同觀察)
    Serial.print("Btn_1_x1000:");
    Serial.print(btnState[0] * 1000);
    Serial.print(",");

    Serial.print("Btn_2_x1000:");
    Serial.println(btnState[1] * 1000); // 最後一筆換行
  }
}