/*Final release code
 * Subject: IoT 
 * 
 * Author: Tsipini
 * Date: Oct 16,2022 
 * Modified: Dic 1,2022
 * 
 * Version: 7.0
 * 
 * Notes:
*All gut, not good, gut!
 */

 
 
#include "ThingSpeak.h"
#include "WiFi.h"
#include <ArduinoJson.h>    //JSON
WiFiServer server(80);      //Wifi Server



//ThingSpeak retrive information
char   host[]      = "api.thingspeak.com";  // ThingSpeak address
String APIkey      = "1934178";             // Thingspeak Read Key, works only if a PUBLIC viewable channel
String APIreadkey  = "YWOJUI2OQGO78WME";    // Thingspeak Read Key, works only if a PUBLIC viewable channel
const int httpPort = 80;



//Conexión a Internet


const char* ssid = "RED"; //Nombre de su internet                           <<<<<<------WI-FI-----------------------+O
const char* password = "PSW"; //Contraseña de su internet


//Wifi client to retrieve from thingSpeak
WiFiClient client;
const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server


unsigned long myChannelNumber = 4;
const char * myWriteAPIKey = "YWOJUI2OQGO78WME";

//Información para poder subir datos a ThingSpeak
unsigned long R_channelID = 1916671; //ID del canal de UDS
const char* ReadAPIKey = "EADNR0T3DL06KNSR"; //READ API Key del canal que creaste

unsigned long channelID = 1916671; //ID del canal de UDS
const char* WriteAPIKey = "HXXCY391D2TWZCYY"; //Write API Key del canal que creaste
WiFiClient cliente; //Objeto Cliente del WiFi

//ThingSpeak retrieve, define this as PUBLIC or PRIVATE to access the channels as required by their security setting
#define PUBLIC  

/*PIN Definitions flowmeter*/

#define SENSOR        2

/*#define PIN_FLOWSENSOR  2 // Could be any available*/
#define RELE_1          5

/*Definitions for button*/
//#define BUTTON         21
int manual_flag = 0;


/*Definitions for LM35*/
#define ADC_VREF_mV    3300.0   // in millivolt
#define ADC_RESOLUTION 4096.0   // ADC resolution
#define pinLM35        32       // ESP32 pin GIOP36 (ADC0) connected to LM35                             <<<<<<---------PIN-LM35-------------------+O

int TempSet = 30;

int remote_temp = 0;

int TEMP = 0;
float tempC = 0;
int adcVal = 0;
float milliVolt = 0;


/*Definitions for Rele's*/
const int RelePin = RELE_1;
int valvestatus = 0;

/*Definitions for flow meter*/

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
/*
void manual_op()
{
  manual_flag = 0;
}
*/




void setup() {
  /*Serial monitor setup*/
 Serial.begin(115200);

pinMode(21, INPUT);


/*Rele's*/
pinMode (RelePin, OUTPUT);
digitalWrite (RelePin, HIGH); //safety value -> Válvula cerrada

/*Flow sensor*/
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
 attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

//Conexión a internet
Serial.println("INICIANDO...");
WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
 Serial.println("Wifi conectado!");

WiFi.mode(WIFI_STA);

 ThingSpeak.begin(cliente); //Inicializar el servidor de ThingSpeak


Serial.println(" SISTEMA EN LINEA ");



 /*Interrupt for push button*/
// attachInterrupt(digitalPinToInterrupt(BUTTON), manual_op, FALLING);
// manual_flag = 0;
}




void loop() {

 //Actualizamos temperatura
  /*  LM35_code  */
    adcVal = analogRead(pinLM35);                             // Read the ADC value from the temperature sensor
    milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);    // Convert the ADC value to voltage in millivolt
    tempC = (milliVolt / 10) + 8;                                 // Convert the voltage to the temperature in °C
    // print the temperature in the Serial Monitor:
   Serial.print("Temperatura del agua: ");
    Serial.print(tempC);   // print the temperature in °C
    Serial.print("°C \n");
    delay(100);
  /*  End_LM35  */

