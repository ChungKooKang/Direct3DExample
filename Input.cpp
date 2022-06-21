#include "Input.h"

namespace MyUtil

{
	void Input::Initialize()
	{
		mKeys.fill(false);
		mCursorX = mCursorY = 0;
	}
	void Input::SetKeyDown(unsigned int key)
	{
		mKeys[key] = true;
	}
	void Input::SetKeyUp(unsigned int key)
	{
		mKeys[key] = false;
	}
	void Input::SetCursor(int x, int y)
	{
		mCursorX = x;
		mCursorY = y;
	}
	bool Input::IsKeyDown(unsigned int key)
	{
		return mKeys[key];
	}
	void Input::GetCursorPosition(int& x, int& y)
	{
		x = mCursorX;
		y = mCursorY;
	}
}