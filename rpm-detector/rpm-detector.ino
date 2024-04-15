////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////rpm-detector/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Defining some Variables 

const int wheelSpeedSensorMagnets = 15;                               //wheelSpeed
const float wheelCircumferenceInMeter = 0.52;                         //wheelSpeed
const int wheelSpeedSensorPin = 34;                                   //wheelSpeed
const int wheelSpeedMaxRotationTriggersPerSecondConstant = 1000;      //wheelSpeed
const float refreshIntervalInMillis = 1000.0;

//those Variables are stored in RAM! 
volatile int wheelSpeedMaxRotationTriggersPerSecond = wheelSpeedMaxRotationTriggersPerSecondConstant;           //wheelSpeed
volatile unsigned long wheelSpeedRotationTimestamps[wheelSpeedMaxRotationTriggersPerSecondConstant] = {0};      //wheelSpeed
volatile int wheelSpeedRotationTimestampsIndex = 0;                   //wheelSpeed
volatile byte wheelSpeedSensorTriggerState = LOW;                     //wheelSpeed
volatile unsigned long wheelSpeedInterruptTime = 0;                   //wheelSpeed
volatile unsigned long wheelSpeedLastInterruptTime = 0;               //wheelSpeed


////////////////////////////////////////////////////////////VOID SETUP////////////////////////////////////////////////////////////////////////////////



//Void Setup (only runs once)

void setup() {

  Serial.begin(115200);

  pinMode(wheelSpeedSensorPin, INPUT_PULLUP);                                                         //wheelSpeed
  attachInterrupt(digitalPinToInterrupt(wheelSpeedSensorPin), wheelSpeedSensorInterrupt, CHANGE);     //wheelSpeed

}



//////////////////////////////////////////////////////////INTERRUPTS///////////////////////////////////////////////////////////////////////////////////



//Interrupt when Hall Effect Pin gets triggered (wheel speed)

void wheelSpeedSensorInterrupt() {

  wheelSpeedInterruptTime = micros();
  if (wheelSpeedInterruptTime - wheelSpeedLastInterruptTime > 250){
    
          bool currentSensorState = digitalRead(wheelSpeedSensorPin);
          unsigned long currentMicros = wheelSpeedInterruptTime;
          
          if (currentSensorState == wheelSpeedSensorTriggerState) {
            wheelSpeedRotationTimestamps[wheelSpeedRotationTimestampsIndex] = currentMicros;
            wheelSpeedRotationTimestampsIndex = (wheelSpeedRotationTimestampsIndex + 1) % wheelSpeedMaxRotationTriggersPerSecond;
        
          }
         wheelSpeedLastInterruptTime = wheelSpeedInterruptTime;
   }
}


/////////////////////////////////////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////



//Function for Calculating Triggers (wheel speed)

int getWheelSpeedRotationTriggersOfLastSecond() {

  unsigned long currentTimestamp = micros();
  int wheelSpeedRotationTriggers = 0;

  for (int i = 0; i < wheelSpeedMaxRotationTriggersPerSecond; i++) {
    unsigned long timestamp = wheelSpeedRotationTimestamps[i];
    
    if (currentTimestamp - timestamp <= 1000000) {
      
      wheelSpeedRotationTriggers ++;

    }
  }

  return wheelSpeedRotationTriggers;

}





//Function for Calculating RPM (wheel speed)

float calculateWheelSpeedRPM(int wheelSpeedRotationTriggersOfLastSecond) {

  const float wheelSpeedRotationsOfLastSecond = (float)wheelSpeedRotationTriggersOfLastSecond / (float)wheelSpeedSensorMagnets;
  const float wheelSpeedRPM = wheelSpeedRotationsOfLastSecond * 60.0;

  return wheelSpeedRPM;

}




//Function for Calculating KMH (wheel speed)

float calculateWheelSpeedKMH(float wheelSpeedRPM) {

  const float wheelSpeedRPH = wheelSpeedRPM * 60.0;
  float wheelSpeedMeterPH = wheelSpeedRPH * wheelCircumferenceInMeter;
  float wheelSpeedKMH = wheelSpeedMeterPH / 1000.0;

  return wheelSpeedKMH;

}








///////////////////////////////////////////////////////VOID LOOP/////////////////////////////////////////////////////////////////////////////////

unsigned long lastRefresh = millis();

//Void Loop 
void loop() {

  unsigned long currentMillis = millis();
  if(currentMillis - lastRefresh >= refreshIntervalInMillis) {                                                                            //wheelSpeed
    
    const int wheelSpeedRotationTriggersOfLastSecond = getWheelSpeedRotationTriggersOfLastSecond();       //wheelSpeed
    const float wheelSpeedRPM = calculateWheelSpeedRPM(wheelSpeedRotationTriggersOfLastSecond);           //wheelSpeed
  
    const float wheelSpeedKMH = calculateWheelSpeedKMH(wheelSpeedRPM);                                    //wheelSpeed
  
    Serial.print(String("Current RPM:") + String(wheelSpeedRPM, 0));                                //wheelSpeed
    Serial.print(",");                                                                              //wheelSpeed
    Serial.print(String("Current KMH:") + String(wheelSpeedKMH, 2));                                //wheelSpeed
    Serial.println("");                                                                             //wheelSpeed

    lastRefresh = millis();
  }

}
