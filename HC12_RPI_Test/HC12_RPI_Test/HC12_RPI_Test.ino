#define TX2 4
#define RX2 5
#define SET_PIN 22 // 改用 GP22，避開可能的內部衝突

#ifndef LED_BUILTIN
#define LED_BUILTIN 25
#endif

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
  // digitalWrite(SET_PIN, LOW); // 強制進入 AT 模式
  digitalWrite(SET_PIN, HIGH); // 強制進入 通訊 模式
  
  // Serial.println("--- Pico 2 UART1 Basic Test ---");
}

void loop() {
  while (Serial2.available()) {        // If HC-12 has data
    digitalWrite(LED_BUILTIN, HIGH);
    while (Serial2.available())
      Serial.write(Serial2.read());      // Send the data to Serial monitor
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
  }
  while (Serial.available()) {      // If Serial monitor has data
    digitalWrite(LED_BUILTIN, HIGH);
    while (Serial.available())
      Serial2.write(Serial.read());      // Send that data to HC-12
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
