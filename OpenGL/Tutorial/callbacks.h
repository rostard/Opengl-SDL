
#ifndef CALLBACKS_H
#define CALLBACKS_H

class ICallbacks
{
public:

	virtual void KeyboardUpCB(int Key) = 0;
	virtual void KeyboardDownCB(int Key) = 0;
	virtual void PassiveMouseCB(int x, int y) = 0;
	virtual void RenderSceneCB() = 0;
	virtual void MouseCB(int Button, int State, int x, int y) = 0;
	virtual ~ICallbacks() {};
};

#endif /* CALLBACKS_H */