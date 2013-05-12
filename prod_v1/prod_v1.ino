/*temperature and humidity*/
#include "DHT.h"
#include <string.h>
 #include <ctype.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

int ledPin = 13;                  // LED test pin
int rxPin = 7;                    // RX PIN 
int txPin = 8;                    // TX TX
int byteGPS=-1;
char linea[300] = "";
char comandoGPR[7] = "$GPRMC";
int cont=0;
int bien=0;
int conta=0;
int indices[13];

const int temperaturePin = 0;

int REDPin = 4;    // RED pin of the LED to PWM pin 4
int GREENPin = 5;  // GREEN pin of the LED to PWM pin 5
int BLUEPin = 6;   // BLUE pin of the LED to PWM pin 6
int brightness = 0; // LED brightness
int increment = 20;  // brightness increment
int count = 0;

int COPin = A1;
int AirPin = A2;

void setup()
{
  Serial.print("Setup");
  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  pinMode(ledPin, OUTPUT);       // Initialize LED pin
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
   
  Serial.begin(9600);
  for (int i=0;i<300;i++){       // Initialize a buffer for received data
   linea[i]=' ';
  }   
  Serial.println("DHT11 start!");
  dht.begin();
}

void loop()
{
  brightness = brightness + increment;  // increment brightness for next loop iteration

  if (brightness <= 0 || brightness >= 255)    // reverse the direction of the fading
  {
    increment = -increment;
  }
  brightness = constrain(brightness, 0, 255);
  //Serial.print(brightness);
  if (count < 4) {
    blink(GREENPin, brightness);
    count++;
  } else if (count == 5) {
    blink(GREENPin, 1);
    count++;
  } else {
    blink(BLUEPin, 70);
  }
  
  senseTemperature();
  
  Serial.print("CO measurement ");
  Serial.print(sense(COPin), DEC);
  Serial.println(";");
  Serial.print("Air quality measurement ");
  Serial.print(sense(AirPin), DEC);
  Serial.println(";");
  
  digitalWrite(ledPin, HIGH);
   byteGPS=Serial.read();         // Read a byte of the serial port
   if (byteGPS == -1) {           // See if the port is empty yet
     Serial.print("Latitude: ");
     Serial.print("37.874955 N    ");
     Serial.print("Longitude: ");
     Serial.println("-122.25837 W;");
     
   } else {
     linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
     conta++;                      
     Serial.write(byteGPS); 
     if (byteGPS==13){            // If the received byte is = to 13, end of transmission
       digitalWrite(ledPin, LOW); 
       cont=0;
       bien=0;
       for (int i=1;i<7;i++){     // Verifies if the received command starts with $GPR
         if (linea[i]==comandoGPR[i-1]){
           bien++;
         }
       }
       if(bien==6){               // If yes, continue and process the data
         for (int i=0;i<300;i++){
           if (linea[i]==','){    // check for the position of the  "," separator
             indices[cont]=i;
             cont++;
           }
           if (linea[i]=='*'){    // ... and the "*"
             indices[12]=i;
             cont++;
           }
         }
         Serial.println("");      // ... and write to the serial port
         Serial.println("");
         Serial.println("---------------");
         for (int i=0;i<12;i++){
           Serial.print("Hello" + i);
           switch(i){
             case 0 :Serial.print("Time in UTC (HhMmSs): ");break;
             case 1 :Serial.print("Status (A=OK,V=KO): ");break;
             case 2 :Serial.print("Latitude: ");break;
             case 3 :Serial.print("Direction (N/S): ");break;
             case 4 :Serial.print("Longitude: ");break;
             case 5 :Serial.print("Direction (E/W): ");break;
             case 6 :Serial.print("Velocity in knots: ");break;
             case 7 :Serial.print("Heading in degrees: ");break;
             case 8 :Serial.print("Date UTC (DdMmAa): ");break;
             case 9 :Serial.print("Magnetic degrees: ");break;
             case 10 :Serial.print("(E/W): ");break;
             case 11 :Serial.print("Mode: ");break;
             case 12 :Serial.print("Checksum: ");break;
           }
           for (int j=indices[i];j<(indices[i+1]-1);j++){
             Serial.print(linea[j+1]); 
           }
           Serial.println("");
         }
         Serial.println("---------------");
       }
       conta=0;                    // Reset the buffer
       for (int i=0;i<300;i++){    //  
         linea[i]=' ';             
       }                 
     }
   }
  
  delay(1000); //repeat once per second
}

float sense(int pin) {
  //TODO: calibrate these sensors
  return analogRead(pin);
}

void senseTemperature() {
  float voltage, degreesC, degreesF;
  voltage = analogRead(temperaturePin) * 0.00322580645;
  //voltage = analogRead(temperaturePin) * 0.00488758553;
  degreesC = (voltage * 1000.0 / 19.5) - 20.5;
  degreesF = degreesC * (9.0/5.0) + 32.0;
  
  //uncomment if debugging
  //Serial.print("voltage: ");
  //Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(degreesC);
  Serial.print("  deg F: ");
  Serial.print(degreesF);
  Serial.println(";");
}

void blink(int colorPin, int power)
{
    analogWrite(colorPin, power);
    delay(20); // wait for 20 milliseconds to see the dimming effect
}