if (tempC>=TempSet)
  {
       Serial.print(" El agua está a la temperatura deseada. Valvula: CERRADA ");
  }
   /*Flow sensor*/

   currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print(" Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    

        Serial.println("\n");
        Serial.println("\n");


  /*End flow sensor*/
  }
delay(10);

while(flowRate >= 0.5)
{
  /*  LM35_code  */
    adcVal = analogRead(pinLM35);                             // Read the ADC value from the temperature sensor
    milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);    // Convert the ADC value to voltage in millivolt
    tempC = (milliVolt / 10) + 8;                                 // Convert the voltage to the temperature in °C
    // print the temperature in the Serial Monitor:
    Serial.print("Temperatura del agua: ");
    Serial.print(tempC);   // print the temperature in °C
    Serial.print("°C \n");
  /*  End_LM35  */
delay(10);
  /*Comtrol*/
  if (tempC<=TempSet)
  {
       digitalWrite (RelePin, LOW);
       Serial.print(" Valvula: ABIERTA");
       Serial.print(" \n");
       valvestatus = 1;

 // Se ponen los campos definidos en el canal ThingSpeak con las variables de temperatura y humedad
  
  ThingSpeak.setField (1,flowRate); //En el campo 1 se le asigna caudal
  // delay(1000);
  ThingSpeak.setField (2,tempC); //En el campo 2 se le asigna la temperatura
  // delay(1000);
  ThingSpeak.setField (3,valvestatus); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  ThingSpeak.setField (4,remote_temp); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  
  //Mandar los datos a ThingSpeak
  ThingSpeak.writeFields(channelID,WriteAPIKey);


       
  }
delay(10);
  if (tempC>=TempSet)
  {
      digitalWrite (RelePin, HIGH);
      Serial.print(" El agua está a la temperatura deseada. Valvula: CERRADA ");
      Serial.print(" \n");
      valvestatus = 0;
  }
delay(10);

 //Actualizamos la variable de flujo
   /*Flow sensor*/

   currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print(" Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    

        Serial.println("\n");
        Serial.println("\n");


    /*End flow sensor*/

    /*End of control*/
  }
}
  digitalWrite (RelePin, HIGH);
  valvestatus = 0;



        if(valvestatus == 1){
              Serial.print(" Valvula: ABIERTA");
              Serial.print(" \n");
        } 
        if(valvestatus == 0){
                Serial.print(" Valvula: CERRADA");
              Serial.print(" \n");
        } 




  if(0 == digitalRead(21))        //Condicional del boton
  {
    int counter = 0;
    int safety_timmer = 10;    //segundos de apagado automático

    while(TempSet >= tempC)
    {
    adcVal = analogRead(pinLM35);                             // Read the ADC value from the temperature sensor
    milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);    // Convert the ADC value to voltage in millivolt
    tempC = (milliVolt / 10) + 8;                                 // Convert the voltage to the temperature in °C
    Serial.print("Temperatura del agua: ");
    Serial.print(tempC);   // print the temperature in °C
    Serial.print("°C \n");
    
      Serial.print(" PURGA MANUAL ");
      digitalWrite (RelePin, LOW);
      Serial.print(" Valvula: ABIERTA");
      Serial.print(" \n");
      
      delay(1000);
      counter++;
      Serial.print(counter);

            if(safety_timmer == counter)
            {
              tempC = TempSet+1;
              counter = 0;
            }

    }

 // Se ponen los campos definidos en el canal ThingSpeak con las variables de temperatura y humedad
  
  ThingSpeak.setField (1,flowRate); //En el campo 1 se le asigna caudal
  // delay(1000);
  ThingSpeak.setField (2,tempC); //En el campo 2 se le asigna la temperatura
  // delay(1000);
  ThingSpeak.setField (3,valvestatus); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  ThingSpeak.setField (4,remote_temp); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  
  //Mandar los datos a ThingSpeak
  ThingSpeak.writeFields(channelID,WriteAPIKey);                        

    
  }



        
                                        ///////////////////// VARIABLES ENVÍO DE DATOS \\\\\\\\\\\\\\\\\\\\\\\\



  // Se ponen los campos definidos en el canal ThingSpeak con las variables de temperatura y humedad
  
  ThingSpeak.setField (1,flowRate); //En el campo 1 se le asigna caudal
  // delay(1000);
  ThingSpeak.setField (2,tempC); //En el campo 2 se le asigna la temperatura
  // delay(1000);
  ThingSpeak.setField (3,valvestatus); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  ThingSpeak.setField (4,remote_temp); //En el campo 2 se le asigna status de valvula
  // delay(1000);
  
  //Mandar los datos a ThingSpeak
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("Información actualizada");
  Serial.println("\n");
  



  

  //Retreive data from ThingSpeak

    RetrieveTSChannelData();
    delay(1000); //Wait before we request again

 Serial.println("Temperatura deseada: ");
  Serial.print(remote_temp);
  Serial.println("\n");
  delay(1000);




}
 ////////////////////////////////////*Loop END*///////////////////////////////////////


