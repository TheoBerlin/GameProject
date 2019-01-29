#pragma once

class Event
{
};

struct KeyEvent : public Event
{
	KeyEvent(int key, int action) : key{ key }, action{ action } {};
	int key;
	int action;
};