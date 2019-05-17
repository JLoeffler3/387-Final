#include <HX711.h>

#define calibration_factor 9450.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT  3
#define CLK  2

HX711 scale;

float dumbbell_weight = 5.0;
float current_weight;
float prev_weight = 0.0;

//timer
unsigned long delayStart = 0; // the time the delay started
bool delayRunning = false; // true if still waiting for delay to finish

//fan
int gate = 10;
int pushCounter = 0;
int lastButtonState =0;
int buttonState = 0;

//Servo motors for spray bottle
#include <Servo.h>  // servo library
const int serv1 = 8, serv2 = 9;
Servo servo1;
Servo servo2;


// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool showweight= false;

//pins for talking to control Arduino
const int wTo = 8, wFrom = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Dumbell Weight Testing");

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");

  Serial.print("Reading: ");
  current_weight = scale.get_units(); //scale.get_units() returns a float
  Serial.print(current_weight, 1);   
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();

  pinMode(gate, OUTPUT);
  digitalWrite(gate,LOW);
  delayRunning = false; // not started yet

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.display();

  //Servos
  servo1.attach(serv1);
  servo2.attach(serv2);
  servo1.write(90);
  servo2.write(0);

  pinMode(wTo, OUTPUT);
  pinMode(wFrom, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //measure current weight on rack
  Serial.print("Reading: ");
  prev_weight = current_weight;
  current_weight = scale.get_units(); //scale.get_units() returns a float
  
  Serial.print(current_weight, 1);   
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();
  // if weight has just been put back onto the rack (current wight just changed over to == dumbell weight)
  if(abs(current_weight - prev_weight) >= 0.3 && current_weight >= dumbbell_weight - 0.5 && current_weight <= dumbbell_weight + 0.5){
      clean();
      lcd.clear();
      lcd.print(current_weight); lcd.print(" lbs");
      showweight=true;
      digitalWrite(wTo,HIGH);
  }
  

  // check if delay has timed out after 10sec == 10000mS
  if (delayRunning && ((millis() - delayStart) >= 10000)) {
    delayRunning = false; // // prevent this code being run more then once
    digitalWrite(gate, LOW); // turn fan off
    Serial.println("Turned Fan Off");
    showweight=false;
  }
  // check if weight is taken off
  if (abs(current_weight - prev_weight) >= 0.3 && current_weight < dumbbell_weight - 0.5 || current_weight > dumbbell_weight + 0.5) {
    delayRunning = false; // // prevent this code being run more then once
    digitalWrite(gate, LOW); // turn fan off
    Serial.println("Turned Fan Off");
    showweight=false;
    digitalWrite(wTo,LOW);
  }

  if(showweight==false  && current_weight < dumbbell_weight - 1.5 || current_weight > dumbbell_weight + 1.5){
    lcd.clear();
    lcd.print("This rack is for");
    lcd.setCursor(0,1);
    lcd.print(dumbbell_weight);
    lcd.print(" lb weights.");
    lcd.setCursor(0,0);
  }

  //if(digitalRead(wFrom) == HIGH){
   // clean();
    //Serial.println(digitalRead(wFrom));
 // }
  //lcd.setCursor(0,1);
  //lcd.print(digitalRead(wFrom));
}

void clean(){
  spray();
  fan();
}

void spray(){
  //move motor to make bottle spray
  servo1.write(180);
  servo2.write(169);
  
  delay(500);
  
  //move motor back to starting position
  servo1.write(90);
  servo2.write(-169);
}

void fan(){
  //turn on fans
  Serial.println("on");
  digitalWrite(gate,HIGH);
  delayStart = millis();   // start delay
  delayRunning = true; // not finished yet
}
