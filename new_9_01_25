#include <Wire.h> 
#include <Servo.h>

Servo servoMotor;  // Create a servo object to control a servo motor

#define SLAVE_ADDR 5
#define ANSWERSIZE 3

//ปุ่มหน้าจอ 
int button_pin[4] = {22, 26, 28, 24};
int button_enter = 20;
int buttonState;
                 
//ปุ่มแยก
int button_mode = 30;

//step motor
const int stepPin = 16;
const int dirPin = 15;
const int enPin = 14;

//relay advice
const int Relay_PumpSp = 12;
const int Relay_PumpWater = 40;
const int Start_button = 13;           //used to be relay_fan 

//LED
const int Relay_LED_Red = 8;
const int Relay_LED_Green=  9 ; 
const int Relay_LED_Blue = 10;
//SW
const int limitSwitchPin = 3;
const int limitmotor = 2;

//dc_motor_หมุน
const int driveDC_PWM = 7;
const int driveDC_INA = 5;
const int driveDC_INB = 4;

const int encoderPin = 42;
int servoPin = 50; 
 
int position = 0;
int startposition = 90;

unsigned long previousMillis = 0;
const long interval = 500;

// Encoder variables
volatile int encoderCount = 1;
bool lastEncoderState = HIGH;

const int PULSES_PER_90_DEGREES = 3; 
const int PULSES_PER_90_DEGREES_NEW = 4;
const int PULSES_PER_90_DEGREES_for_small = 4;


///menu select
bool condition_selected = false;
bool side_selected = false;
bool option_selected = false;


int menu_level = 0;
int current_condition = 1;
int current_side = 1;
int current_option = 1;
const int max_condition = 2;
const int max_side = 2;
const int max_option = 3;



