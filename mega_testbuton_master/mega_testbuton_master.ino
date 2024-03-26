#include <Wire.h> 

#define SLAVE_ADDR 5
#define ANSWERSIZE 3

//ปุ่มหน้าจอ
int button_pin[4] = {24, 26, 28, 22};
int buttonState[4]; 
String final_menu ="";

//step motor
const int stepPin = 16;
const int dirPin = 15;
const int enPin = 14;

//relay advice
const int Relay_PumpSp = 12;
const int Relay_PumpWater = 11;
const int Start_button = 13;           //used to be relay_fan 

//LED
const int Relay_LED_Red = 10;
const int Relay_LED_Green=  8 ; 
const int Relay_LED_Blue = 9;
//SW
const int limitSwitchPin = 3;
const int limitmotor = 2;
//dc_motor_หมุน
const int driveDC_PWM = 7;
const int driveDC_INA = 5;
const int driveDC_INB = 4;

bool setup_complete = false;
bool door_status = false;
bool menu_complete = false;

unsigned long previousMillis = 0;
const long interval = 500;


// ---- FUNCTIONS ----// 
void MachineSetup();
String MenuBegin();
void operationWhenMenu1();
void operationWhenMenu2();
void write_i2c(int value);
String receive_i2c();
void limitSwitchInterrupt();
void when_door_opened();


void setup() {
  Wire.begin(SLAVE_ADDR);
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  pinMode(Relay_PumpSp, OUTPUT);
  pinMode(Relay_PumpWater, OUTPUT);
  pinMode(Start_button, INPUT_PULLUP);
  pinMode(Relay_LED_Green, OUTPUT);
  pinMode(Relay_LED_Red, OUTPUT);
  pinMode(Relay_LED_Blue, OUTPUT);
  pinMode(limitmotor, INPUT_PULLUP);
  pinMode(limitSwitchPin, INPUT_PULLUP);
  pinMode(button_pin[0], INPUT_PULLUP);
  pinMode(button_pin[1], INPUT_PULLUP);
  pinMode(button_pin[2], INPUT_PULLUP);
  pinMode(button_pin[3], INPUT_PULLUP);

  digitalWrite(enPin, LOW);
  digitalWrite(Relay_LED_Red, LOW);
  digitalWrite(Relay_LED_Green, LOW);
  digitalWrite(Relay_LED_Blue, HIGH);
  digitalWrite(Relay_PumpSp, LOW);
  digitalWrite(Relay_PumpWater, LOW);

  //  interrupt
  //attachInterrupt(digitalPinToInterrupt(limitSwitchPin), limitSwitchInterrupt, HIGH);
}

void loop() {
  //write_i2c(1);
  final_menu = MenuBegin();
  // select_menu = "";
  // select_menu1 = "";
   Serial.println("End Menu Begin = "+final_menu);
   delay(100);

  // for (int i = 0; i < 4; i++) {
  //   buttonState[i] = digitalRead(button_pin[i]);
  // }
    
  //   // --------------------- Print Buttons State -----------------// 
  // for (int i = 0; i < 4; i++) {
  //   Serial.print(buttonState[i]);
  //   Serial.print(" , ");
  // }
  // Serial.println("");
}


//----------------------------------------------------------------------------------------------------//
// ---------------------------------------- FUNCTION BELOW HERE --------------------------------------//

void MachineSetup(){
  digitalWrite(Relay_LED_Green, HIGH);
  digitalWrite(Relay_LED_Red, HIGH);
  Serial.println("start");
  delay(1000);
  
  //------------------ setup system -------------------//
  digitalWrite(Relay_LED_Green, LOW);
  digitalWrite(Relay_LED_Red, HIGH);
  digitalWrite(Relay_LED_Blue, LOW);
  digitalWrite(dirPin, HIGH);
  //Serial.println("door state : " + String(door_status));
  while(digitalRead(limitmotor) == HIGH && door_status == true){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    //Serial.println("door state : " + String(door_status));
  }
  digitalWrite(Relay_LED_Blue, HIGH);
  digitalWrite(Relay_LED_Red, HIGH);
  digitalWrite(Relay_LED_Green, HIGH);
  //----------------- complete setup system --------------//
  Serial.println("------->>> Setup Subsytem Complete ");

  if(door_status == true) {
    setup_complete = true; 
    Serial.println("------->>> Setup Complete <<<------------- ");
  }
  else{
    when_door_opened();
  }
}

