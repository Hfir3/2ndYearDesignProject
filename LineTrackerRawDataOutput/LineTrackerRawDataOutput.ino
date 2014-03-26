#include <Servo.h>

//Letter Aquisition Test Code

const int lineTrackerPin1 = A0;
const int lineTrackerPin2 = A1;
const int aquisitionMotorPin = 7;
Servo aquisitionMotor;

int lineTrackerData1 = 0;
int lineTrackerData2 = 0;
int deliveryAddress = -1;
int letterInTransitFlag =0;

void setup() {
  pinMode(lineTrackerPin1, INPUT);
  pinMode(lineTrackerPin2, INPUT);
  
  aquisitionMotor.attach(aquisitionMotorPin);
  
  Serial.begin(9600);
}


void loop() {
  readLineTrackers();
  Serial.print(lineTrackerData2);
  Serial.print("     ");
  Serial.println(lineTrackerData1);
}

void readLineTrackers() {
  lineTrackerData1 = analogRead(lineTrackerPin1);
  lineTrackerData2 = analogRead(lineTrackerPin2); 
}