void setup() {
  Wire.begin(SLAVE_ADDR);
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  pinMode(Relay_PumpSp, OUTPUT);
  pinMode(Relay_LED_Green, OUTPUT);
  pinMode(Relay_LED_Red, OUTPUT);
  pinMode(Relay_PumpWater, OUTPUT);
  pinMode(Relay_LED_Blue, OUTPUT);
  pinMode(limitmotor, INPUT_PULLUP);
  pinMode(limitSwitchPin, INPUT_PULLUP);
  pinMode(button_pin[0], INPUT_PULLUP);
  pinMode(button_pin[1], INPUT_PULLUP);
  pinMode(button_pin[2], INPUT_PULLUP);
  pinMode(button_pin[3], INPUT_PULLUP);
  pinMode(button_mode, INPUT_PULLUP);
  pinMode(encoderPin, INPUT);

  digitalWrite(enPin, LOW);
  digitalWrite(Relay_LED_Red, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  digitalWrite(Relay_LED_Blue, HIGH);
  digitalWrite(Relay_PumpSp, HIGH);
  digitalWrite(Relay_PumpWater, LOW);

  servoMotor.attach(servoPin);
  servoMotor.write(90);
  spin_90_deg();
  Serial.println("finish_spin");
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  digitalWrite(dirPin, HIGH);
  for (int R = 0; R < 2000; R++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }

  // -------------------------------  SETUP MANUAL  ---------------------------------- //
  write_i2c(7);
  while(digitalRead(button_pin[0]) != LOW){           // รอคำสั่งจากปุ่ม 1
    Serial.println("-------press for setup--------");
    write_i2c(7);
  }
  //  เปิด pump spray
  servoMotor.write(90);// อยู่ตำแหน่งปิด 
  delay(100);
  digitalWrite(Relay_PumpSp, LOW);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(500);
  write_i2c(6);

  while(digitalRead(button_pin[1]) != LOW){           // รอคำสั่งจากปุ่ม 2
    Serial.println("-------press for done--------");
  }
  //  ปิด pump spray
  digitalWrite(Relay_PumpSp, HIGH);
  delay(500);
  write_i2c(8);

  // ------------------------------------------------------------------------------- //

  Serial.println(">>>>>>>>>  setup done");
  digitalWrite(Relay_LED_Red, HIGH);

}

void loop() {
  // write_i2c(8);
  buttonState = digitalRead(button_pin[2]);  // อ่านสถานะปุ่มปัจจุบัน

  servoMotor.write(startposition);// อยู่ตำแหน่งปิด 
  delay(1000);

  digitalWrite(Relay_PumpWater, LOW);
  Serial.println("Start------------");

  // ------------------ STAGE 1 --------------------//
  // --   setup stepper motor to start position   --//
  digitalWrite(Relay_LED_Green, LOW);
  //write_i2c(9); // display working
  // write_i2c(5);

  condition_selected = false;
  side_selected = false;
  option_selected = false;
  write_i2c(4);

  start_menu:
    menu_level = 0;
    current_condition = 1;
    current_side = 1;
    current_option = 1;

  // --- SELECT CONDITION ---
  delay(500);
  write_i2c(4);
  delay(500);
  digitalWrite(Relay_LED_Green, LOW);

  Serial.print("Select CONDITION: "); Serial.println(current_condition);
  while (true) {
    if (digitalRead(button_pin[0]) == LOW) { // up
      current_condition = (current_condition % max_condition) + 1;
      write_i2c(1);
      Serial.print("Condition: "); Serial.println(current_condition);
    } else if (digitalRead(button_pin[1]) == LOW) { // down
      current_condition = (current_condition - 2 + max_condition) % max_condition + 1;
      write_i2c(2);
      Serial.print("Condition: "); Serial.println(current_condition);
    } else if (digitalRead(button_pin[2]) == LOW) { // back on level 0: clear
      while(digitalRead(button_pin[2]) != HIGH){
        Serial.println("release button 2 ...");
      }
      Serial.println("clearrrrrrrr");
      write_i2c(5);
      Serial.println("yellow pressed.");
      digitalWrite(Relay_LED_Green, HIGH);
      digitalWrite(Relay_LED_Red, LOW);
      servoMotor.write(0);// อยู่ตำแหน่งปิด 
      delay(1500);
      digitalWrite(Relay_PumpWater, HIGH);
      digitalWrite(Relay_PumpSp, LOW);
      delay(500);
      while(digitalRead(button_pin[2]) != LOW){
        Serial.println("Waiting for stop yellow.");
      }
      digitalWrite(Relay_PumpSp, HIGH);
      delay(500);
      write_i2c(4);
      delay(500);
      digitalWrite(Relay_LED_Green, LOW); // low is led on
      digitalWrite(Relay_LED_Red, HIGH);
      goto start_menu; // restart menu

    } else if (digitalRead(button_pin[3]) == LOW) { // confirm
      write_i2c(3);
      menu_level = 1;
      break;
    }
    delay(400);
  }

  while(digitalRead(button_pin[3]) != HIGH){
    Serial.println("release button 3...");
  }

  // --- SELECT SIDE ---
  Serial.print("Select SIDE: "); Serial.println(current_side);
  while (true) {
    if (digitalRead(button_pin[0]) == LOW) {
      current_side = (current_side % max_side) + 1;
      write_i2c(1);
      Serial.print("Side: "); Serial.println(current_side);
    } else if (digitalRead(button_pin[1]) == LOW) {
      current_side = (current_side - 2 + max_side) % max_side + 1;
      write_i2c(2);
      Serial.print("Side: "); Serial.println(current_side);
    } else if (digitalRead(button_pin[2]) == LOW) { // back to previous menu
      while(digitalRead(button_pin[2]) != HIGH){
        Serial.println("release button 2 ...");
      }
      menu_level = 0;
      write_i2c(5);
      goto start_menu;
    } else if (digitalRead(button_pin[3]) == LOW) { // confirm
      write_i2c(3);
      menu_level = 2;
      break;
    }
    delay(400);
  }

  while(digitalRead(button_pin[3]) != HIGH){
    Serial.println("release button 3...");
  }
  delay(200);
  // --- SELECT OPTION ---
  Serial.print("Select OPTION: "); Serial.println(current_option);
  while (true) {
    if (digitalRead(button_pin[0]) == LOW) {
      current_option = (current_option % max_option) + 1;
      write_i2c(1);
      Serial.print("Option: "); Serial.println(current_option);
    } else if (digitalRead(button_pin[1]) == LOW) {
      current_option = (current_option - 2 + max_option) % max_option + 1;
      write_i2c(2);
      Serial.print("Option: "); Serial.println(current_option);
    } else if (digitalRead(button_pin[2]) == LOW) { // back to previous menu
      while(digitalRead(button_pin[2]) != HIGH){
        Serial.println("release button 2 ...");
      }
      menu_level = 1;
      write_i2c(5);
      goto start_menu;
    } else if (digitalRead(button_pin[3]) == LOW) { // confirm & finish
      write_i2c(3);
      menu_level = 3;
      break;
    }
    delay(400);
  }

  while(digitalRead(button_pin[3]) != HIGH){
    Serial.println("release button ...");
  }

  // === MENU COMPLETE ===
  Serial.print("RESULT: ");
  Serial.print(current_condition);
  Serial.print(" ");
  Serial.print(current_side);
  Serial.print(" ");
  Serial.println(current_option);
  delay(2500);

  // ------------------ STAGE 2 -------------------//
  //--------- set zero then start the pump --------//
  if(current_condition == 1){ //อากาศแห้ง
    if(current_side == 1){
      if(current_option == 1){
        performSprayOperation(1, 400, 3400, 1600, 1, 5000);
        }
      else if(current_option == 2){
        performSprayOperation(2, 250, 3400, 1600, 2, 5000);
        }
    
      else if(current_option == 3){
        performSprayOperation(3, 100, 3400, 1600, 3, 5000);
        } 
      }

    else if(current_side == 2){
        if(current_option == 1){
        performSprayBackOperation(4, 3400, 1600, 1, 5000);
        }
      else if(current_option == 2){
        performSprayBackOperation(5, 3400, 1600, 2, 5000);
        }
      else if(current_option == 3){
        performSprayBackOperation(6, 3400, 1600, 3, 5000);
        }
      }
    }
  if(current_condition == 2){ //อากาศชื้น
    if(current_side == 1){
      if(current_option == 1){
        performSprayOperation(7, 400, 3400, 1600, 1, 15000);
        }
      else if(current_option == 2){
        performSprayOperation(8, 250, 3400, 1600, 2, 15000);
        }
    
      else if(current_option == 3){
        performSprayOperation(9, 100, 3400, 1600, 3, 25000);
        } 
      }

    else if(current_side == 2){
        if(current_option == 1){
        performSprayBackOperation(10, 3400, 1600, 1, 15000);
        }
      else if(current_option == 2){
        performSprayBackOperation(11, 3400, 1600, 2, 15000);
        }
      else if(current_option == 3){
        performSprayBackOperation(12, 3400, 1600, 3, 25000);
        }
      }
    }
  goto start_menu; // repeat menu forever
 }
//----------------------------------------------------------------------------------------------------//
//------------------------------------------------  ALL  ---------------------------------------------//
// ---------------------------------------- FUNCTION BELOW HERE --------------------------------------//

// เขียน i2c ส่ง
void write_i2c(int value) {
  Serial.print(value);
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(value);
  Wire.endTransmission();
}

//รับ i2c
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
    Serial.print(char(response[i]));
    text_response += (char)response[i];
  }
  Serial.println("");
  return text_response;

}


