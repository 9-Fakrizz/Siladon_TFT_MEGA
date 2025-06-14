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
const int limit90deg = 42;

//dc_motor_หมุน
const int driveDC_PWM = 7;
const int driveDC_INA = 5;
const int driveDC_INB = 4;

int servoPin = 50; 
 
int position = 0;
int startposition = 90;

unsigned long previousMillis = 0;
const long interval = 500;

///menu select
int current_condition = 1;
int current_side = 1;
int current_option = 1;

int max_condition = 2;
int max_side = 2;
int max_option = 3;

bool condition_selected = false;
bool side_selected = false;
bool option_selected = false;




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

  digitalWrite(enPin, LOW);
  digitalWrite(Relay_LED_Red, LOW);
  digitalWrite(Relay_LED_Green, HIGH);
  digitalWrite(Relay_LED_Blue, HIGH);
  digitalWrite(Relay_PumpSp, LOW);
  digitalWrite(Relay_PumpWater, LOW);

  servoMotor.attach(servoPin);
  servoMotor.write(90);
  spin_90_deg();
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
  digitalWrite(Relay_PumpSp, HIGH);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(500);
  write_i2c(6);

  while(digitalRead(button_pin[1]) != LOW){           // รอคำสั่งจากปุ่ม 2
    Serial.println("-------press for done--------");
  }
  //  ปิด pump spray
  digitalWrite(Relay_PumpSp, LOW);
  delay(500);
  write_i2c(8);

  // ------------------------------------------------------------------------------- //

  Serial.println(">>>>>>>>>  setup done");
  digitalWrite(Relay_LED_Red, HIGH);

}

void loop() {
  write_i2c(8);
  buttonState = digitalRead(button_pin[2]);  // อ่านสถานะปุ่มปัจจุบัน

  servoMotor.write(startposition);// อยู่ตำแหน่งปิด 
  delay(1000);

  digitalWrite(Relay_PumpWater, LOW);
  Serial.println("Start------------");

  // ------------------ STAGE 1 --------------------//
  // --   setup stepper motor to start position   --//
  digitalWrite(Relay_LED_Green, LOW);
  //write_i2c(9); // display working
  write_i2c(5);

  condition_selected = false;
  side_selected = false;
  option_selected = false;
  write_i2c(4);
  // select condition
  while(!condition_selected){
    if (digitalRead(button_pin[0]) == LOW) {  // Button 1 pressed
      current_condition++;
      if(current_condition > max_condition) current_condition = 1;
      write_i2c(1);
    }else if (digitalRead(button_pin[1]) == LOW) { // Button 2 pressed
      current_condition--;
      if(current_condition < 1) current_condition = max_condition;
      write_i2c(2);
    }else if(digitalRead(button_pin[3]) == LOW){
      Serial.println("condition selected ! : " +String(current_condition));
      condition_selected = true;
      write_i2c(5);
    }

    Serial.println("Waiting for press green button...");
    if(digitalRead(button_pin[2]) == LOW){
      Serial.println("yellow pressed.");
      digitalWrite(Relay_LED_Green, HIGH);
      digitalWrite(Relay_LED_Red, LOW);
      servoMotor.write(0);// อยู่ตำแหน่งปิด 
      delay(1500);
      digitalWrite(Relay_PumpWater, HIGH);
      digitalWrite(Relay_PumpSp, HIGH);
      delay(500);
      write_i2c(6);
      delay(500);
      while(digitalRead(button_pin[2]) != LOW){
        Serial.println("Waiting for stop yellow.");
      }
      digitalWrite(Relay_PumpSp, LOW);
      delay(500);
      write_i2c(8);
      digitalWrite(Relay_LED_Green, LOW);
      digitalWrite(Relay_LED_Red, HIGH);
    }
    delay(200);
  }

  // select side
  write_i2c(3);
  while(!side_selected){
    if (digitalRead(button_pin[0]) == LOW) {  // Button 1 pressed
      current_side++;
      if(current_side > max_side) current_side = 1;
      write_i2c(1);
    }else if (digitalRead(button_pin[1]) == LOW) { // Button 2 pressed
      current_side--;
      if(current_side < 1) current_side = max_side;
      write_i2c(2);
    }else if(digitalRead(button_pin[3]) == LOW){
      Serial.println("side selected ! : " +String(current_side));
      side_selected = true;
      write_i2c(5);
    }

    Serial.println("Waiting for press green button...");
    if(digitalRead(button_pin[2]) == LOW){
      Serial.println("yellow pressed.");
      digitalWrite(Relay_LED_Green, HIGH);
      digitalWrite(Relay_LED_Red, LOW);
      servoMotor.write(0);// อยู่ตำแหน่งปิด 
      delay(1500);
      digitalWrite(Relay_PumpWater, HIGH);
      digitalWrite(Relay_PumpSp, HIGH);
      delay(500);
      write_i2c(6);
      delay(500);
      while(digitalRead(button_pin[2]) != LOW){
        Serial.println("Waiting for stop yellow.");
      }
      digitalWrite(Relay_PumpSp, LOW);
      delay(500);
      write_i2c(8);
      digitalWrite(Relay_LED_Green, LOW);
      digitalWrite(Relay_LED_Red, HIGH);
    }
    delay(200);
  }

  //select option
  write_i2c(2);
  while(!option_selected){
    if (digitalRead(button_pin[0]) == LOW) {  // Button 1 pressed
      current_option++;
      if(current_option > max_option) current_option = 1;
      write_i2c(1);
    }else if (digitalRead(button_pin[1]) == LOW) { // Button 2 pressed
      current_option--;
      if(current_option < 1) current_option = max_option;
      write_i2c(2);
    }else if(digitalRead(button_pin[3]) == LOW){
      Serial.println("option selected ! : " +String(current_option));
      option_selected = true;
      write_i2c(5);
    }

    Serial.println("Waiting for press green button...");
    if(digitalRead(button_pin[2]) == LOW){
      Serial.println("yellow pressed.");
      digitalWrite(Relay_LED_Green, HIGH);
      digitalWrite(Relay_LED_Red, LOW);
      servoMotor.write(0);// อยู่ตำแหน่งปิด 
      delay(1500);
      digitalWrite(Relay_PumpWater, HIGH);
      digitalWrite(Relay_PumpSp, HIGH);
      delay(500);
      write_i2c(6);
      delay(500);
      while(digitalRead(button_pin[2]) != LOW){
        Serial.println("Waiting for stop yellow.");
      }
      digitalWrite(Relay_PumpSp, LOW);
      delay(500);
      write_i2c(8);
      digitalWrite(Relay_LED_Green, LOW);
      digitalWrite(Relay_LED_Red, HIGH);
    }
    delay(200);
  }

  
  //เมื่อปุ่มเขียวถูกกดแล้ว
  write_i2c(5);
  digitalWrite(Relay_LED_Green, HIGH);
  Serial.print("Option  " +String(current_option));
  Serial.println("Selected");
  delay(1000);

  // ------------------ STAGE 2 -------------------//
  //--------- set zero then start the pump --------//
  if(current_condition == 1){ //อากาศแห้ง
    if(current_side == 1){
      if(current_option == 1){
        performSprayOperationforsmall(1, 600, 3400, 1600, 1, 5000);
        }
      else if(current_option == 2){
        performSprayOperation(2, 300, 3400, 1600, 2, 5000);
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
        performSprayOperationforsmall(7, 600, 3400, 1600, 1, 15000);
        }
      else if(current_option == 2){
        performSprayOperation(8, 300, 3400, 1600, 2, 15000);
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



void spin_90_deg() {
  analogWrite(driveDC_PWM, 225);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  while(digitalRead(limit90deg) != LOW){
    Serial.println("spining to 90 deg...");
  }
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
  analogWrite(driveDC_PWM, 225);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH); 
  while(digitalRead(limit90deg) != HIGH){
    Serial.println("spining to 90 deg...");
}
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
}

void spin_90_deg_2() {
  analogWrite(driveDC_PWM, 185);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  while(digitalRead(limit90deg) != LOW){
    Serial.println("spining to 90 deg...");
  }
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
  analogWrite(driveDC_PWM, 185);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH); 
  while(digitalRead(limit90deg) != HIGH){
    Serial.println("spining to 90 deg...");
  }

  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
}

void spin_90_deg_for_small() {
  analogWrite(driveDC_PWM, 155);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);

  while(digitalRead(limit90deg) != LOW){
    Serial.println("spining to 90 deg...");
  }
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, HIGH);
  delay(500);
  analogWrite(driveDC_PWM, 155);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH); 
  while(digitalRead(limit90deg) != HIGH){
    Serial.println("spining to 90 deg...");
  }

  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, LOW);
}


