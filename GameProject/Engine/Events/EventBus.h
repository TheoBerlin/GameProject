#pragma once
#include <list>
#include <map>
#include <typeindex>
#include "memberFunctionHandler.h"
#include "Events.h"

// TO SUBSCRIBE TO THE EVENTBUS
// EventBus::get().subscribe(this, &ClassName::functionName);
// FUNCTION NEEDS TO HAVE THIS STRUCTURE
// functionName(EventType * evnt)

// TO PUBLISH TO THE EVENTBUS
// EventBus::get().publish(&EventType(param1, param2));

// UNSUBSCRIBE IS THE SAME AS SUBCRIBE


typedef std::list<HandlerFunctionBase*> HandlerList;
class EventBus
{
public:
	// Publish to the EventBus using an event from Events.h
	template<typename EventType>
	void publish(EventType * evnt);

	// Subscribe to the EventBus, see Events.h or EventBus.h for more info on structure
	template<class T, class EventType>
	void subscribe(T * instance, void (T::*memberFunction)(EventType *));

	// Unsubscribe to the EventBus, see Events.h or EventBus.h for more info on structure
	template<class T, class EventType>
	void unsubscribe(T * instance, void (T::*memberFunction)(EventType *));

	void unsubscribeAll();

	// Get the single instance of the EventBus in order to subscribe/publish
	static EventBus& get();

	// Only called when program exits
	~EventBus();

private:
	template<class T, class EventType>
	static unsigned getID(T* instance);
	
	std::map<std::type_index, HandlerList*> subscribers;
};


// Publish to the EventBus using an event from Events.h
template<typename EventType>
inline void EventBus::publish(EventType * evnt)
{
	//Get all subscribed handlers associated with the current event
	HandlerList* handlers = subscribers[typeid(EventType)];

	if (handlers == nullptr)
	{
		return;
	}


	bool shouldBreak = false;
	for (auto it = handlers->begin(); it != handlers->end(); it++)
	{
		while ((*it) == nullptr)
		{
			it = handlers->erase(it);
			if (it == handlers->end())
			{
				shouldBreak = true;
				break;
			}
		}

		if (shouldBreak)
			break;

		(*it)->exec(evnt);
	}
}

// Subscribe to the EventBus, see Events.h or EventBus.h for more info on structure
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

	HandlerFunctionBase* temp = new MemberFunctionHandler<T, EventType>(instance, memberFunction);
	temp->id = getID<T, EventType>(instance);
	handlers->push_back(temp);
}

// Unsubscribe to the EventBus, see Events.h or EventBus.h for more info on structure
template<class T, class EventType>
inline void EventBus::unsubscribe(T * instance, void(T::* memberFunction)(EventType *))
{
	if(this->subscribers.find(typeid(EventType)) != this->subscribers.end())
	{
		HandlerList* handlers = this->subscribers[typeid(EventType)];

		unsigned id = getID<T, EventType>(instance);
		std::list<HandlerFunctionBase*>::iterator it;
		for (it = handlers->begin(); it != handlers->end(); ++it)
		{
			if ((*it) == nullptr)
				break;
			if ((*it)->id == id)
			{
				delete *it;
				(*it) = nullptr;
				break;
			}
		}
	}
}

template<class T, class EventType>
inline unsigned EventBus::getID(T * instance)
{
	size_t h = typeid(EventType).hash_code();
	return (unsigned)(h - (size_t)instance);
}

inline void EventBus::unsubscribeAll()
{
	for (auto const& i : this->subscribers)
	{
		HandlerList* handlers = i.second;
		if (handlers != nullptr)
		{
			std::list<HandlerFunctionBase*>::iterator it;
			for (it = handlers->begin(); it != handlers->end(); ++it)
				delete *it;
		}
		delete handlers;
	}
	this->subscribers.clear();
}

// Get the single instance of the EventBus in order to subscribe/publish
inline EventBus& EventBus::get()
{
	static EventBus instance;
	return instance;
}

inline EventBus::~EventBus()
{
	unsubscribeAll();
}