// หรือถ้าต้องการให้แน่ใจว่าต้องเจอช่องสว่างก่อนจึงจะนับทึบครั้งต่อไป
// ใช้ฟังก์ชันนี้แทน (เพิ่ม state tracking)

bool canCountNext = true;  // เพิ่มตัวแปรนี้ในส่วน global variables
bool white_start = false;

void readEncoderWithDebounce() {
  bool currentEncoderState = digitalRead(encoderPin);
  Serial.println("currentEncoderState : " + String(currentEncoderState));

  if (lastEncoderState != currentEncoderState) {
    if (currentEncoderState == LOW && !canCountNext) {
      // เจอช่องสว่าง - อนุญาตให้นับครั้งต่อไปได้
      canCountNext = true;
    }
    else if (currentEncoderState == HIGH && canCountNext) {
      // เจอของทึบ และสามารถนับได้ (เพราะเพิ่งผ่านช่องสว่างมา)
      encoderCount++;
      Serial.println("en count : " + String(encoderCount));
      canCountNext = false;  // ป้องกันการนับซ้ำ จนกว่าจะเจอช่องสว่างอีกครั้ง
    }
  }
  
  lastEncoderState = currentEncoderState;
}

// ฟังก์ชันรีเซ็ต encoder counter (แก้ไข)
void resetEncoderCount() {
  encoderCount = 0;
  canCountNext = true;  // รีเซ็ตสถานะการนับด้วย
  lastEncoderState = digitalRead(encoderPin);
  if (lastEncoderState == LOW) {
    white_start = true;
    encoderCount = 0;

  }

}