// Function to perform the spraying operation with customizable parameters
void performSprayOperation(int option, int sprayDelayMs, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
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
    analogWrite(driveDC_PWM, 0);
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // First spray cycle
    digitalWrite(Relay_PumpSp, LOW);
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
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);
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

    // Second spray cycle
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);
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

    // Third spray cycle
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Return to home position
    digitalWrite(Relay_PumpSp, LOW);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
    // Stop rotation platform
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, LOW);
    delay(500);
  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  digitalWrite(Relay_LED_Blue, HIGH);
}

void performSprayOperationforsmall(int option, int sprayDelayMs, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
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
    analogWrite(driveDC_PWM, 0);
    digitalWrite(driveDC_INA, LOW);
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
    delay(2000);
    digitalWrite(Relay_PumpSp, LOW);
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

    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    delay(2000);
    digitalWrite(Relay_PumpSp, LOW);
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

    // Third spray cycle
  
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    delay(2000);
    digitalWrite(Relay_PumpSp, LOW);
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
  
    // Four spray cycle
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, HIGH);
    delay(2000);
    digitalWrite(Relay_PumpSp, LOW);
    delay(sprayDelayMs);
    
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    // Return to home position
    digitalWrite(Relay_PumpSp, LOW);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
    // Stop rotation platform
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, LOW);
    delay(500);
  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  digitalWrite(Relay_LED_Blue, HIGH);
}

void performSprayBackOperation(int option, int stepperForwardSteps, int stepperBackwardSteps, 
                          int useSmallRotation, int rotationDelayMs) {
  Serial.println("Option " + String(option));
  digitalWrite(Relay_LED_Blue, LOW);
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
    analogWrite(driveDC_PWM, 0);
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, HIGH);
    
    // Initial spray position
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < stepperForwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, HIGH);
    delay(1000);
    // First spray cycle
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
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
    digitalWrite(Relay_PumpSp, HIGH);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);
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
    digitalWrite(Relay_PumpSp, HIGH);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);
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

    digitalWrite(Relay_PumpSp, HIGH);
    delay(1000);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < stepperBackwardSteps + 300; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    // Return to home position
    digitalWrite(Relay_PumpSp, LOW);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
    // Stop rotation platform
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, LOW);
    delay(500);
  }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  digitalWrite(Relay_LED_Blue, HIGH);
}
