#include "DHT.h"
#include <Thermistor.h>

#define DHTPIN    A0 // DHT11 conectado na porta A0 - Analógica
#define THRMPIN   1  // Thermistor conectado na porta A1 - Analógica
#define LDRPIN    2  // LDR conectado na porta A2 - analógica
#define RAINPIN_A A3 // Sensor de chuva na porta A3 - Analógica
#define UVS       A4 // Sensor de raios UV - Analógica
#define PLCSOLAR  A5 // Placa Solar - Analógica

#define READ_INTERVAL 60 // Definição do tempo de leitura do dados, em segundos

#define DHTTYPE DHT11 // Definição do tipo de dado para o sensor DHT 11

#define STATION_ID "5bc68059751bf000e357a249" // Id da estação metereológica

// Leitor de Humidade e temperatura
DHT dht(DHTPIN, DHTTYPE);

// Leitor da segunda Temperatura - Thermistor
Thermistor temp(1);

// Formata em Json e transfere os dados pela serial
// Não foi utilizada a biblioteca de Json para economizar a utilização de memória
void transferDataToNodeMCU(
  String arduinoId,
  float humidity,
  float temperatureHumidity,
  float ambienceTemperature,
  int lightIntensity,
  float rainfall,
  int uvRay,
  float sunCapability
) {
    String aux;

    String jsonData = "{\"arduinoId\": \"" + String(arduinoId)           + "\", ";
    jsonData += "\"humidity\":"            + String(humidity)            + ", ";
    jsonData += "\"temperatureHumidity\":" + String(temperatureHumidity) + ", ";
    jsonData += "\"ambienceTemperature\":" + String(ambienceTemperature) + ", ";
    jsonData += "\"lightIntensity\":"      + String(lightIntensity)      + ", ";
    jsonData += "\"rainfall\":"            + String(rainfall)            + ", ";
    jsonData += "\"uvRay\":"               + String(uvRay)               + ", ";
    jsonData += "\"sunCapability\":"       + String(sunCapability)       + "}";

    // Limpa a sujeira da serial
    if (Serial.available() > 0) {
        aux = Serial.readString();
    }

    // Transmite a string pela serial
    Serial.println(jsonData);

    // Espera o Buffer ser esvaziado
    delay(READ_INTERVAL / 2*1000);

    // Espera e Lê a confirmação de leitura
    while (Serial.available() < 0);
    aux = Serial.readString();
}

void setup() 
{
    // Inicializa Serial
    Serial.begin(115200);

    // Inicializando o DHT11
    dht.begin();

    // Define o pino do sensor de luminosidade como entrada
    pinMode(LDRPIN, INPUT);

    // Define os pinos do sensor de chuva como entrada
    pinMode(RAINPIN_A, INPUT);

    // Define os pino do sensor de UV
    pinMode(UVS, INPUT);

    // Define os Pinos da placa solar
    pinMode(PLCSOLAR, INPUT);
}
 
void loop() 
{
    // Leitura do DHT11 - Humidade e Temperatura - Valores: Humidade em % e Temperatura em Graus Celsius
    float humidity = dht.readHumidity();
    float temperatureHumidity = dht.readTemperature();
    // Final da leitura do DHT11

    // Leituda da Segunda temperatura - Thermistor - Temperatura em Graus Celsius
    float ambienceTemperature = temp.getTemp();
    // Final Leitura do Thermistor

    // Leitura da luminosidade - LDR - Intensidade - 0 (Muita Luminosidade) - 1024 (Sem Luminosidade)
    int lightIntensity = analogRead(LDRPIN);
    // Final Leitura de Luminosidade

    // Leitura do sensor de chuva - Valor Digital - 0 (chuva) ou 1 (sem chuva) - analógivo a intensidade da chuva (0 (Chuva Forte) - 1024 (Sem chuva))
    int rainfall = analogRead(RAINPIN_A);//* (5.0 / 1023.0);
    // Final da leitura do sensor de chuva

    // Leitura do Sensor UV
    int uvRay = analogRead(UVS);
    // Final da leitura do Sensor UV

    // Leitura do Sensor de humidadde do Solo
    float sunCapability = analogRead(PLCSOLAR) * (5.0 / 1023.0);
    // Final da leitura do Sensor humidadde do Solo

    // Formata e transfere os dados para o NodeMCU via Serial
    transferDataToNodeMCU(
        STATION_ID,
        humidity,
        temperatureHumidity,
        ambienceTemperature,
        lightIntensity,
        rainfall,
        uvRay,
        sunCapability
    );

    delay(READ_INTERVAL / 2*1000);
}