// ในฟังก์ชันหมุน 90 องศา ให้เปลี่ยนจาก readEncoder() เป็น readEncoderWithDebounce()
void spin_90_deg_new() {
  Serial.println("Starting 90 degree rotation with optical encoder...");
  resetEncoderCount();
  
  // เริ่มหมุน
  analogWrite(driveDC_PWM, 205);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  
  // หมุนจนกว่าจะได้ 2 pulse (90 องศา)
  while(encoderCount < PULSES_PER_90_DEGREES_NEW) {
    readEncoderWithDebounce();  // เปลี่ยนจาก readEncoder()
  }
  
  // หยุดมอเตอร์
  analogWrite(driveDC_PWM, 255);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
  
  Serial.println("90 degree rotation completed!");
}

// ในฟังก์ชันหมุน 90 องศา ให้เปลี่ยนจาก readEncoder() เป็น readEncoderWithDebounce()
void spin_90_deg() {
  Serial.println("Starting 90 degree rotation with optical encoder...");
  resetEncoderCount();
  
  // เริ่มหมุน
  analogWrite(driveDC_PWM, 225);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  
  // หมุนจนกว่าจะได้ 2 pulse (90 องศา)
  while(encoderCount < PULSES_PER_90_DEGREES) {
    readEncoderWithDebounce();  // เปลี่ยนจาก readEncoder()
  }
  
  // หยุดมอเตอร์
  analogWrite(driveDC_PWM, 255);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);

  Serial.println("90 degree rotation completed!");
}

// ทำเช่นเดียวกันกับฟังก์ชันอื่นๆ
void spin_90_deg_2() {
  Serial.println("Starting 90 degree rotation with optical encoder (speed 2)...");
  resetEncoderCount();
  
  // เริ่มหมุน
  analogWrite(driveDC_PWM, 205);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  
  // หมุนจนกว่าจะได้ 2 pulse (90 องศา)
  while(encoderCount < PULSES_PER_90_DEGREES) {
    readEncoderWithDebounce();  // เปลี่ยนจาก readEncoder()
  }
  
  // หยุดมอเตอร์
  analogWrite(driveDC_PWM, 255);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);

  Serial.println("90 degree rotation completed (speed 2)!");
}

void spin_90_deg_for_small() {
  Serial.println("Starting 90 degree rotation with optical encoder (for small)...");
  resetEncoderCount();
  
  // เริ่มหมุน
  analogWrite(driveDC_PWM, 165);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  
  // หมุนจนกว่าจะได้ 2 pulse (90 องศา)
  while(encoderCount < PULSES_PER_90_DEGREES) {
    readEncoderWithDebounce();  // เปลี่ยนจาก readEncoder()
  }
  
  // หยุดมอเตอร์
  analogWrite(driveDC_PWM, 225);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);

  Serial.println("90 degree rotation completed (for small)!");
}


// Function to perform the spraying operation with customizable parameters
void performSprayOperation(int option, int sprayDelayMs, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  write_i2c(9);
  delay(1000);
  
  // Turn on water pump
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  
  // Move stepper to home position
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);

  if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }

  // Check if mode button is pressed
  if (digitalRead(button_mode) == 1) {
    // Move stepper away from home position
    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    
    // Start rotation platform
    analogWrite(driveDC_PWM, 255);
    digitalWrite(driveDC_INA, HIGH);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // First spray cycle
    digitalWrite(Relay_PumpSp, HIGH);
    delay(sprayDelayMs);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // First rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    
    // Second spray cycle
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Second rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);

    // th spray cycle
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // th rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);

    // fo spray cycle
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp,  HIGH);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Return to home position
    digitalWrite(Relay_PumpSp, HIGH);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  delay(500);
  digitalWrite(Relay_LED_Blue, HIGH);
}

