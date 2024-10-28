
#ifndef BUTTONLED_H
#define BUTTONLED_H

class I_ButtonLED {
public:
	virtual void switchLightOn() = 0;
	virtual void switchLightOff() = 0;

	virtual ~I_ButtonLED();
};


#endif /*BUTTONLED_H*/
