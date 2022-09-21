#include <WiFi.h>
#include "time.h"

#include <DHT.h>


// Definimos el pin digital donde se conectan los sensores y transductores
/*S Temperatura humedad*/
#define DHTPIN        5
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

/*Ultrasonico*/
#define U_sonic_Trig  19
#define U_sonic_Echo  18

/*Configuración de la red*/
const char* ssid       = "AndroidAPCD60";
const char* password   = "luhp1657";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;   /*este parámetro se modificó para retrasar 6hr el offset de tiempo
correspondiente a mexico -6GTM    6*3600=21600sec *-1 para que se resten las horas*/
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

/*_____________Inicialización del sistema____________________*/
void setup()
{
  /*Salida de uart serial*/
  Serial.begin(115200);

  // Inicialización del sensor de humedad
  dht.begin();  
 
  /*Para Ultrasonico*/
  pinMode(U_sonic_Trig, OUTPUT); //pin como salida
  pinMode(U_sonic_Echo, INPUT);  //pin como entrada
  digitalWrite(U_sonic_Trig, LOW);//Inicializamos el pin con 0
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
 /*__________________INTERNET TIME____________________________________________*/ 
  delay(1000);
  printLocalTime();
 /*__________________END INTERNET TIME________________________________________*/ 

 /*__________________DTH11 SENSOR_____________________________________________*/ 
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();

 Serial.print("Temperatura DHT11: ");
 Serial.print(t);
 Serial.println(" ºC.");
 Serial.print("Humedad DHT11: ");
 Serial.print(h);
 Serial.println(" %."); 
 Serial.println("-----------------------");
/*__________________END DTH11 SENSOR_____________________________________________*/

/*__________________SENSOR ULTRASONICO_____________________________________________*/
  long tiempo; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(U_sonic_Trig, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(U_sonic_Trig, LOW);

  tiempo = pulseIn(U_sonic_Echo, HIGH); //obtenemos el ancho del pulso
  d = tiempo/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Ultrasonico distancia: -----> ");
  Serial.print(d);      //Enviamos serialmente el valor de la distancia
  Serial.print(" cm");
  Serial.println();
  //delay();          //Hacemos una pausa de 100ms

/*__________________END SENSOR ULTRASONICO____________________________________________*/

  
}
