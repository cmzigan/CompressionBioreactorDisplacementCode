// RTC setup 

#include "uRTCLib.h" 

uRTCLib rtc(0x68); 

 

const int StartTime1 = 10; 

const int StopTime1 = 11; 

const int StartTime2 = 12; 

const int StopTime2 = 13; 

const int StartTime3 = 14; 

const int StopTime3 = 15; 

const int StartTime4 = 16; 

const int StopTime4 = 17; 

 

// outputs for stepper 

const int StpPin1 = 10; 

const int DirPin1 = 11; 

const int HomeLED = 3; 

const int autoLED = 4; 

 

// inputs  

int homeStep = 0; 

int autoCycling = 0; 

int extendto1 = 0; 

 

// control variables for travelling long distances 

int extendedto1 = 0; 

int homed = 0; 

 

// Distances for steps 

const int strain1to10 = .44/.006; 

const int homing = 3/.006; 

const int hometo1B = (3.3 -.5)/.006; // validation - difference of well plate bottom 

const int hometo1R = (2.4 -.5)/.006; 

 

// variables for FSM 

int state = 0; 

const int homingStep = 300; 

const int MoveOnePerc = 350; 

const int autoCycle = 400; 

const int extend = 500; 

void setup() { 

  // put your setup code here, to run once: 

  pinMode(StpPin1, OUTPUT); 

  pinMode(DirPin1, OUTPUT); 

 

  pinMode(HomeLED, OUTPUT); 

  pinMode(autoLED, OUTPUT); 

 

  Serial.begin(9600); 

   

  URTCLIB_WIRE.begin(); 

  rtc.set(55, 59, 9 , 11, 1, 4, 23); 

  // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year) 

} 

void loop() { 

  checkTime(); 

  Serial.print("State = "); Serial.println(state); 

  delay(10); 

   

  switch(state) { 

    case 0: // no motion 

      digitalWrite(HomeLED, LOW); 

      digitalWrite(autoLED, LOW); 

      homed = 0; 

      extendedto1 = 0; 

      Serial.println("State 0, no motion"); 

      digitalWrite(StpPin1, LOW); 

      if (homeStep == 0 && autoCycling == 0 ){ 

        state = 0; 

      } 

      else{ 

        if (homeStep == 1 && extendto1 == 0 && autoCycling == 0){ 

          state = homingStep; 

        } 

        else if (homeStep == 0 &&extendto1 == 1 && autoCycling == 0){ 

          state = extend; 

        } 

        else if (homeStep == 0 && extendto1 == 0 && autoCycling == 1){ 

          state = autoCycle; 

        } 

      } 

      break; 

 

    case homingStep:// Homing sequence 

      extendedto1 = 0; 

      Serial.println("Homing Stepper State"); 

      digitalWrite(HomeLED, HIGH); 

      digitalWrite(autoLED, LOW); 

 

      if (homed == 0){ 

 

      digitalWrite(DirPin1, LOW);       

      for (int i = 0; i < homing; i++){ 

        Serial.println("retracting"); 

        digitalWrite(StpPin1, HIGH); 

        delayMicroseconds(800); 

        digitalWrite(StpPin1, LOW); 

        delayMicroseconds(800); 

      } 

      homed = 1; 

      } 

      if (homeStep == 1 && extendto1 == 0 && autoCycling == 0){ 

        Serial.println("Staying in home state"); 

        state = homingStep; 

      } 

      else if (homeStep == 0 && extendto1 == 0 && autoCycling == 0){ 

          Serial.println("Going to stop state"); 

          state = 0; 

        } 

      else if (homeStep == 0 && extendto1 == 0 && autoCycling == 1){ 

        Serial.println("Going to auto cycle"); 

        state = autoCycle; 

      } 

      else if (homeStep == 0 && extendto1 == 1 && autoCycling == 0){ 

        Serial.println("Going to extend state"); 

        state = extend; 

      } 

      break; 

 

    case autoCycle: 

      digitalWrite(HomeLED, LOW); 

      digitalWrite(autoLED, HIGH); 

      Serial.print(" Extended =  ");Serial.print(extendedto1); 

      Serial.println("Auto Cycle"); 

 

      digitalWrite(DirPin1, HIGH); 

      Serial.println("Extending"); 

      for (int i =0; i < strain1to10; i++){ 

        digitalWrite(StpPin1, HIGH); 

        delayMicroseconds(800); 

        digitalWrite(StpPin1, LOW); 

        delayMicroseconds(800); 

      } 

      delay(400); 

      digitalWrite(DirPin1, LOW); 

      Serial.println("Retracting"); 

      for (int i =0; i < strain1to10; i++){ 

        digitalWrite(StpPin1, HIGH); 

        delayMicroseconds(800); 

        digitalWrite(StpPin1, LOW); 

        delayMicroseconds(800); 

      } 

      delay(400); 

 

      if (homeStep == 0 && extendto1 == 0 && autoCycling == 1){ 

        Serial.println("Staying in auto cycle"); 

        state = autoCycle; 

      } 

      else if (homeStep == 0 && extendto1 == 0 && autoCycling == 0){ 

          Serial.println("Going to stop state"); 

          state = 0; 

        } 

      else if (homeStep == 1 && extendto1 == 0 && autoCycling == 0){ 

          Serial.println("Going to home state"); 

          state = homingStep; 

        } 

      break; 

 

    case extend:// extending to 1 sequence 

      digitalWrite(HomeLED, HIGH); 

      digitalWrite(autoLED, HIGH); 

      homed = 0; 

      if (extendedto1 == 0){ 

        Serial.print(" Extended =  ");Serial.print(extendedto1);Serial.println("Extending to 1 percent"); 

        digitalWrite(DirPin1, HIGH);   

        for (int i = 0; i < hometo1B; i++){ 

          Serial.println("Extending One"); 

          digitalWrite(StpPin1, HIGH); 

          delayMicroseconds(800); 

          digitalWrite(StpPin1, LOW); 

          delayMicroseconds(800); 

        } 

         

        extendedto1 = 1; 

      } 

      if (homeStep == 0 && extendto1 == 1 && autoCycling == 0){ 

        Serial.println("Staying in extend state"); 

        state = extend; 

      } 

      else{ 

        if (homeStep == 0 && extendto1 == 0 && autoCycling == 0){ 

          Serial.println("Going to stop state"); 

          state = 0; 

        } 

        else if (homeStep == 1 && extendto1 == 0 && autoCycling == 0){ 

          Serial.println("Going to home state"); 

          state = homingStep; 

        } 

        else if (homeStep == 0 && extendto1 == 0 && autoCycling == 1){ 

          Serial.println("Goimage.pnging to auto cycle"); 

          state = autoCycle; 

      } 

      break;} 

      }} 

 

