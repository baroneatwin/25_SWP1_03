#define PIN_LED 7
unsigned int count, toggle;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200); // Initialize serial port
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  count = toggle = 0;
  digitalWrite(PIN_LED, toggle); // turn off LED.
}

void loop() {
  digitalWrite(PIN_LED, 0); // update LED status.
  delay(1000); // wait for 1,000 milliseconds
 
  int c=0;
  while (1){
      if(c<=4){
        digitalWrite(PIN_LED, 1);
        delay(100);
        digitalWrite(PIN_LED, 0);
        delay(100);
        c+=1;}
      digitalWrite(PIN_LED, 1);
      }
     
  }
 
  
