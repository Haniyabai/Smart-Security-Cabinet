#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

/* ---------- OLED ---------- */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* ---------- BLUETOOTH ---------- */
SoftwareSerial bt(10, 11); // RX, TX

/* ---------- PIN DEFINITIONS ---------- */
#define SERVO_PIN   5
#define BUZZER_PIN  8
#define LASER_PIN   9
#define TRIG_PIN    7
#define ECHO_PIN    6
#define LDR_PIN     A0

/* ---------- GLOBALS ---------- */
Servo doorServo;
long duration;
int distance = 0;
bool isLocked = false;

/* ---------- SETUP ---------- */
void setup() {
  Serial.begin(9600);     // USB DEBUG ONLY
  bt.begin(9600);         // HC-05 baud rate
  delay(1000);

  Serial.println("SMART CABINET SYSTEM STARTED");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  digitalWrite(LASER_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0); // Door closed

  /* OLED INIT */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Smart Cabinet");
  display.println("System Ready");
  display.display();
}

/* ---------- LOOP ---------- */
void loop() {
  readUltrasonic();
  intrusionCheck();
  bluetoothControl();
  printBluetoothData();
  updateOLED();
  delay(500);
}

/* ---------- ULTRASONIC ---------- */
void readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration > 0) {
    distance = duration * 0.034 / 2;
  }
}

/* ---------- LDR + BUZZER ---------- */
void intrusionCheck() {
  int ldrValue = analogRead(LDR_PIN);

  if (ldrValue < 200) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

/* ---------- BLUETOOTH CONTROL ---------- */
void bluetoothControl() {
  if (bt.available()) {
    char cmd = bt.read();

    if (cmd == 'O' && !isLocked) {
      doorServo.write(90);
    }
    else if (cmd == 'C' && !isLocked) {
      doorServo.write(0);
    }
    else if (cmd == 'L') {
      isLocked = true;
      digitalWrite(BUZZER_PIN, LOW);
    }
    else if (cmd == 'U') {
      isLocked = false;
    }
    else if (cmd == 'R') {
      sendStatusToApp();
    }
  }
}

/* ---------- SEND CONTINUOUS DATA ---------- */
void printBluetoothData() {
  int ldrValue = analogRead(LDR_PIN);

  bt.print("STATE:");
  bt.print(isLocked ? "LOCKED" : "UNLOCKED");
  bt.print(";");

  bt.print("DIST:");
  bt.print(distance);
  bt.print("cm;");

  bt.print("ALERT:");
  bt.print(ldrValue < 200 ? "INTRUDER" : "SAFE");
  bt.print(";");

  bt.print("LDR:");
  bt.println(ldrValue);
}

/* ---------- SEND DATA ON REQUEST ---------- */
void sendStatusToApp() {
  printBluetoothData();
}

/* ---------- OLED UPDATE ---------- */
void updateOLED() {
  int ldrValue = analogRead(LDR_PIN);

  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("State: ");
  display.println(isLocked ? "LOCKED" : "UNLOCKED");

  display.print("Dist : ");
  display.print(distance);
  display.println(" cm");

  display.print("LDR  : ");
  display.println(ldrValue);

  display.print("Alert: ");
  display.println(ldrValue < 200 ? "INTRUDER" : "SAFE");

  display.display();
}
