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
  if ((deliveryAddress == -1) && (letterInTransitFlag ==0))
    aquisitionMotor.writeMicroseconds(1750);
  readLineTrackers();
  if ((lineTrackerData1 > 700)||(lineTrackerData1 < 100)) {
  //grey is around 626, white is around 33
    aquisitionMotor.writeMicroseconds(1500);
    //letterInTransitFlag = 1;
    if (lineTrackerData1 < 100) {
      if (lineTrackerData2 < 100)
        deliveryAddress = 0;
      else 
        deliveryAddress = 1;
    }
    else if (lineTrackerData2 > 700) {
      if (lineTrackerData2 > 700)
        deliveryAddress = 3;
      else 
        deliveryAddress = 4;
    }
  }
  else
      deliveryAddress = -1;
  
  Serial.println(deliveryAddress);
  if (letterInTransitFlag ==2) {
    aquisitionMotor.writeMicroseconds(1250);
    delay(1000);
    letterInTransitFlag = 0;
  }
}

void readLineTrackers() {
  lineTrackerData1 = analogRead(lineTrackerPin1);
  lineTrackerData2 = analogRead(lineTrackerPin2); 
}
