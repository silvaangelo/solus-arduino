#include "ESP8266WiFi.h"

#define TEMPO_LEITURA 30 // Definição do tempo de leitura do dados, em segundos

const char* ssid     = "wifiangelo";
const char* password = "123aa4a5";

const char* HOST  = "206.189.181.208";
const int   PORTA = 3000;
const char* API   =  "/api/measure";

int wifiStatus;

String chamaWebservice(String host, int porta, String api, String postData) {
    String retorno;

    if(WiFi.status() == WL_CONNECTED) {
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

String recebeArduino() {
    String json;

    // Verifica se há dados na serial
    if (Serial.available() > 0) {
        // Lê String da serial
        json = Serial.readString();

        // Espera o Buffer Serial ser esvaziado
        delay(TEMPO_LEITURA / 2*1000);

        //Retorna para o Arduino que a serial está livre
        Serial.println("Json:" + json);
    }

    return json;
}

void setup() {
    Serial.begin(115200);

    delay(5000);

    // Iniciar ligação à rede
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Wifi connected.\n");

    // Espera o arduino subir e começar a ler.
    delay(10000);
}

void loop() {
    // Lê dados da Serial, vindo do arduíno
    String valores = recebeArduino(); 

    // Se há valores recebidos
    if (valores != NULL ) {

    // Chama o WebService para tratar os dados
    String ret = chamaWebservice(HOST, PORTA, API, valores);

    }

    // Tempo de espera para a próxima leitura
    delay(TEMPO_LEITURA / 2*1000);
}