void RetrieveTSChannelData() {  // Receive data from Thingspeak
  static char responseBuffer[3*1024]; // Buffer for received data
  client = server.available(); 
  if (!client.connect(host, httpPort)) { 
    Serial.println("connection failed"); 
    return; 
  } 
  String url = "/channels/" + APIkey; // Start building API request string
  //GET /channels/CHANNEL_ID/feeds.json?api_key=<your API key>&results=2
  #ifdef PRIVATE 
  url += "/fields/1.json?results=5";  // 5 is the results request number, so 5 are returned, 1 woudl return the last result received
  #else
  url += "/fields/1.json?api_key="+APIreadkey+"&results=5";  // 5 is the results request number, so 5 are returned, 1 woudl return the last result received
  #endif
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
 // Serial.println("\n\r"+String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  while (!skipResponseHeaders());                      // Wait until there is some data and skip headers
  while (client.available()) {                         // Now receive the data
    String line = client.readStringUntil('\n');
    if ( line.indexOf('{',0) >= 0 ) {                  // Ignore data that is not likely to be JSON formatted, so must contain a '{'
   //   Serial.println(line);                            // Show the text received
      line.toCharArray(responseBuffer, line.length()); // Convert to char array for the JSON decoder
      decodeJSON(responseBuffer);                      // Decode the JSON text
    }
  }
  client.stop();
}

bool skipResponseHeaders() { 
  char endOfHeaders[] = "\r\n\r\n"; // HTTP headers end with an empty line 
  client.setTimeout(HTTP_TIMEOUT); 
  bool ok = client.find(endOfHeaders); 
  if (!ok) { Serial.println("No response or invalid response!"); } 
  return ok; 
} 

bool decodeJSON(char *json) {
  StaticJsonBuffer <3*1024> jsonBuffer;
  char *jsonstart = strchr(json, '{'); // Skip characters until first '{' found and ignore length, if present
  if (jsonstart == NULL) {
 //   Serial.println("JSON data missing");
    return false;
  }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json); // Parse JSON
  if (!root.success()) {
 //   Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }
  JsonObject& root_data = root["channel"]; // Begins and ends within first set of { }
  String id   = root_data["id"];
  String name = root_data["name"];
  String field1_name = root_data["field1"];
  String datetime    = root_data["updated_at"];
 // Serial.println("\n\n Channel id: "+id+" Name: "+ name);
 // Serial.println(" Readings last updated at: "+datetime);
  
  for (int result = 0; result < 5; result++){
    JsonObject& channel = root["feeds"][result]; // Now we can read 'feeds' values and so-on
    String entry_id     = channel["entry_id"];
    String field1value  = channel["field1"];
  //  Serial.print(" Field1 entry number ["+entry_id+"] had a value of: ");
  //  Serial.println(field1value);
    remote_temp = channel["field1"];
    
  }
}
  //Thing speak response to GET request(headers removed) and /result=1:
  //{"channel":{"id":320098,"name":"Test Channel","latitude":"0.0","longitude":"0.0","field1":"Pressure","field2":"Temperature","field3":"Humidity",
  //"created_at":"2017-08-21T13:22:12Z","updated_at":"2017-08-26T22:18:16Z","last_entry_id":85},
  // Second level [0] begins with array pointer [0] and then { } so we need to move down a level
  //"feeds":[{"created_at":"2017-08-26T22:18:16Z","entry_id":85,"field1":"40"}]}
