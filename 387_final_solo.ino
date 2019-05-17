#include <LiquidCrystal.h>
#include <Keypad.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = A0, d5 = A1, d6 = A2, d7 = A3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//initialize the keypad


const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//timer
unsigned long delayStart = 0; // the time the delay started
bool typedLast5 = false; // true if still waiting for delay to finish(someone just typed on the keypad
unsigned long w5GoneTime = 0;
bool w5Gone = false;
bool w5WasGone = false;

//pins for talking to other Arduino, copy code for additional weights
const int w5To = 13, w5From = 10;

//value typing storage
#define Weight_Type 3

char Data[Weight_Type]; 
char Master[Weight_Type] = "05"; 
byte data_count = 0, master_count = 0;
bool Weight_is_good;
char customKey;

void setup() {
  //set up Pins for talking to Arduino, copy code for additional weights
  pinMode(w5To, OUTPUT);
  pinMode(w5From, INPUT);
  digitalWrite(w5To,LOW);

  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Enter Weight:");

  Serial.begin(9600);
}

void loop() {
  lcd.display();
  digitalWrite(w5To,LOW);
 

  customKey = customKeypad.getKey();

  if (customKey){
    Data[data_count] = customKey; 
    lcd.setCursor(data_count + 14,0); 
    lcd.print(Data[data_count]); 
    data_count++; 
    
    }
  
  if(data_count == Weight_Type-1){
    delay(2000);
    lcd.clear();
    if(!strcmp(Data, Master)){
      if(w5Gone == false){
        lcd.print("Available, clean?");
        lcd.setCursor(0,1);
        lcd.print("A: yes, Other: no");
        customKey = NULL;
        customKey = customKeypad.waitForKey();  //wait until a new input is typed

        if( customKey == 'A'){
          digitalWrite(w5To,HIGH);
          lcd.clear();
          lcd.print("Cleaning...");
          delay(1000);
          digitalWrite(w5To,LOW);
          delay(10000);
          lcd.setCursor(0,1);
          lcd.print("Done!");
          delay(2000);
        }
        lcd.clear();
        lcd.print("Enjoy your day!");
        
      }
      if(w5Gone == true){
        lcd.print("Sorry unavaiable");
        lcd.setCursor(0,1);
        lcd.print("for last ");
        lcd.print((millis()-w5GoneTime)/1000);
        lcd.print(" secs");
        delay(3000);
      }
    }
      
    else{
      lcd.print("Put in a valid");
      lcd.setCursor(0,1);
      lcd.print("weight ");      
      delay(1000);
     }
     lcd.clear();
     lcd.print("Enter Weight:");
     clearData();
  }

    if(digitalRead(w5From) == LOW){
    w5WasGone = w5Gone;
    w5Gone = true;
    if(w5WasGone == false){
      w5GoneTime = millis();
    }
    lcd.setCursor(0,1);
    lcd.print(digitalRead(w5From));
  }

    if(digitalRead(w5From) == HIGH){
    w5Gone = false;
   }



    
}

void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}