void checkTime(){  

  rtc.refresh(); 

  Serial.print("TIME:  "); Serial.print(rtc.hour());Serial.print(" : "); Serial.print(rtc.minute());Serial.print(" : ");Serial.println(rtc.second()); 

  if (rtc.hour() == StartTime1 ||rtc.hour() == StartTime2 || rtc.hour() == StartTime3 ||rtc.hour() == StartTime4 ){ 

    if (rtc.minute() == 0 && rtc.second() < 10){ 

      Serial.println("Autocycling, extend to 1 percent"); 

      extendto1 = 1; 

      homeStep = 0; 

      autoCycling = 0; 

      state = extend; 

    } 

    else{ 

      Serial.println("Autocycling"); 

      extendto1 = 0; 

      homeStep = 0; 

      autoCycling = 1; 

      state = autoCycle; 

    } 

  } 

  else if (rtc.hour() == StopTime1 || rtc.hour() == StopTime2 || rtc.hour() == StopTime3 || rtc.hour() == StopTime4){ 

    if (rtc.minute() == 0 && rtc.second() < 10){ 

      Serial.println("stopping time"); 

      extendto1 = 0; 

      homeStep = 1; 

      autoCycling = 0; 

      state = homingStep; 

    } 

    else{ 

      extendto1 = 0; 

      homeStep = 0; 

      autoCycling = 0; 

      state = 0; 

    } 

   } 

  else if (rtc.hour() != StopTime1 && rtc.hour() != StopTime2 && rtc.hour() != StopTime3 && rtc.hour() != StopTime4 && rtc.hour() != StartTime1 && rtc.hour() != StartTime2 && rtc.hour() != StartTime3 && rtc.hour() != StartTime4){ 

    Serial.println("every other time"); 

    extendto1 = 0; 

    homeStep = 0; 

    autoCycling = 0; 

    state = 0; 

  } 

}  