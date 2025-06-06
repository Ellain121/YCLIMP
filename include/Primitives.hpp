#pragma once

#include <ncurses.h>
#undef OK

template <typename T>
struct Vector2
{
public:
	Vector2(T x, T y)
		: x(x)
		, y(y)
	{
	}

public:
	T x;
	T y;
};

template <typename T>
struct Rect
{
public:
	Rect() = default;
	Rect(T left, T top, T width, T height)
		: left(left)
		, top(top)
		, width(width)
		, height(height)
	{
	}

public:
	T left;
	T top;
	T width;
	T height;
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Rectf = Rect<float>;
using Recti = Rect<int>;

struct Time
{
	Time(int sec);
	Time(int min, int sec);
	int min;
	int sec;
};

enum class EEventKey
{
	Unknown = -1,
	A = 'a',
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	EnumEnd,
	KeyResize = KEY_RESIZE
};

struct Event
{
public:
	Event(int eventKey);

public:
	EEventKey key;

	int GetRawKey() const;

private:
	int rawKey;
};

bool isEventKeyValid(EEventKey eventKey);
bool isEventKeyValid(int eventKey);
bool isEventValid(const Event& event);
