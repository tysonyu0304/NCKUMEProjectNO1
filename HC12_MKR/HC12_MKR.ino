#include <Arduino.h>

// 定義 MKR1000 接線腳位
#define HC12_SET  5   // 接 HC-12 的 SET 腳位
#define Stick1_X  A1  // 搖桿 X 軸接類比 A1
#define Stick1_Y  A2  // 搖桿 Y 軸接類比 A2 (新加入)
#define Stick1_b  4   // 搖桿按鈕接數位 4

char txBuffer[32];

void setup() {
  Serial.begin(115200);    // 與電腦通訊的波特率
  Serial1.begin(9600);     // 初始化 HC-12 硬體串口 (Pin 13, 14)
  
  pinMode(HC12_SET, OUTPUT);
  pinMode(Stick1_b, INPUT_PULLUP);
  
  // 讓 MKR1000 的 analogRead 範圍變成 0-4095 (12位元)
  // 這樣能跟原先 ESP32 的數值解析度完全對齊
  analogReadResolution(12); 
  
  // 進入通訊模式
  digitalWrite(HC12_SET, HIGH);
  delay(100); 
} 

void loop() {
  int x_val = analogRead(Stick1_X);     // 讀取 X 軸 (0-4095)
  int y_val = analogRead(Stick1_Y);     // 讀取 Y 軸 (0-4095)
  int btn_val = digitalRead(Stick1_b);  // 讀取按鈕 (0 或 1)

  // 封裝成新格式: <X,Y,Btn>
  sprintf(txBuffer, "<%d,%d,%d>", x_val, y_val, btn_val);

  // 透過 HC-12 發送
  Serial1.print(txBuffer);

  Serial.print("Joystick_X:");
  Serial.print(x_val);
  Serial.print(",");
  Serial.print("Joystick_Y:");
  Serial.println(y_val); // 最後一個一定要用 println

  delay(50); // 將延遲縮短到 50ms（每秒更新 20 次），繪圖器的曲線才會流暢

  delay(100); // 保持 100ms 傳送頻率
}