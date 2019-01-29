#pragma once

class State
{
public:
	State();
	virtual ~State();

	virtual void start() {};
	virtual void end() {};
	virtual void update(float dt) {};
	virtual void updateLogic() {};
	virtual void render() {};

private:

};