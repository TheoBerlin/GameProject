#pragma once

// TO SUBSCRIBE TO THE EVENTBUS
// EventBus::get().subscribe(this, &ClassName::functionName);
// FUNCTION NEEDS TO HAVE THIS STRUCTURE
// functionName(EventType * evnt)

// TO PUBLISH TO THE EVENTBUS
// EventBus::get().publish(new EventType(param1, param2));

// THE SYNTAX FOR UNSUBSCRIBE IS THE SAME AS FOR SUBSCRIBE

/// Class to inherit from
class Event
{
};

struct KeyEvent : public Event
{
	KeyEvent(int key, int action) : key{ key }, action{ action } {};
	int key;
	int action;
};

struct WindowResizeEvent : public Event
{
	WindowResizeEvent(int width, int height) : width{ width }, height{ height } {};
	int width;
	int height;
};

struct MouseMoveEvent : public Event
{
	MouseMoveEvent(unsigned int posX, unsigned int posY, int deltaX, int deltaY) : posX{ posX }, posY{ posY }, deltaX{ deltaX }, deltaY{ deltaY } {};
	unsigned int posX;
	unsigned int posY;
	int deltaX;
	int deltaY;
};

struct MouseClickEvent : public Event
{
	MouseClickEvent(int button, int action) : button{ button }, action{ action } {};
	int button;
	int action;
};

struct MouseScrollEvent : public Event
{
	MouseScrollEvent(int xoffset, int yoffset) : xoffset{ xoffset }, yoffset{ yoffset } {};
	int xoffset;
	int yoffset;
};
