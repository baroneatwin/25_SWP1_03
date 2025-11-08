#include <Servo.h>

// Arduino pin assignment

#define PIN_IR    A0         // IR sensor at Pin A0
#define PIN_LED   8
#define PIN_SERVO 10

#define _DUTY_MIN 1000  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1500  // servo neutral position (90 degree)
#define _DUTY_MAX 2000  // servo full counter-clockwise position (180 degree)

#define _DIST_MIN  100.0   // minimum distance 100mm
#define _DIST_MAX  250.0   // maximum distance 250mm

#define EMA_ALPHA  0.8      // for EMA Filter

#define LOOP_INTERVAL 20   // Loop Interval (unit: msec)

Servo myservo;
unsigned long last_loop_time;   // unit: msec

float dist_prev = _DIST_MIN;
float dist_ema = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
 
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);
 
  Serial.begin(1000000);    // 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  // and turn on LED if the distance is in the range
  if(dist_raw>250 || dist_raw<100)
    digitalWrite(PIN_LED,0);
  else
  {
    digitalWrite(PIN_LED,1);
    dist_prev=dist_raw;
  }

  dist_ema = dist_prev * EMA_ALPHA + (1- EMA_ALPHA) * dist_ema;
     
  //duty = map(dist_ema, _DIST_MIN, _DIST_MAX, _DUTY_MIN, _DUTY_MAX);
  duty = _DUTY_MIN + (_DUTY_MAX-_DUTY_MIN)*((dist_ema-100)/150);     
 
  myservo.writeMicroseconds(duty);
// ---- replace your Serial.print(...) block with this ----
double dist_cm  = dist_ema * 0.1;
double servo_ratio = (duty - _DUTY_MIN) * 1.0 / (_DUTY_MAX - _DUTY_MIN);
if (servo_ratio < 0) servo_ratio = 0;
if (servo_ratio > 1) servo_ratio = 1;
double servo_cm = (_DIST_MIN * 0.1) + ((_DIST_MAX - _DIST_MIN) * 0.1) * servo_ratio;

// 항상 같은 개수의 label:value 를 공백으로 구분 + 마지막에 줄바꿈
Serial.print("IR:");        Serial.print((int)a_value);  Serial.print(' ');
Serial.print("dist_cm:");   Serial.print(dist_cm, 2);    Serial.print(' ');
Serial.print("servo_cm:");  Serial.print(servo_cm, 2);   Serial.print(' ');
Serial.print("ema_cm:");    Serial.print(dist_cm, 2);    Serial.print(' '); // 같은 값 재표시해도 OK
Serial.println();

} 
