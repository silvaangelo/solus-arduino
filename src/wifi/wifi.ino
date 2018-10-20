#include "ESP8266WiFi.h"

// GLOBALS
#define INTERVALO_LEITURA 30

const char* WIFI_SSID     = "teste-em";
const char* WIFI_PASSWORD = "teste-em*2018";

const char* API_HOST             = "200.133.218.86";
const int   API_PORT             = 8081;
const char* API_MEASURE_ROUTER   = "/ic/public_html/WSEstMet/em.php";

int wifiStatus;

// Função para configuração dos módulos Serial e WIFI do nodeMCU
void setup() {
    Serial.begin(115200);
    delay(5000);

    // Iniciar ligação à rede
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("Waiting for wifi connection...\n");
    }

    Serial.println("Wifi connected\n");

    // Espera o arduino subir e começar a ler.
    delay(10000);
}

// Função para chamada de Webservice REST via POST
String chamaWebservice(String host, int porta, String api, String postData) {
    String retorno;

    wifiStatus = WiFi.status();

    if (wifiStatus == WL_CONNECTED) {
        WiFiClient client;

        if (client.connect(host, porta)) {
            // Chamada do Webservice
            client.println("POST "+ api + " HTTP/1.1");
            client.println("Host:  " + host + ":" + porta);
            client.println("Accept: application/json");
            client.println("Cache-Control: no-cache");
            client.println("Content-Type: application/json");
            client.println("Content-Length:" + String(postData.length()));
            client.println("Connection: close");
            client.println();
            client.println(postData);
            client.println();

            while (client.available() || client.connected()) {
                retorno += client.readStringUntil('\r');
            }

            client.stop();
        }
    }

    return retorno;
}

// Função para receber os dados do ARDUINO via Serial
String recebeArduino() {
    String json;
    // Enquanto receber algo pela serial, verifica se há dados na serial
    if (Serial.available() > 0) {
        // Lê String da serial
        json = Serial.readString();

        // Espera o Buffer Serial ser esvaziado
        delay(INTERVALO_LEITURA / 2*1000);

        //Retorna para o Arduino que a serial está livre
        Serial.println("Json:" + json);
    }

    return json;
}

// Loop principal do nodeMCU
void loop() {
    // Lê dados da Serial, vindo do arduíno
    String valores = recebeArduino();

    // Se há valores recebidos
    if (valores != NULL) {
        // Chama o WebService para tratar os dados
        String ret = chamaWebservice(API_HOST, API_PORT, API_MEASURE_ROUTER, valores);
    }

    // Tempo de espera para a próxima leitura
    delay(INTERVALO_LEITURA/2*1000);
}
