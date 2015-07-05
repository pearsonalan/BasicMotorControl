#include <Arduino.h>

// This class implements driving logic for a stepper motor. This can be used
// to operate a motor like the 28BYJ-48 using a transistor array such as
// a ULN2003a or ULN2803a
class Motor {
private:
	int pin1;
	int pin2;
	int pin3;
	int pin4;

	int dir;
	int step;

public:
	Motor(int d, int p1, int p2, int p3, int p4) :
		pin1(p1), pin2(p2), pin3(p3), pin4(p4),
		dir(d), step(0)
	{
		pinMode(pin1, OUTPUT);
		pinMode(pin2, OUTPUT);
		pinMode(pin3, OUTPUT);
		pinMode(pin4, OUTPUT);
	}

	void Step();
};

Motor *m = NULL;

void setup() {
	// put your setup code here, to run once:
	m = new Motor(-1, 8, 9, 10, 11);
}

void loop() {
	m->Step();
	delayMicroseconds(4000);
}

void Motor::Step() {
	step = step + dir;

	if (step == 4)  step = 0;
	if (step == -1) step = 3;

	switch (step) {
	case 0:  // 1010
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, LOW);
		digitalWrite(pin3, HIGH);
		digitalWrite(pin4, HIGH);
		break;
	case 1:    // 0110
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, HIGH);
		digitalWrite(pin3, HIGH);
		digitalWrite(pin4, LOW);
		break;
	case 2:    //0101
		digitalWrite(pin1, HIGH);
		digitalWrite(pin2, HIGH);
		digitalWrite(pin3, LOW);
		digitalWrite(pin4, LOW);
		break;
	case 3:    //1001
		digitalWrite(pin1, HIGH);
		digitalWrite(pin2, LOW);
		digitalWrite(pin3, LOW);
		digitalWrite(pin4, HIGH);
		break;
	} 
}

