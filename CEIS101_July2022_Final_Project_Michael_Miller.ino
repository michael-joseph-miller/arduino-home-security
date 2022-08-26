#include <DHT.h> //include DHT11 library temp/humidity sensor

const byte AUTO_LIGHT = 2; // Automated light pin output
const byte DHTPIN = 3; // DHT11 temp/humidity sensor pin input
const byte DISTANCE_ECHO = 4; // HC-SRO4 Distance sensor echo pin input
const byte DISTANCE_TRIGGER = 5; // HC-SR04 Distance sensor trigger pin output
const byte DOOR_SENSOR = 6; // Door opened/closed sensor pin input
const byte BUZZER = 7; // Buzzer pin output
const byte RED_LED = 8; // Red led pin output
const byte YELLOW_LED = 9; // Yellow led pin output
const byte GREEN_LED = 10; // Green led pin output
const byte PHOTOCELL = A0; // Photocell pin input
const byte WATER_SENSOR = A1; // HW-038 water sensor signal pin input
int doorCounter = 0; // Door open counter variable
float temperature, humidity; // Temp/humidity variables
bool autoLightOn; // Automated light status variable
bool doorClosed = true, noWater = true, noIntruder = true; // Used to check for all clear status

DHT dht(DHTPIN, DHT11); // Initialize DHT sensor

void setup() {
  Serial.begin(9600);
  Serial.println("CEIS101 July 2022 Final Project");
  Serial.println("Michael J. Miller");
  
  pinMode(DISTANCE_TRIGGER, OUTPUT);
  pinMode(DISTANCE_ECHO, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(AUTO_LIGHT, OUTPUT);
  dht.begin(); // Start DHT11 temp/humidity sensor
}

void loop() {
  checkTempHumid();
  checkPhotocell();
  checkDoor();
  checkWaterLevel();
  checkDistanceSensor();
  if (doorClosed && noWater && noIntruder) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }

  delay(100); //pause program to stabilize ultrasonic sensor readings
} //end of loop()

// ========================================
// === Read Temperature Humidity Sensor ===
// ========================================
void checkTempHumid() {
  float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature(true);
  // If change in temp or humidity....
  if (newHumidity != humidity || newTemperature != temperature) {
    humidity = newHumidity;
    temperature = newTemperature;
    // ...display new temp and humidity
    Serial.println(String("Temperature:" +  String(temperature, 2) + "F") + "  Humidity:" + String(humidity, 2) + "%");
  }
}

// =======================================
// === Read Photocell, Automated Light ===
// =======================================
void checkPhotocell() {
  int photocellValue = analogRead(PHOTOCELL); // Read the value from the light sensor
  // If photocell reading is greater than 900 and automated light is OFF...
  if (photocellValue > 900 && !autoLightOn) {
    digitalWrite(AUTO_LIGHT, HIGH); // ...turn on automated light.
    autoLightOn = true; // ...set autoLightOn to true
    Serial.println("Automated light ON.");  // ...send automated light status to setrial monitor
  }
  // If photocell reading less than or equal to 900 and automated light is ON...
  if (photocellValue <= 900 && autoLightOn) {
    digitalWrite(AUTO_LIGHT, LOW); // ...turn off automated light.
    autoLightOn = false; // ...set autoLightOn to false
    Serial.println("Automated light OFF."); // ...send automated light status to setrial monitor
  }
}

// =================================
// === Door Opened/Closed Sensor ===
// =================================
void checkDoor() {
  bool doorClosed = digitalRead(DOOR_SENSOR); // Read door sensor
  // If door open...
  if (!doorClosed) {
    doorClosed = false;
    doorCounter++; // ...increment doorCounter
    // If door open for less than or equal to 10 count...
    if (doorCounter <= 10) {
      Serial.println("Warning, Door open."); // ...send warning status to serial monitor
      signalWarning(); // ...flash yellow LED
    // If door open for more than 10 count...
    } else {
      Serial.println("Alert! DoorOpen!"); // ... send alert status to serial monitor
      signalAlert(); // ...flash red LED and sound buzzer
    }
  } else {
    doorClosed = true;
  }
}

// =========================
// === Read Water Sensor ===
// =========================
void checkWaterLevel() {
  int waterValue = analogRead(WATER_SENSOR); // Read the value of the water level sensor
  // If water detected...
  if (waterValue <= 500 and waterValue > 300) {
    noWater = false;
    Serial.println("Warning, Water detected."); // ...send status to serial monitor
    signalWarning(); // ...flash yellow LED
  // If high water detected...
  } else if (waterValue > 500) {
    noWater = false;
    Serial.println("Alert! Flooding!"); // ...send status to serial monitor
    signalAlert(); // ... flash red LED and sound buzzer
  } else {
    noWater = true;
  }
}

// ================================
// === Intruder/Distance Sensor ===
// ================================
void checkDistanceSensor() {
  // Trigger distance sensor
  digitalWrite(DISTANCE_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(DISTANCE_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(DISTANCE_TRIGGER, LOW);

  // Read the echo signal
  long duration = pulseIn(DISTANCE_ECHO, HIGH); // Read duration for roundtrip distance
  long distance = (duration / 2) * 0.0135; // Convert duration to one way distance in inches

  // If object detected 6 to 12 inches from sensor...
  if (distance > 6 && distance <= 12) {
    noIntruder = false;
    Serial.println("Warning Intruder"); // ...send status to serial monitor
    signalWarning(); // ...flash yellow LED
  // If object detected within 6 inches or less...
  } else if (distance > 0 && distance <= 6) {
    noIntruder = false;
    Serial.println("Alert! Intruder!"); // ...send status to serial monitor
    signalAlert();// ...flash red LED and sound buzzer
  } else {
    noIntruder = true;
  }
}

// ======================
// === Warning signal ===
// ======================
void signalWarning() {
  // Flash yellow led for 1/2 second
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(500);
  digitalWrite(YELLOW_LED, LOW);
}

// ====================
// === Alert signal ===
// ====================
void signalAlert() {
  // Flash red led and sound buzzer every 1/4 second
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  delay(250);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
}
