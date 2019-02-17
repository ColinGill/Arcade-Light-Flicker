/*
 Arcade Light Flicker 
 Date: 10 -2 -19
 Author: Colin Gill

 This Program requires 3 additional conponents: A SW-420 Vibration Sensor, a Solid State Relay Module Board, and 12V Led Strip
 Hook output of SW-420 to digital pin 7 on arduino, and wire input of SS relay to Digital pin 13. 
 The Led Strip will have to be connected to the hot side of Relay module with an AND gate setup for positive lead of a 12V supply. 
 
*/

#define Idle 0
#define EnableTimer 1
#define Flashing 2

int state , ledStrip = 13 , sensorValue = 0 , sensorPin = 7, contact = 0, tickCnt = 0;
long randNumber_FlashOnOff, randNumber_HitsToReset , randLightOnTime;

void setup() {

  pinMode(ledStrip, OUTPUT);
  pinMode(sensorPin, INPUT);
  randomSeed(analogRead(0));

  //set timer1 interrupt at 100Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 156;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  state = Idle;
}

// the loop function runs over and over again forever
void loop() {

  switch (state) {

    case Idle:
    
      randLightOnTime = random(600000,1200000); //Generate random number
      digitalWrite(ledStrip, HIGH); //hold LEDs high
      delay(randLightOnTime); //For this amount of time 
      state = EnableTimer  
      break;

    case EnableTimer:

      sei();//allow interrupts    
      contact = 0;
      randNumber_HitsToReset = random(1, 2); //randomly assign how many hits it will take to reset light
      state = Flashing;
      break;

    case Flashing:

      randNumber_FlashOnOff = random(50, 200); //add randomness to flicker

      if (contact > randNumber_HitsToReset) 
      {
        state = Idle;
      }

      else
      {
        digitalWrite(ledStrip, HIGH);
        delay(randNumber_FlashOnOff);
        digitalWrite(ledStrip, LOW);
        delay(40);
      }
      break;
  }
}

ISR(TIMER1_COMPA_vect) {


  //read sensor
  sensorValue =  digitalRead(sensorPin);
  if (sensorValue == HIGH && contact == 0)
  {
    contact ++;
  }

  else if (contact > 0)
  {
    tickCnt ++;
    if (tickCnt >= 50) //wait for sensor to settle as to not detect false hits on side panel
    {
      tickCnt = 50; //prevent overflow, cap at 50
      if (sensorValue == HIGH)
      {
        contact ++;
        tickCnt = 0;
      }
    }
  }
}

