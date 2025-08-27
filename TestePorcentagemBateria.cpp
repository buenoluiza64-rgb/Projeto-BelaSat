#include <Arduino.h>

#define PIN_ADC 35
#define PIN_BUZZER 25

float lerBateria() {
  int leituraADC = analogRead(PIN_ADC);
  return ((float)leituraADC / 2384.0) * 100.0;
}

void buzzerAlerta(int duracaoMs) {
  ledcAttachPin(PIN_BUZZER, 0);
  ledcSetup(0, 1200, 10);
  ledcWrite(0, 512);
  delay(duracaoMs);
  ledcWrite(0, 0);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUZZER, OUTPUT);
}

void loop() {
  float bateria = lerBateria();
  Serial.print("Bateria: ");
  Serial.println(bateria);

  if (bateria < 21) {
    Serial.println("BATERIA BAIXA");
    buzzerAlerta(3000);
  }

  delay(1000);
}
