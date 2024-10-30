
#ifndef BUTTONLED_H
#define BUTTONLED_H

class I_Led {
public:
	virtual void switchLightOn() = 0;
	virtual void switchLightOff() = 0;

	virtual ~I_Led();
};


#endif /*BUTTONLED_H*/
