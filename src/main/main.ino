#include "DHT.h"
#include <Thermistor.h>

#define DHTPIN    A0 // DHT11 conectado na porta A0 - Analógica
#define THRMPIN   1  // Thermistor conectado na porta A1 - Analógica
#define LDRPIN    2  // LDR conectado na porta A2 - analógica
#define RAINPIN_A A3 // Sensor de chuva na porta A3 - Analógica
#define UVS       A4 // Sensor de raios UV - Analógica
#define PLCSOLAR  A5 // Placa Solar - Analógica

#define TEMPO_LEITURA 60 // Definição do tempo de leitura do dados, em segundos

#define DHTTYPE DHT11 // Definição do tipo de dado para o sensor DHT 11

#define ID_EM "5bc90f8ece15980006543831" // Id da estação metereológica

// Leitor de Humidade e temperatura
DHT dht(DHTPIN, DHTTYPE);

//Leitor da segunda Temperatura - Thermistor
Thermistor temp(1);

// Formata em Json e transfere os dados pela serial
// Não foi utilizada a biblioteca de Json para economizar a utilização de memória
void transfereDadosNodeMCU(
    String id_sensor,
    float umidadeDHT11,
    float temperaturaDHT11,
    float temperaturaTherm,
    int luminosidadeLDR,
    float val_rain,
    int valor_UV,
    float valor_PLCSOL
) {
    String aux;

    String strEnv = "{\"arduinoId\": \"" + String(id_sensor) + "\", ";
    strEnv += "\"humidity\":" + String(umidadeDHT11) + ", ";
    strEnv += "\"temperatureHumidity\":" + String(temperaturaDHT11) + ", ";
    strEnv += "\"ambienceTemperature\":" + String(temperaturaTherm) + ", ";
    strEnv += "\"lightIntensity\":" + String(luminosidadeLDR) + ", ";
    strEnv += "\"rainfall\":" + String(val_rain) + ", ";
    strEnv += "\"uvRay\":" + String(valor_UV) + ", ";
    strEnv += "\"sunCapability\":" + String(valor_PLCSOL) + "}";

    // Limpa a sujeira da serial
    if (Serial.available() > 0) {
        aux = Serial.readString();
    }

    //Transmite a string pela serial
    Serial.println(strEnv);

    // Espera o Buffer ser esvaziado
    delay(TEMPO_LEITURA/2*1000);

    //Espera e Lê a confirmação de leitura
    while (Serial.available() < 0);
        aux = Serial.readString();
}


void setup() 
{
    //Inicializa Serial
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
    float umidadeDHT11 = dht.readHumidity();
    float temperaturaDHT11 = dht.readTemperature();

    // Leituda da Segunda temperatura - Thermistor - Temperatura em Graus Celsius
    float temperaturaTherm = temp.getTemp();

    // Leitura da luminosidade - LDR - Intensidade - 0 (Muita Luminosidade) - 1024 (Sem Luminosidade)
    int luminosidadeLDR = analogRead(LDRPIN);

    // Leitura do sensor de chuva - Valor Digital - 0 (chuva) ou 1 (sem chuva) - analógivo a intensidade da chuva (0 (Chuva Forte) - 1024 (Sem chuva))
    int val_rain = analogRead(RAINPIN_A);//* (5.0 / 1023.0);

    // Leitura do Sensor UV
    int valor_UV = analogRead(UVS);

    // Leitura do Sensor de humidadde do Solo
    float valor_PLCSOL = analogRead(PLCSOLAR) * (5.0 / 1023.0);

    // Formata e transfere os dados para o NodeMCU via Serial
    transfereDadosNodeMCU(
        ID_EM,
        umidadeDHT11,
        temperaturaDHT11,
        temperaturaTherm,
        luminosidadeLDR,
        val_rain,
        valor_UV,
        valor_PLCSOL
    );

    delay(TEMPO_LEITURA/2*1000);
}