void performSprayOperationforsmall(int option, int sprayDelayMs, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  write_i2c(9);
  delay(1000);
  
  // Turn on water pump
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  
  // Move stepper to home position
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);

  if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }

  // Check if mode button is pressed
  if (digitalRead(button_mode) == 1) {
    // Move stepper away from home position
    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    
    // Start rotation platform
    analogWrite(driveDC_PWM, 255);
    digitalWrite(driveDC_INA, HIGH);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

     // First spray cycle
    digitalWrite(Relay_PumpSp, HIGH);
    //delay(2000);
    //digitalWrite(Relay_PumpSp, LOW);
    delay(sprayDelayMs);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // First rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);

    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    //delay(2000);
    //digitalWrite(Relay_PumpSp, LOW);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Second rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);

    // // Third spray cycle
  
    // digitalWrite(dirPin, HIGH);
    // for (int R = 0; R < stepperBackwardSteps; R++) {
    //   digitalWrite(stepPin, HIGH);
    //   delayMicroseconds(500);
    //   digitalWrite(stepPin, LOW);
    //   delayMicroseconds(500);
    // }
    
    // digitalWrite(Relay_PumpSp, HIGH);
    // delay(2000);
    // digitalWrite(Relay_PumpSp, LOW);
    // delay(sprayDelayMs);
    
    // digitalWrite(dirPin, LOW);
    // for (int R = 0; R < stepperBackwardSteps; R++) {
    //   digitalWrite(stepPin, HIGH);
    //   delayMicroseconds(500);
    //   digitalWrite(stepPin, LOW);
    //   delayMicroseconds(500);
    // }
    
    // // Second rotation
    // if (useSmallRotation == 1) {
    //   spin_90_deg_for_small();
    // } else if (useSmallRotation == 2){
    //   spin_90_deg_2();
    // } else if (useSmallRotation == 3){
    //   spin_90_deg();
    // }
    // delay(rotationDelayMs);
  
    // Four spray cycle
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // digitalWrite(Relay_PumpSp, HIGH);
    // delay(2000);
    digitalWrite(Relay_PumpSp, HIGH);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Return to home position
    digitalWrite(Relay_PumpSp, HIGH);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  delay(500);
  digitalWrite(Relay_LED_Blue, HIGH);
}

void performSprayBackOperation(int option, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  write_i2c(9);
  delay(1000);
  
  // Turn on water pump
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  
  if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }

  // Move stepper to home position
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);

  // Check if mode button is pressed
  if (digitalRead(button_mode) == 1) {
    // Move stepper away from home position
    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    
    // Start rotation platform
    analogWrite(driveDC_PWM, 255);
    digitalWrite(driveDC_INA, HIGH);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    // First spray cycle
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(Relay_PumpWater, LOW);
    // First rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    digitalWrite(Relay_PumpWater, HIGH);
    delay(1000);
    // Second spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(Relay_PumpWater, LOW);
    // 2 rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    digitalWrite(Relay_PumpWater, HIGH);
    delay(1000);

    // Third spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(Relay_PumpWater, LOW);
    // 3 rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    digitalWrite(Relay_PumpWater, HIGH);
    delay(1000);
    
    // Four spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    // Return to home position
    digitalWrite(Relay_PumpSp, HIGH);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);

  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  delay(500);
  digitalWrite(Relay_LED_Blue, HIGH);
}

void performSprayBackOperation_for_small(int option, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  write_i2c(9);
  delay(1000);
  
  // Turn on water pump
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  
  if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }

  // Move stepper to home position
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);

  // Check if mode button is pressed
  if (digitalRead(button_mode) == 1) {
    // Move stepper away from home position
    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    
    // Start rotation platform
    analogWrite(driveDC_PWM, 255);
    digitalWrite(driveDC_INA, HIGH);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    // First spray cycle
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(Relay_PumpWater, LOW);
    // First rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    digitalWrite(Relay_PumpWater, HIGH);
    delay(1000);


    // Third spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(Relay_PumpWater, LOW);
    // 3 rotation
    if (useSmallRotation == 1) {
      spin_90_deg_for_small();
    } else if (useSmallRotation == 2){
      spin_90_deg_2();
    } else if (useSmallRotation == 3){
      spin_90_deg();
    }
    delay(rotationDelayMs);
    digitalWrite(Relay_PumpWater, HIGH);
    delay(1000);
    
    // Four spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, LOW);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    // Return to home position
    digitalWrite(Relay_PumpSp, HIGH);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);

  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  delay(500);
  digitalWrite(Relay_LED_Blue, HIGH);
}
