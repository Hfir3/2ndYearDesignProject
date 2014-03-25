//#include <uSTimer2.h>

#include <avr/interrupt.h>
#define USECS 200

volatile unsigned long TimerCount;
volatile char TimerOverflowing;
volatile unsigned int TimerPreload;

////////////////////////////////////ENCODERS//////////////////////////////////////
//Pin connections
const int left_enc_chA = 2;
const int left_enc_chB = 3;
const int right_enc_chA = 4;
const int right_enc_chB = 5;

//Current encoder value
long left_enc_val = 0;
long right_enc_val = 0;

//Time in (ms) between last two encoder counts, can be used for velocity
unsigned long left_enc_time = 0;
unsigned long right_enc_time = 0;

//Private use
int left_enc_state = 0;
int left_enc_stateA = 0;
int left_enc_stateB = 0;
int right_enc_state = 0;
int right_enc_stateA = 0;
int right_enc_stateB = 0;
int left_enc_pos = 0;
int left_enc_neg = 0;
int right_enc_pos = 0;
int right_enc_neg = 0;
unsigned long left_enc_prevtime = 0;
unsigned long right_enc_prevtime = 0;
//////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(115200);
  setupTimer();
  startTimer2();
}

void loop() {
  
  Serial.print(left_enc_val);
  Serial.print( "      ");
  Serial.println(right_enc_val); 
}

void setupTimer() {
	// Setup Timer2
	
	float prescaler = 0.0;
	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {	// prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if (F_CPU < 1000000UL) {	// prescaler set to 8
		TCCR2B |= (1<<CS21);
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	} else { // F_CPU > 16Mhz, prescaler set to 128
		TCCR2B |= ((1<<CS22) | (1<<CS20));
		TCCR2B &= ~(1<<CS21);
		prescaler = 128.0;
	}
	TimerPreload = 256 - (int)((float)F_CPU * 0.00002795 / prescaler);
        setupEncoders();
        startTimer2();
}

void startTimer2() {
	// Start Timer2
	TimerCount = 0;
	TimerOverflowing = 0;
	TCNT2 = TimerPreload;
	TIMSK2 |= (1<<TOIE2);
}

void stopTimer2() {
	// Stop Timer2
	TIMSK2 &= ~(1<<TOIE2);
}

void TimerOverflow() {
	TimerCount += 1;
	if (TimerCount >= USECS && !TimerOverflowing) {
		TimerOverflowing = 1;
		TimerCount = 0;
		
		//INSERT ALL ENCODER CODE HERE
		checkEncoders();
		TimerOverflowing = 0;
	}
}

ISR(TIMER2_OVF_vect) {
	TCNT2 = TimerPreload;
	TimerOverflow();
}

void setupEncoders()
{
    pinMode(left_enc_chA, INPUT);
    pinMode(left_enc_chB, INPUT);
    pinMode(right_enc_chA, INPUT);
    pinMode(right_enc_chB, INPUT);

    left_enc_stateA = digitalRead(left_enc_chA) & 1;
    left_enc_stateB = digitalRead(left_enc_chB) & 1;
    right_enc_stateA = digitalRead(right_enc_chA) & 1;
    right_enc_stateB = digitalRead(right_enc_chB) & 1;

    left_enc_pos = left_enc_neg = (left_enc_stateB << 1) | left_enc_stateA;
    right_enc_pos = right_enc_neg = (right_enc_stateB << 1) | right_enc_stateA;
    left_enc_pos = left_enc_pos ^ B10;
    left_enc_neg = left_enc_neg ^ B01;
    right_enc_pos = right_enc_pos ^ B10;
    right_enc_neg = right_enc_neg ^ B01;
    left_enc_prevtime = right_enc_prevtime = millis();
}

void checkEncoders()
{
    bool updateLeft = false, updateRight = false;

    left_enc_stateA = digitalRead(left_enc_chA) & 1;
    left_enc_stateB = digitalRead(left_enc_chB) & 1;
    right_enc_stateA = digitalRead(right_enc_chA) & 1;
    right_enc_stateB = digitalRead(right_enc_chB) & 1;

    left_enc_state = (left_enc_stateA << 1) | left_enc_stateB;
    right_enc_state = (right_enc_stateA << 1) | right_enc_stateB;

    if(left_enc_state == left_enc_pos)
    {
        left_enc_val++;
        updateLeft = true;
    }
    if(left_enc_state == left_enc_neg)
    {
        left_enc_val--;
        updateLeft = true;
    }
    if(right_enc_state == right_enc_pos)
    {
        right_enc_val++;
        updateRight = true;
    }
    if(right_enc_state == right_enc_neg)
    {
        right_enc_val--;
        updateRight = true;
    }

    if(updateLeft)
    {
        left_enc_time = millis() - left_enc_prevtime;
        left_enc_prevtime = millis();
        left_enc_pos = left_enc_neg = (left_enc_stateB << 1) | left_enc_stateA;
        left_enc_pos = left_enc_pos ^ B10;
        left_enc_neg = left_enc_neg ^ B01;
    }

    if(updateRight)
    {
        right_enc_time = millis() - right_enc_prevtime;
        right_enc_prevtime = millis();
        right_enc_pos = right_enc_neg = (right_enc_stateB << 1) | right_enc_stateA;
        right_enc_pos = right_enc_pos ^ B10;
        right_enc_neg = right_enc_neg ^ B01;
    }
}

