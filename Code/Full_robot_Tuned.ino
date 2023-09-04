#include "NewPing.h" //Sonar

#define TRIGGER_PIN A4 //Sonar
#define ECHO_PIN A5 //Sonar
#define MAX_DISTANCE 400  //Sonar

#define IR1 7 //leftmost
#define IR2 6
#define IR3 5
#define IR4 4
#define IR5 3
#define IR6 2 //rightmost


#define RMotorIN1 11 //motor2
#define RMotorIN2 8
//#define RMotorPWM 2
#define RMotorEn 9

#define LMotorIN3 13 //motor 1  // inA
#define LMotorIN4 12           //inB
//#define LMotorPWM 3
#define LMotorEn 10

#define MaxSpeed 120//100
#define MotorBaseSpeed  30 //60

int IR_val[] = {0, 0, 0, 0, 0, 0};
int IR_weights[] = {-8, -4, -3, 1, 2, 4};//{-6, -5.5, -1, 1, 5.5, 6}or{-1.8, -1.5, -1, 1, 1.5, 1.8}or{-12, -10, -3.5, 3, 6, 8}

int LMotorSpeed = 0;
int RMotorSpeed = 0;
int speedAdjust = 0;

float P, I, D;
float error = 0;
float previousError = 0;
float Kp = 8; //7
float Kd = 60; //2
float Ki = 0; //0.5

int State = 0; //For T-junction
float distance = 0.0; //Sonar

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //Sonar

void read_IR();
void set_speed();
void set_forward();
void stop_motors();
void read_side_IRs();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(IR6, INPUT);

  pinMode(RMotorIN1, OUTPUT);
  pinMode(RMotorIN2, OUTPUT);

  pinMode(LMotorIN3, OUTPUT);
  pinMode(LMotorIN4, OUTPUT);

  set_forward();
}


void loop() {
Serial.println(State);

read_IR();
distance = sonar.ping_cm(); //Sonar

if ((IR_val[0] == 0) && (IR_val[1] == 0) && (IR_val[2] == 1) && (IR_val[3] == 1) && (IR_val[4] == 0) && (IR_val[5] == 0)){
  stop_motors();
}

else if ((IR_val[0] == 1) && (IR_val[1] == 1) && (IR_val[2] == 1) && (IR_val[3] == 1) && (IR_val[4] == 1) && (IR_val[5] == 1)){
    analogWrite(LMotorEn, 0);
    analogWrite(RMotorEn, 0);
    delay(500);
    //Serial.println("Turn");
    turnAround(360);
  }

else if (distance < 10){
    turnAroundright(180);
    State = 1;
  }

//At a T junction
  else if ((((IR_val[0] == 1) && (IR_val[1] == 1) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 0) && (IR_val[5] == 0))||((IR_val[0] == 0) && (IR_val[1] == 0) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 1) && (IR_val[5] == 1))) && State == 0){
    // State = 1;
    // set_T_speed();
    LMotorSpeed = MotorBaseSpeed;
    RMotorSpeed = MotorBaseSpeed;
    set_speed();
    delay(500);
    State = 1;
    }

 // After a T junctoin dead end
  else if (((IR_val[0] == 0) && (IR_val[1] == 0) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 1) && (IR_val[5] == 1)) && (State == 1)){
    //while ((IR_val[0] == 1) && (IR_val[1] == 1) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 1) && (IR_val[5] == 1)){
    turnAroundleft(90);
    State = 0;
   // }
  }

  else if (((IR_val[0] == 1) && (IR_val[1] == 1) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 0) && (IR_val[5] == 0)) && (State == 1)){
    //while ((IR_val[0] == 1) && (IR_val[1] == 1) && (IR_val[2] == 0) && (IR_val[3] == 0) && (IR_val[4] == 1) && (IR_val[5] == 1)){
    turnAroundright(90);
    State = 0;
   // }
  }

/*Serial.print(IR_val[0]);
Serial.print(", ");
Serial.print(IR_val[1]);
Serial.print(", ");
Serial.print(IR_val[2]);
Serial.print(", ");
Serial.print(IR_val[3]);
Serial.print(", ");
Serial.print(IR_val[4]);
Serial.print(", ");
Serial.println(IR_val[5]);
Serial.print("error:  ");
Serial.println(error);
*/
else{  

error = 0;
for (int i = 0; i < 6; i++) {
  //Serial.print(IR_val[i]);
  error += IR_weights[i] * !IR_val[i];
  }

 // Serial.print('-');
 // Serial.println(error);
//  delay(1000);
  //Serial.println("Line");
  P = error;
  I = I + error;
  D = error - previousError;
  /*Serial.print("error");
  Serial.println(error);
  Serial.print("Previous error");
  Serial.println(previousError);*/

  previousError = error;

  speedAdjust = Kp * P + Ki * I + Kd * D;

  LMotorSpeed = MotorBaseSpeed + speedAdjust/2;
  RMotorSpeed = MotorBaseSpeed - speedAdjust/2;
  //Serial.print("speedAdjust:");
  //Serial.println(speedAdjust);

  if (LMotorSpeed < 0) {
    LMotorSpeed = 0;
  }
  if (RMotorSpeed < 0) {
    RMotorSpeed = 0;
  }
  if (LMotorSpeed > MaxSpeed) {
    LMotorSpeed = MaxSpeed;
  }
  if (RMotorSpeed > MaxSpeed) {
    RMotorSpeed = MaxSpeed;
  }

  set_speed();

/*Serial.print("Left motor speed: ");
Serial.print(LMotorSpeed);
Serial.print(" , ");
Serial.print("Right motor speed: ");
Serial.println(RMotorSpeed);
*/
// }

  }
}




