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
int var = 0;
 
int position = 0;
int startposition = 90;

unsigned long previousMillis = 0;
const long interval = 500;

int dish = 0;



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

  write_i2c(3);

  while(digitalRead(button_pin[3]) != LOW){
    
      if (digitalRead(button_pin[0]) == LOW) {  // Button 1 pressed
        var = 1;
      } else if (digitalRead(button_pin[1]) == LOW) { // Button 2 pressed
        var = 2;
      } else {
        var = 0;
      }

      switch (var) {
        case 1:
          Serial.println("Option 1");
          dish =+ 1;
          if(dish > 4):
            dish = 4;
          write_i2c(1);
          break;
        case 2:
          Serial.println("Option 2");
          dish =- 1;
          if(dish < 1):
            dish = 1;
          write_i2c(2);
          break;
        default:
          // No button pressed
          break;
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
  }

  
  //เมื่อปุ่มเขียวถูกกดแล้ว
  digitalWrite(Relay_LED_Green, HIGH);
  write_i2c(4); //remove menu gui
  spin_90_deg();
  // ------------------ STAGE 2 -------------------//
  //--------- set zero then start the pump --------//
  if(dish == 1){
  Serial.println("Option 1");
   digitalWrite(Relay_LED_Blue, LOW);
  write_i2c(9);
  //ทำงาานแขนพ่น อันนี้เช็คว่าอยู่ตำแหน่งเริ่มต้นยัง
  servoMotor.write(startposition);// อยู่ตำแหน่งปิด 
  delay(1000);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);

  switch (digitalRead(button_mode)) {
  case 1:
 

    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    //แท่นหมุน
    analogWrite(driveDC_PWM, 0);
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, HIGH);
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 3400; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    servoMotor.write(180);

    digitalWrite(Relay_PumpSp, LOW);
    delay(600);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < 1400; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      }

       spin_90_deg();
    delay(2000);
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 1400; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
  
  digitalWrite(Relay_PumpSp, LOW);
  delay(500);

  digitalWrite(dirPin, LOW);
    for (int R = 0; R < 1400; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      }
    
      
    spin_90_deg();
    delay(2000);
    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 1400; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
  delay(600);
  digitalWrite(dirPin, LOW);
  for (int R = 0; R < 1700; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, LOW);
    delay(500);
    digitalWrite(dirPin, LOW);

    digitalWrite(Relay_PumpSp, LOW);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
    servoMotor.write(90);
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
    //หยุดแท่นหมุน
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, LOW);
    delay(500);
    break;
  case 0:
    // statements
    break;
  default:
    // statements
    break;
}

  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  digitalWrite(Relay_LED_Blue, HIGH);
  }
  else if(dish == 2){
     Serial.println("Option 2");
   digitalWrite(Relay_LED_Blue, LOW);
  write_i2c(9);
  //ทำงาานแขนพ่น อันนี้เช็คว่าอยู่ตำแหน่งเริ่มต้นยัง
  servoMotor.write(startposition);// อยู่ตำแหน่งปิด 
  delay(1000);
  digitalWrite(Relay_PumpWater, HIGH);
  delay(1000);
  digitalWrite(dirPin, LOW);
  while(digitalRead(limitmotor) == HIGH){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
  Serial.println("start stepper--------");
  delay(1000);
 

  switch (digitalRead(button_mode)) {
  case 1:

    digitalWrite(dirPin, HIGH);
    while(digitalRead(limitmotor) == HIGH){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    Serial.println("start dc and spray--------");
    //แท่นหมุน
    analogWrite(driveDC_PWM, 0);
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, HIGH);
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 3150; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, HIGH);
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, LOW);
    spin_90_deg_2();
    delay(1000);
    
    servoMotor.write(180);
  
    digitalWrite(Relay_PumpSp, LOW);
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      }
      digitalWrite(Relay_PumpSp, HIGH);

      digitalWrite(dirPin, LOW);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    digitalWrite(Relay_PumpSp, LOW);
    spin_90_deg_2();
    delay(1000);
    
    digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    digitalWrite(Relay_PumpSp, HIGH);

    digitalWrite(dirPin, LOW);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
  
  digitalWrite(Relay_PumpSp, LOW);
    spin_90_deg_2();
    delay(1000);

  digitalWrite(dirPin, HIGH);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
      }
      digitalWrite(Relay_PumpSp, HIGH);
 
    digitalWrite(dirPin, LOW);
    for (int R = 0; R < 800; R++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
    
    digitalWrite(Relay_PumpSp, LOW);

    digitalWrite(Relay_PumpSp, LOW);
    delay(500);
    digitalWrite(dirPin, LOW);
    analogWrite(driveDC_PWM, 0); //หมุนต่อเนื่อง
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, HIGH);

    delay(10000);
   
    digitalWrite(Relay_PumpSp, LOW);
    delay(4500);
    while(digitalRead(limitmotor) == HIGH){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
    servoMotor.write(90);
    digitalWrite(Relay_PumpWater, LOW);
    delay(2000);
    
    //หยุดแท่นหมุน
    digitalWrite(driveDC_INA, LOW);
    digitalWrite(driveDC_INB, LOW);
    delay(500);
    break;
  case 0:
    // statements
    break;
  default:
    // statements
    break;
}

 else if(dish == 3){

 }

 else if(dish == 4){

 }
  
  Serial.println("----------------------- DONE -----------------------");
  write_i2c(8);
  digitalWrite(Relay_LED_Blue, HIGH);
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
  analogWrite(driveDC_PWM, 185);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  delay(200);  // Adjust this value to get as close to 90° as possible
  while(digitalRead(limit90deg) != HIGH){
    Serial.println("spining to 90 deg...");
  }
  // Active braking
  analogWrite(driveDC_PWM, 205);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, LOW);
  delay(100);  // brief brake
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, LOW);
}

void spin_90_deg_2() {
  analogWrite(driveDC_PWM, 205);               // Slower speed
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, HIGH);
  delay(200);  // Adjust this value to get as close to 90° as possible
  while(digitalRead(limit90deg) != HIGH){
    Serial.println("spining to 90 deg...");
  }
  // Active braking
  analogWrite(driveDC_PWM, 255);
  digitalWrite(driveDC_INA, HIGH);
  digitalWrite(driveDC_INB, LOW);
  delay(100);  // brief brake
  analogWrite(driveDC_PWM, 0);
  digitalWrite(driveDC_INA, LOW);
  digitalWrite(driveDC_INB, LOW);
}