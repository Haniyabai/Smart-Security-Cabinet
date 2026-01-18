#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo doorServo;

// PIN DEFINITIONS
#define SERVO_PIN 5
#define TRIG_PIN 2
#define ECHO_PIN 3
#define BUZZER_PIN 4
#define LASER_PIN 9
#define LDR_PIN A0

// VARIABLES
long duration;
int distance;
String btData = "";
bool doorLocked = false;

void setup() {
  Serial.begin(9600);

  doorServo.attach(SERVO_PIN);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  digitalWrite(LASER_PIN, HIGH); // turn laser ON

  // OLED INIT
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Cabinet System Ready!");
  display.display();

  doorServo.write(0); // INITIAL CLOSED POSITION
}

// ULTRASONIC FUNCTION
int getDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

// HANDLE BLUETOOTH COMMANDS
void handleBluetooth() {
  if (Serial.available()) {
    btData = Serial.readStringUntil('\n');

    if (btData == "OPEN") {
      doorServo.write(90);
      doorLocked = false;
      Serial.println("OPENED");
    }
    if (btData == "CLOSE") {
      doorServo.write(0);
      doorLocked = false;
      Serial.println("CLOSED");
    }
    if (btData == "LOCK") {
      doorServo.write(0);
      doorLocked = true;
      Serial.println("LOCKED");
    }
    if (btData == "UNLOCK") {
      doorLocked = false;
      Serial.println("UNLOCKED");
    }
  }
}

// OLED DISPLAY FUNCTION
void updateOLED(String cabinet, String items, String alertMsg) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Cabinet: "); display.println(cabinet);
  display.print("Items: "); display.println(items);
  display.print("Alert: "); display.println(alertMsg);
  display.display();
}

void loop() {
  handleBluetooth();

  // DISTANCE CHECK
  distance = getDistance();
  bool personDetected = (distance < 20);

  // LDR / LASER CHECK
  int ldrValue = analogRead(LDR_PIN);
  bool laserCut = (ldrValue < 200); // adjust depending on light

  String cabinetStatus = doorLocked ? "LOCKED" : "ACTIVE";
  String itemStatus = laserCut ? "Laser Broken" : "OK";
  String alertStatus = "None";

  if (personDetected) {
    digitalWrite(BUZZER_PIN, HIGH);
    alertStatus = "Person Detected!";
  } else if (laserCut) {
    digitalWrite(BUZZER_PIN, HIGH);
    alertStatus = "Laser Beam Broken!";
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  updateOLED(cabinetStatus, itemStatus, alertStatus);

  delay(200);
}

