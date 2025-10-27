#include <Servo.h>

Servo myservo;

// Arduino pin assignment
#define PIN_LED   9    // LED active-low
#define PIN_TRIG  12   // sonar sensor TRIGGER
#define PIN_ECHO  13   // sonar sensor ECHO
#define PIN_SERVO 10   // servo motor

// Configurable parameters for sonar
#define SND_VEL        346.0     // sound velocity at 24°C (m/s)
#define INTERVAL       25        // sampling interval (ms)
#define PULSE_DURATION 10        // ultrasonic pulse duration (µs)
#define DIST_MIN       180.0     // minimum measurable distance (mm)
#define DIST_MAX       360.0     // maximum measurable distance (mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0)  // max echo wait time (µs)
#define SCALE   (0.001 * 0.5 * SND_VEL)    // µs → mm conversion factor

#define EMA_ALPHA 0.8   // EMA filter weight (0~1)

// Target distance range (mm)
#define TARGET_LOW   250.0
#define TARGET_HIGH  290.0

// Servo duty durations (µs)
#define DUTY_MIN  600
#define DUTY_NEU 1475
#define DUTY_MAX 2350

// Global variables
float  distance_ema, distance_prev = DIST_MAX;
unsigned long lastSampleTime; // ms

void setup() {
  // Initialize GPIO
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH); // LED OFF initially (active-low)

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(DUTY_MIN);

  // Initialize variables
  distance_prev = DIST_MIN;
  distance_ema  = distance_prev;

  Serial.begin(57600);
}

void loop() {
  float distance_raw, distance_filtered;

  // Maintain sampling interval
  if (millis() < lastSampleTime + INTERVAL)
    return;

  // Measure distance
  distance_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // Range filter
  if ((distance_raw == 0.0) || (distance_raw >= DIST_MAX)) {
    myservo.writeMicroseconds(DUTY_MAX);
    distance_filtered = distance_prev;
  } 
  else if (distance_raw <= DIST_MIN) {
    myservo.writeMicroseconds(DUTY_MIN);
  } 
  else { // In desired range
    distance_filtered = distance_raw;
    distance_prev = distance_raw;
    myservo.writeMicroseconds(DUTY_MIN + (DUTY_MAX - DUTY_MIN) * ((distance_raw - 180.0) / 180.0));
  }

  // EMA filter
  distance_ema = EMA_ALPHA * distance_prev + (1 - EMA_ALPHA) * distance_ema;

  // 🔹 LED control (only ON when 180~360 mm)
  if (distance_ema >= 180.0 && distance_ema <= 360.0) {
    digitalWrite(PIN_LED, LOW);   // LED ON (active-low)
  } else {
    digitalWrite(PIN_LED, HIGH);  // LED OFF
  }

  // Serial output
  Serial.print("Min:");    Serial.print(DIST_MIN);
  Serial.print(", Raw:");  Serial.print(distance_raw);
  Serial.print(", EMA:");  Serial.print(distance_ema);
  Serial.print(", Servo:");Serial.print(myservo.read());
  Serial.print(", Max:");  Serial.print(DIST_MAX);
  Serial.println("");

  // Update time
  lastSampleTime += INTERVAL;
}

// Measure distance from ultrasonic sensor (mm)
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}

 
  
