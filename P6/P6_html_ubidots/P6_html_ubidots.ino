/**************************************** 
 * Include Libraries 
 ****************************************/ 
#include <WiFi.h> 
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include <HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
 
/**************************************** 
 * Define Constants 
 ****************************************/ 
#define WIFISSID "Villa" //WIFI SSID aqui
#define PASSWORD "610916e99" // WIFI pwd
char payload[200]; 

// Space to store values to send 
char str_temp[10]; 
char str_hum[10]; 

float t1;
float led_value = 0;

#define PIN_SENSOR 5
DHT dht1(PIN_SENSOR, DHT11); //El sensor de temp y humedad azul. 
 
/**************************************** 
 * Main Functions 
 ****************************************/ 
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
   pinMode(18, OUTPUT);
  Serial.println();
  Serial.print("Wait for WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Sensor de temp y humedad 
  dht1.begin();

//test
  digitalWrite (18, HIGH);
  delay(3000);
  digitalWrite (18, LOW);
}

char xAuthTokenHeader[] = "BBFF-R13vxmGSVzjpZ0RlUMEt69xI29o2Ji"; // Account token (not api key)
char humedadUrl[] = "http://industrial.api.ubidots.com/api/v1.6/variables/63460157e38084000e743c58/values";
char temperaturaUrl[] = "http://industrial.api.ubidots.com/api/v1.6/variables/634601560cdeae000ee86396/values";
char secureHumedadUrl[] = "https://industrial.api.ubidots.com/api/v1.6/variables/63460157e38084000e743c58/values";
char secureTemperaturaUrl[] = "https://industrial.api.ubidots.com/api/v1.6/variables/634601560cdeae000ee86396/values";

int shouldUseHttps = 1;

void loop() {
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    getTemperatureJSON(payload);
    if (shouldUseHttps) {
      sendHttpsPostRequest(secureTemperaturaUrl, payload, "LED");
    } else {
      sendPostRequest(temperaturaUrl, payload, "LED");
    }
    
    getHumidityJSON(payload);
    // Esperamos un poco antes de mandar la siguiente petición para no exceder el límite
   // delay(1000);
    if (shouldUseHttps) {
      sendHttpsPostRequest(secureHumedadUrl, payload, "Humedad");
    } else {
      sendPostRequest(humedadUrl, payload, "Humedad");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
  //Serial.println("----- Waiting 15s -----");
  Serial.println();
  delay(1000);

    Serial.print("Temperatura: ");
   Serial.print(t1);
if(t1 <= 25.0)
{
  led_value=0;
   digitalWrite (18, LOW);
   Serial.print(" <= 25 off");
  }
if(t1>=28)
{
  led_value=1;
  digitalWrite (18, HIGH);
  Serial.print(" >= 25 off");
}
  
}

void sendPostRequest(char* url, char* payload, char* variable) {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);
  // If you need an HTTP request with a content type: application/json, use the following:
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Auth-Token", xAuthTokenHeader);
  Serial.print("Sending POST request to variable ");
  Serial.print(variable);
  Serial.print(": ");
  Serial.println(url);
  int httpResponseCode = http.POST(payload);
  Serial.print(variable);
  Serial.print(" - HTTP Response code: ");
  Serial.println(httpResponseCode);
  if (httpResponseCode > 0) {
    Serial.print("Response from Ubidots: ");
    Serial.println(http.getString());
  } else {
    Serial.print("Error response: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());  
  }
  Serial.println();
  // Free resources
  http.end();
}

char* ubidotsCertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

void sendHttpsPostRequest(char* url, char* payload, char* variable) {
  WiFiClientSecure *client = new WiFiClientSecure;
  client->setCACert(ubidotsCertificate); 
  HTTPClient https;
  if (https.begin(*client, url)) {  // HTTPS
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Auth-Token", xAuthTokenHeader);
    // start connection and send HTTP header
    Serial.print("Sending HTTPS POST request to variable ");
    Serial.print(variable);
    Serial.print(": ");
    Serial.println(url);
    int httpResponseCode = https.POST(payload);
    Serial.print(variable);
    Serial.print(" - HTTPS Response code: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode > 0) {
      Serial.print("Secure response from Ubidots: ");
      Serial.println(https.getString());
    } else {
      Serial.print("Error response: ");
      Serial.println(https.errorToString(httpResponseCode).c_str());  
    }
    Serial.println();
    https.end();
  } else {
    Serial.printf("HTTPS - Error opening secure connection\n");
  }
  delete client;
}

void getTemperatureJSON(char* dest) {
  t1 = dht1.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.println(t1); // Imprime temperatura en el serial monitor  
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(led_value, 4, 2, str_temp);
  sprintf(dest, "{\"value\": %s }", str_temp);
}

void getHumidityJSON(char* dest) {
  float h1 = dht1.readHumidity(); 
  Serial.print("Humedad: ");
  Serial.println(h1); // Imprime humedad en el serial monitor  
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(h1, 4, 2, str_hum);
  sprintf(dest, "{\"value\": %s }", str_hum);
}
