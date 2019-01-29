#pragma once
#include <list>
#include <map>
#include <typeindex>
#include "memberFunctionHandler.h"
#include "Events.h"


typedef std::list<HandlerFunctionBase*> HandlerList;
class EventBus
{
public:
	template<typename EventType>
	void publish(EventType * evnt);

	template<class T, class EventType>
	void subscribe(T * instance, void (T::*memberFunction)(EventType *));

	static EventBus& get();

private:
	std::map<std::type_index, HandlerList*> subscribers;
};



template<typename EventType>
inline void EventBus::publish(EventType * evnt)
{
	//Get all subscribed handlers associated with the current event
	HandlerList* handlers = subscribers[typeid(EventType)];

	if (handlers == nullptr)
	{
		return;
	}

	for (auto & handler : *handlers)
	{
		if (handler != nullptr)
		{
			handler->exec(evnt);
		}
	}
}

template<class T, class EventType>
inline void EventBus::subscribe(T * instance, void(T::* memberFunction)(EventType *))
{
	//Get all subscribed handlers associated with the current event
	HandlerList* handlers = subscribers[typeid(EventType)];

	//If there are no handlers for the current EventType, create one
	if (handlers == nullptr)
	{
		handlers = new HandlerList();
		subscribers[typeid(EventType)] = handlers;
	}

	handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
}

inline EventBus& EventBus::get()
{
	static EventBus instance;
	return instance;
}