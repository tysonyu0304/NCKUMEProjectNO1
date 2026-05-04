#include <HardwareSerial.h>

// 定義接線腳位
#define HC12_RX 16
#define HC12_TX 17
#define HC12_SET 18  // 建議將 SET 接在 GPIO 18，方便自動切換模式
#define Stick1 34
#define Stick1_b 22

HardwareSerial HC12(2); // 使用硬體串口 2

char txBuffer[32];

void setup() {
  Serial.begin(115200);    // 與電腦通訊的波特率
  
  // HC-12 預設波特率通常為 9600
  HC12.begin(9600, SERIAL_8N1, HC12_RX, HC12_TX);
  
  pinMode(HC12_SET, OUTPUT);
  // pinMode(Stick1, INPUT);
  pinMode(Stick1_b, INPUT_PULLUP);
  
  // 進入 AT 模式：將 SET 拉低
  // digitalWrite(HC12_SET, LOW);
  // 進入通訊模式
  digitalWrite(HC12_SET, HIGH);
  delay(100); // 給模組一點反應時間
  
  // Serial.println("--- HC-12 AT Mode Test Start ---");
  // Serial.println("請在監控器輸入 'AT' (記得選 Both NL & CR)");
} 
void loop() {
  int Stick1_val = analogRead(Stick1);
  int Stick1_b_val = digitalRead(Stick1_b);

  sprintf(txBuffer, "<%d,%d>", Stick1_val, Stick1_b_val);

  HC12.print(txBuffer);

  Serial.print("Sending: ");
  Serial.println(txBuffer);

  delay(100);
}