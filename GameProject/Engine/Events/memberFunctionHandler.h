#pragma once
#include "Events.h"

//In order to have different MemberFunctionHandlers in a list we use a base class to use in the list instead
class HandlerFunctionBase
{
public:
	void exec(Event * evnt)
	{
		call(evnt);
	}

	unsigned int id{0};
private:
	virtual void call(Event * evnt) = 0;
};



template<class T, class EventType>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
	typedef void (T::*MemberFunction)(EventType*);

	//Constructor
	MemberFunctionHandler(T * instance, MemberFunction memberFunction) : instance{ instance }, memberFunction{ memberFunction }{};

	//Function that is called to call the assigned function
	void call(Event * evnt)
	{
		//Cast event to the correct type and call member function
		(instance->*memberFunction)(static_cast<EventType*>(evnt));
	}

private:
	//Pointer to class instance
	T * instance;

	//Pointer to member function
	MemberFunction memberFunction;
};