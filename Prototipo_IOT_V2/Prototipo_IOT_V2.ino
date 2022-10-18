/*Code for 1'st prototype
 * Subject: IoT 
 * 
 * Author: t$1p¡ni
 * Date: Oct 16,2022 
 * 
 * Version: 2.0
 * 
 * Notes:
 * All sensor reading's print on terminal
 * The control algorithm need's minor fixes
 */


/*PIN Definitions*/
#define PIN_LM35      15 // ESP32 pin GIOP36 (ADC0) connected to LM35
#define SENSOR        2

/*#define PIN_FLOWSENSOR  2 // Could be any available*/
#define RELE_1          5

/*Definitions for LM35*/
#define ADC_VREF_mV    3300.0 // in millivolt   
#define ADC_RESOLUTION 4096.0
int adcVal = 0;
float milliVolt = 0;
float tempC = 0;

/*Definitions for Rele's*/
const int RelePin = RELE_1;

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

void setup() {
/*Serial monitor setup*/
 Serial.begin(9600);
digitalWrite (RelePin, LOW);
delay(1000);

digitalWrite (RelePin, HIGH);
/*Flow sensor*/
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
    attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

    
/*Rele's*/
  pinMode (RelePin, OUTPUT);
}

void loop() {

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
    Serial.println("L");

/*End flow sensor*/

/*  LM35  */
 
  adcVal = analogRead(PIN_LM35);                              /*read the ADC value from the temperature sensor*/
  milliVolt = (adcVal/ADC_RESOLUTION) * ADC_VREF_mV;  /*convert the ADC value to voltage in millivolt*/
  tempC = milliVolt / 10;                                /*convert the voltage to the temperature in °C*/

 /*print the °C temperature in the Serial Monitor:*/
  Serial.print("Temperature: ");
  Serial.print(tempC);   
  Serial.print("°C");


/*  End LM35  */

/*Comtrol*/

if (tempC>=30)
{
   digitalWrite (RelePin, LOW);
   Serial.print(" Valve ON");
}
else
{
  digitalWrite (RelePin, HIGH);
  Serial.print(" Valve OFF");
}
/*End of control*/



} 

} /*Loop END*/
