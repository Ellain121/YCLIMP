#include "Primitives.hpp"

#include <iostream>

Time::Time(int totalSec)
{
	min = totalSec / 60;
	sec = totalSec % 60;
}

Time::Time(int min, int sec)
	: min{min}
	, sec{sec}
{
}

Event::Event(int eventKey)
{
	key = EEventKey(eventKey);
	rawKey = eventKey;
}

int Event::GetRawKey() const
{
	return rawKey;
}

bool isEventKeyValid(EEventKey eventKey)
{
	return eventKey != EEventKey::Unknown;
}

bool isEventKeyValid(int eventKey)
{
	return isEventKeyValid(static_cast<EEventKey>(eventKey));
}

bool isEventValid(const Event& event)
{
	return isEventKeyValid(event.key);
}
