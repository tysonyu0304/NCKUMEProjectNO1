#include <Arduino.h>

// 1. 建立按鈕腳位陣列，方便用迴圈讀取
const int buttonPins[8] = {0, 1, 2, 3, 4, 6, 7, 9};

// 2. 定義 HC-12 控制腳位
#define HC12_SET 8

// 3. 定義嚴格對齊的二進制通訊結構體（大小固定為 15 Bytes）
struct __attribute__((__packed__)) RobotControlPacket {
  char startMarker = '<';     // 1 Byte
  uint16_t analogSignals[6];  // 12 Bytes (6 組 0~4095 類比值)
  uint8_t digitalBits;        // 1 Byte  (用 8 個 bit 塞滿 8 顆按鈕)
  char endMarker = '>';       // 1 Byte
};

RobotControlPacket txPacket;  // 宣告封包實體

void setup() {
  Serial.begin(115200);       // 電腦 Debug 用波特率
  Serial1.begin(9600);        // HC-12 預設通訊波特率
  
  // 設定 MKR1000 類比解析度為 12 位元 (0-4095)
  analogReadResolution(12);   
  
  // 初始化 HC-12 的 SET 腳位並進入通訊模式
  pinMode(HC12_SET, OUTPUT);
  digitalWrite(HC12_SET, HIGH);
  
  // 使用迴圈將 8 個按鈕腳位全部初始化為上拉電阻模式
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  // 核心步驟 1：依序讀取 A1 到 A6 的 6 組類比訊號
  txPacket.analogSignals[0] = analogRead(A1);
  txPacket.analogSignals[1] = analogRead(A2);
  txPacket.analogSignals[2] = analogRead(A3);
  txPacket.analogSignals[3] = analogRead(A4);
  txPacket.analogSignals[4] = analogRead(A5);
  txPacket.analogSignals[5] = analogRead(A6);

  // 核心步驟 2：讀取 8 顆按鈕，並壓縮進單一 Byte 的 8 個位元（Bit）中
  txPacket.digitalBits = 0; // 先清空舊資料
  for (int i = 0; i < 8; i++) {
    // 因為是 INPUT_PULLUP，沒按下是 HIGH(1)，按下是 LOW(0)
    // 為了直覺，我們設定「按下時」將該位元設為 1
    if (digitalRead(buttonPins[i]) == LOW) {
      txPacket.digitalBits |= (1 << i); // 將第 i 個位元強制寫入 1
    }
  }

  // 核心步驟 3：將這 15 位元組的二進制記憶體區塊直接倒給 HC-12
  Serial1.write((uint8_t*)&txPacket, sizeof(txPacket));

  // 電腦端 Debug 輸出（可選：如果你想在電腦上看二進制狀態）
  /*
  Serial.print("Analog[0]: "); Serial.print(txPacket.analogSignals[0]);
  Serial.print(" | Bits: "); Serial.println(txPacket.digitalBits, BIN);
  */

  // 核心步驟 4：關鍵延遲 18ms
  // 加上 MCU 讀取腳位與串列埠寫入的時間，整體迴圈頻率會精準落在 55 Hz 左右
  delay(18); 
}