void read_IR() {
  IR_val[0] = digitalRead(IR1);
  IR_val[1] = digitalRead(IR2);
  IR_val[2] = digitalRead(IR3);
  IR_val[3] = digitalRead(IR4);
  IR_val[4] = digitalRead(IR5);
  IR_val[5] = digitalRead(IR6);
}

void stop_motors() {
  digitalWrite(RMotorEn , LOW);
  digitalWrite(LMotorEn , LOW);
//  digitalWrite(RMotorIN1, LOW);
//  digitalWrite(RMotorIN2, LOW);
//  digitalWrite(LMotorIN3, LOW);
//  digitalWrite(LMotorIN4, LOW);
}

void set_forward() {
  digitalWrite(RMotorEn , HIGH);
  digitalWrite(LMotorEn , HIGH);
  digitalWrite(RMotorIN1, HIGH);
  digitalWrite(RMotorIN2, LOW);
  digitalWrite(LMotorIN3, HIGH);
  digitalWrite(LMotorIN4, LOW);
}

void set_speed() {
  analogWrite(RMotorEn, RMotorSpeed);
  analogWrite(LMotorEn, LMotorSpeed);
  digitalWrite(RMotorIN1, HIGH);
  digitalWrite(RMotorIN2, LOW);
  digitalWrite(LMotorIN3, HIGH);
  digitalWrite(LMotorIN4, LOW);
}

void set_T_speed() {
  analogWrite(RMotorEn, RMotorSpeed);
  analogWrite(LMotorEn, LMotorSpeed);
  digitalWrite(RMotorIN1, HIGH);
  digitalWrite(RMotorIN2, LOW);
  digitalWrite(LMotorIN3, HIGH);
  digitalWrite(LMotorIN4, LOW);
  delay(1000); 
}

void turnAround(int degrees) {
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(5);

  //int turnTime = (degrees * 5000) / 360;  // Calculate turn time based on degrees
  int runtime = 100;
  // Set motor speeds to turn the robot around
  analogWrite(LMotorEn, 55);
  analogWrite(RMotorEn, 55);
  digitalWrite(RMotorIN1, LOW);  // Set these pins accordingly for your motor configuration
  digitalWrite(RMotorIN2, HIGH);
  digitalWrite(LMotorIN3, HIGH);   // Set these pins accordingly for your motor configuration
  digitalWrite(LMotorIN4, LOW);
  
  delay(runtime);  // Wait for the turn to complete
  
  // Stop the motors
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(10);
}

void turnAroundleft(int degrees){
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(10);

  //int turntime = (degrees * 500) / 360;  // Calculate turn time based on degrees
  int turntime = 250;
  // Set motor speeds to turn the robot around
  analogWrite(LMotorEn, 60);
  analogWrite(RMotorEn, 60);
  digitalWrite(RMotorIN1, HIGH);  // Set these pins accordingly for your motor configuration
  digitalWrite(RMotorIN2, LOW);
  digitalWrite(LMotorIN3, LOW);   // Set these pins accordingly for your motor configuration
  digitalWrite(LMotorIN4, HIGH);
  
  delay(turntime);  // Wait for the turn to complete
  
  // Stop the motors
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(10);
}

void turnAroundright(int degrees) {
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(10);

  //int turntime = (degrees * 500) / 360;  // Calculate turn time based on degrees
  int turntime = 250;
  // Set motor speeds to turn the robot around
  analogWrite(LMotorEn, 60);
  analogWrite(RMotorEn, 60);
  digitalWrite(RMotorIN1, LOW);  // Set these pins accordingly for your motor configuration
  digitalWrite(RMotorIN2, HIGH);
  digitalWrite(LMotorIN3, HIGH);   // Set these pins accordingly for your motor configuration
  digitalWrite(LMotorIN4, LOW);
  
  delay(turntime);  // Wait for the turn to complete
  
  // Stop the motors
  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(10);
}


/*void turn(int degrees){

  analogWrite(LMotorEn, 100);
  analogWrite(RMotorEn, 100);
  digitalWrite(RMotorIN1, LOW);  // Set these pins accordingly for your motor configuration
  digitalWrite(RMotorIN2, HIGH);
  digitalWrite(LMotorIN3, HIGH);   // Set these pins accordingly for your motor configuration
  digitalWrite(LMotorIN4, LOW);
  delay(100);

  analogWrite(LMotorEn, 0);
  analogWrite(RMotorEn, 0);
  delay(100);
}*/