String MenuBegin(){
  int menu = 0;
  final_menu = "";
  String select_menu = "";
  String select_menu1 = "";

  for (int i = 0; i < 4; i++) {
    buttonState[i] = digitalRead(button_pin[i]);
  }

  //-------------------------- menu 0 ----------------------------//
  while(menu == 0){
    delay(200);
    Serial.println("-------------- menu 0 ------------------");
    for (int i = 0; i < 4; i++) {
      buttonState[i] = digitalRead(button_pin[i]);
    }
    
    // --------------------- Print Buttons State -----------------// 
    for (int i = 0; i < 4; i++) {
      Serial.print(buttonState[i]);
      Serial.print(" , ");
    }
    Serial.println("");
    // ----------------------------------------------------------//
    
    if (buttonState[0] == LOW) {
      write_i2c(1);
      delay(500);

    } else if (buttonState[1] == LOW) {
      write_i2c(2);
      delay(500);
    } else if (buttonState[2] == LOW) {
      write_i2c(3);
      delay(500);
    }
    else if (buttonState[3] == LOW) {
      write_i2c(4);
      select_menu = receive_i2c();
      delay(500);
      menu = 1;
    }
  }


  // --------------------- menu 1 -----------------------------//
  while(menu == 1){
    delay(200);
    Serial.println("-------------- menu 1 ------------------");
    for (int i = 0; i < 4; i++) {
      buttonState[i] = digitalRead(button_pin[i]);
    }
    
    // --------------------- Print Buttons State -----------------// 
    for (int i = 0; i < 4; i++) {
      Serial.print(buttonState[i]);
      Serial.print(" , ");
    }
    Serial.println("");
    // ----------------------------------------------------------//
    
    if (buttonState[0] == LOW) {
      write_i2c(1);
      delay(500);

    } else if (buttonState[1] == LOW) {
      write_i2c(2);
      delay(500);
    } else if (buttonState[2] == LOW) {
      write_i2c(3);
      menu = 0;
      delay(500);
    }
    else if (buttonState[3] == LOW) {
      write_i2c(4);
      delay(500);
      menu = 2;
      buttonState[3] == HIGH;
      select_menu1 = receive_i2c();
    }
  }

  if(menu == 2){
    Serial.print("Menu 0 : "+select_menu);
    Serial.println(",  Menu 2 : "+select_menu1);
    Serial.println("Final_Menu : "+final_menu);
    menu_complete = true;
    return select_menu + select_menu1;

    if (buttonState[3] == LOW) {
      write_i2c(4);
      delay(100);
      menu = 0;
    }
  }
}


void operationWhenMenu1() {
  Serial.println("start_1");
  digitalWrite(Relay_LED_Red, HIGH);
  delay(100);
  digitalWrite(Relay_LED_Green, HIGH);
  delay(100);
  digitalWrite(Relay_LED_Blue, HIGH);
  delay(1000);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  //digitalWrite(Relay_Fan, HIGH);
  //delay(1000);
  digitalWrite(dirPin, HIGH);
  while(digitalRead(limitmotor) == HIGH && door_status == true){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  // ทำงาน
  digitalWrite(dirPin, LOW);
  for (int R = 0; R < 940; R++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  analogWrite(driveDC_PWM, 150);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  delay(1000);
  digitalWrite(Relay_PumpSp, HIGH);
  delay(10000);  // เวลาในการพ่นสเป
  digitalWrite(Relay_PumpSp, LOW);
  delay(500);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, LOW);
  delay(500);
  digitalWrite(Relay_PumpWater, LOW);
  delay(500);
  //digitalWrite(Relay_Fan, LOW);
  //delay(500);
  digitalWrite(dirPin, HIGH);
  for (int R = 0; R < 940; R++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);
}

void operationWhenMenu2() {
  Serial.println("start_2");
  digitalWrite(Relay_LED_Red, HIGH);
  delay(100);
  digitalWrite(Relay_LED_Green, HIGH);
  delay(100);
  digitalWrite(Relay_LED_Blue, HIGH);
  delay(1000);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  //digitalWrite(Relay_Fan, HIGH);
  //delay(1000);
  digitalWrite(dirPin, HIGH);
  while(digitalRead(limitmotor) == HIGH && door_status == true){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  // ทำงาน
  digitalWrite(dirPin, LOW);
  for (int R = 0; R < 1000; R++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  analogWrite(driveDC_PWM, 150);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  delay(1000);
  digitalWrite(Relay_PumpSp, HIGH);
  delay(10000);  // เวลาในการพ่นสเป
  digitalWrite(Relay_PumpSp, LOW);
  delay(500);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, LOW);
  delay(500);
  digitalWrite(Relay_PumpWater, LOW);
  delay(500);
  //digitalWrite(Relay_Fan, LOW);
  //delay(500);
  digitalWrite(dirPin, HIGH);
  for (int R = 0; R < 940; R++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  delay(1000);
}


void write_i2c(int value) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(value);
  Wire.endTransmission();
}

String receive_i2c(){
  Serial.println("Receive data");
  // Read response from Slave---------
  Wire.requestFrom(SLAVE_ADDR,ANSWERSIZE);
  
  // Add characters to string
  String text_response = "";
  byte response[ANSWERSIZE];
  while (Wire.available()) {
    for (byte i=0;i<ANSWERSIZE;i++) {
       response[i] = (byte)Wire.read();
    }
  } 
  for (byte i = 0; i < ANSWERSIZE; i++) {
    //Serial.print(char(response[i]));
    text_response += (char)response[i];
  }
  Serial.println("");
  return text_response;

}

void limitSwitchInterrupt() {
  if(menu_complete){
  door_status = false;
  setup_complete = false;
  }
}
void when_door_opened(){
  door_status = false;
  setup_complete = false;
  Serial.println("Door is Opened...");
  delay(200);
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    if( digitalRead(Relay_LED_Red)== HIGH ){
     digitalWrite(Relay_LED_Red, LOW);
    }
    else{
      digitalWrite(Relay_LED_Red, HIGH);
    }

  }
  digitalWrite(Relay_LED_Green, LOW);
  //delay(1000);
  digitalWrite(Relay_LED_Blue, HIGH);
  //delay(1000);
  digitalWrite(Relay_PumpSp, LOW);
  //delay(1000);
  digitalWrite(Relay_PumpWater, LOW);
  //delay(1000);
  digitalWrite(driveDC_INA, LOW);
  //delay(1000);
  digitalWrite(driveDC_INB, LOW);
 if(digitalRead(limitSwitchPin) == LOW) door_status = true;
}