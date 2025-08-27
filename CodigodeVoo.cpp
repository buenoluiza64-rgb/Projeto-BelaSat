#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <Adafruit_CCS811.h>
#include <MPU9250.h>

#define EQUIPE 2053          // numero da belasat
#define PINO_LUZ 34
#define PINO_BAT 35
#define PINO_CS 5

//dados que serao armazenados
Adafruit_CCS811 V_ccs;
MPU9250 V_mpu;
File V_Arquivo;

uint16_t V_CO2;
uint16_t V_TVOC;
int V_Luz;
int V_Bateria;
String V_DataHora;

//iniciando o contato entre processador e sensores 
float sht20_temperature() {
  Wire.beginTransmission(0x40);
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(70);
  Wire.requestFrom(0x40, 2);
  return -46.86 + 175.72 * (((Wire.read() << 8) | Wire.read()) / 65535.0);
}

float sht20_humidity() {
  Wire.beginTransmission(0x40);
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(70);
  Wire.requestFrom(0x40, 2);
  return -6.0 + 125.0 * (((Wire.read() << 8) | Wire.read()) / 65535.0);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  //leitura em 12 bits
  analogReadResolution(12);
  analogSetPinAttenuation(PINO_LUZ, ADC_11db);
  analogSetPinAttenuation(PINO_BAT, ADC_11db);

// iniciando os sensores e o cartao sd    
  V_ccs.begin(0x5A);      
  V_mpu.setup(0x68);      
  SD.begin(PINO_CS);       
}

// looping
void loop() {

//leitura dos dados
    V_ccs.readData();
    V_CO2 = V_ccs.getCO2();
    V_TVOC = V_ccs.getTVOC();

  V_Luz = analogRead(PINO_LUZ);
  V_Bateria = analogRead(PINO_BAT);

  float V_Temp = sht20_temperature();
  float V_Umi  = sht20_humidity();

  V_mpu.update();

  // hora
  V_DataHora = String(millis()); // marca o tempo desde que o sat foi ligado

  // montar o json
  String V_JSON = "{";
  V_JSON += "\"Equipe\":" + String(EQUIPE) + ",";
  V_JSON += "\"CO2\":" + String(V_CO2) + ",";
  V_JSON += "\"VOR\":" + String(V_TVOC) + ",";
  V_JSON += "\"Data\":" + V_DataHora + ",";
  V_JSON += "\"Luz\":" + String(V_Luz) + ",";
  V_JSON += "\"Temperatura\":" + String(V_Temp, 2) + ",";
  V_JSON += "\"Umidade\":" + String(V_Umi, 2) + ",";
  V_JSON += "\"B\":[" + String(V_mpu.getAccX(), 3) + "," + String(V_mpu.getAccY(), 3) + "," + String(V_mpu.getAccZ(), 3) + "],";
  V_JSON += "\"G\":[" + String(V_mpu.getGyroX(), 3) + "," + String(V_mpu.getGyroY(), 3) + "," + String(V_mpu.getGyroZ(), 3) + "],";
  V_JSON += "\"M\":[" + String(V_mpu.getMagX(), 3) + "," + String(V_mpu.getMagY(), 3) + "," + String(V_mpu.getMagZ(), 3) + "]";
  V_JSON += "}";

  // salvando o json
  V_Arquivo = SD.open("/log.json", FILE_APPEND);
  if (V_Arquivo) {
    V_Arquivo.println(V_JSON);
    V_Arquivo.close();
    Serial.println(V_JSON);
  }

  delay(240000); // quatro minutos de intervalo
}
