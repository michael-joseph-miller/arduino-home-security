#include "DHT.h" //include DHT 11 library

const int DHTPIN 6 // Digital pin connected to the DHT sensor
const word DHTTYPE DHT11 // Define type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
const int  trigPin 8 
const int echoPin 7 
const int  Rled 11 
const int  Yled 12
const int  Gled 13 
#define buzzer 10  
#define photocell A0
#define autoLight 5 
#define waterSignal A1
#define doorSensor 9
 
void setup() { 
Serial.begin(9600); // Sets the baud rate for logging 
Serial.println("CEIS101 Course Project Challenge");
Serial.println("Name: xxxxx "); //replace xxxxx with your name

pinMode(trigPin, OUTPUT); 
pinMode(echoPin, INPUT); 
pinMode(Rled, OUTPUT); 
pinMode(Yled, OUTPUT); 
pinMode(Gled, OUTPUT); 
pinMode(buzzer, OUTPUT); 
pinMode(autoLight, OUTPUT); 
dht.begin();
} 
 
void loop() { 
//=== Temperature Humidity Sensor ===
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature in Fahrenheit: "));
  Serial.println(temperature);
  
// === Water Sensor ===
int waterValue=analogRead(waterSignal); // Read the value of the photocell

if (waterValue > 500){ // Alert condition
analogWrite(Rled, 255); // 100% duty cycle
Serial.println("Alert! Water Detected!"); 
}
else if(waterValue <= 500 and waterValue > 300){ // Warning condition
analogWrite(Yled, 64);// 25% duty cycle
Serial.println("Warning! Water Detected!"); 
}
else{ // No water condition
analogWrite(Gled, 0);// 0% duty cycle
}

//=== Automated Light === 
int value=analogRead(photocell); // Read the value from the light sensor to determine condition  

if (value > 450) {  
 digitalWrite(autoLight, HIGH); 
 Serial.println("The automated light is ON");
} 
else { 
digitalWrite(autoLight, LOW); 
}  

//==== Distance Sensor === 
long duration, distance, inches;

digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Read the echo signal
duration = pulseIn(echoPin, HIGH); // Read duration for roundtrip distance
distance = (duration /2) * 0.0135 ; // Convert duration to one way distance in units of inches

if (distance <= 12) {  // Outer IF statement units of inches
  if (distance <=6){   // Alert range condition
    Serial.println("Alert! Possible Intruder."); 
    digitalWrite(Rled, HIGH); // Alert green LED on
    digitalWrite(Yled, LOW);
    digitalWrite(Gled, LOW);
    } 
  if (distance <12 and distance > 6){  // Warning range condition
  digitalWrite(Rled, LOW);
  digitalWrite(Yled, HIGH);  // Warning yellow LED on
  digitalWrite(Gled, LOW);
  }  

//==================== Beeping Rate Code Start ======
digitalWrite(buzzer,HIGH);
for (int i= distance; i>0; i--)
delay(10);

digitalWrite(buzzer,LOW);
for (int i= distance; i>0; i--)
delay(10);
//==================== Beeping Rate Code End =======
}
 else{ //Safe range condition 
  digitalWrite(Rled, LOW);
  digitalWrite(Yled, LOW);
  digitalWrite(Gled, HIGH);  // Safe distance green LED on
  digitalWrite(buzzer, LOW);
}// end of outer IF statement

delay(100); //pause program to stabilize ultrasonic sensor readings

} //end of loop
