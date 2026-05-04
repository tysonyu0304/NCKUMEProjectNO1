// 針對 Raspberry Pi Pico 2 預設腳位最佳化
// UART1 預設腳位：TX=GP4, RX=GP5
#define TX2 4
#define RX2 5
#define SET_PIN 22 // 改用 GP22，避開可能的內部衝突

void setup() {
  Serial.begin(115200);
  // 等待 USB Serial 穩定，防止開啟失敗
  for(int i=0; i<10; i++) {
    delay(100);
  }

  // 初始化硬體 UART1 (GP4, GP5)
  // 在 Pico 2 中，Serial1 預設就是這組，不進行額外 setTX/RX 以免驅動報錯
  Serial2.setTX(TX2);  
  Serial2.setRX(RX2);
  Serial2.begin(9600); 

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, LOW); // 強制進入 AT 模式
  
  Serial.println("--- Pico 2 UART1 Basic Test ---");
}

void loop() {
  // 電腦 -> HC-12
  while (Serial.available()) {
    Serial2.write(Serial.read());
  }

  // HC-12 -> 電腦
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}

void loop() {
  Serial.println(count++);
  delay(100);
}