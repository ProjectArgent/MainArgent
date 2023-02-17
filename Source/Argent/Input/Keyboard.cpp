#include "Keyboard.h"

Library::Input::Mouse::Mouse()
{
	mouseState[Mouses::mLeftButton] = State::NONE;
	mouseState[Mouses::mRightButton] = State::NONE;
	mouseState[Mouses::mMiddleButton] = State::NONE;
}

void Library::Input::Mouse::Update()
{
	POINT p{};
	GetCursorPos(&p);
	//ScreenToClient(Argent::Graphics::ArGraphics::Instance()->GetWindowHandle(), &p);
	postPosition = position;
	position = DirectX::XMFLOAT2( static_cast<float>(p.x), static_cast<float>(p.y) );
	moveVec = DirectX::XMFLOAT2(position.x - postPosition.x, position.y - postPosition.y);


	for(auto& m : mouseState)
	{
		const SHORT State = GetKeyState(static_cast<int>(m.first));
		if(State & 0x8000)
		{
			switch (m.second)
			{
			case State::NONE:
				m.second = State::PRESS_ENTER;
				break;
		
			case State::PRESS_ENTER:
			case State::PRESS:
				m.second = State::PRESS;
				break;
		
			case State::RELEASE:
				m.second = State::NONE;
				break;
			}
		}
		else
		{
			switch (m.second)
			{
			case State::NONE:
			case State::RELEASE:
				m.second = State::NONE;
				break;
		
			case State::PRESS:
			case State::PRESS_ENTER:
				m.second = State::RELEASE;
				break;
			}
		}
	}


	if(!isWheelRotate) wheelState = WheelStates::wNone;
	else if(rowWheelRotateValue > 0) wheelState = WheelStates::wPlus;
	else if(rowWheelRotateValue < 0) wheelState = WheelStates::wMinus;
	else wheelState = WheelStates::wNone;
}

bool Library::Input::Mouse::IsButtonPress(Mouses m)
{
	if(mouseState[m] == State::PRESS) return true;
	return false;
}

bool Library::Input::Mouse::IsButtonPressEneter(Mouses m)
{
	if(mouseState[m] == State::PRESS_ENTER) return true;
	return false;
}

bool Library::Input::Mouse::IsButtomRelease(Mouses m)
{
	if(mouseState[m] == State::RELEASE) return true;
	return false;
}

void Library::Input::Mouse::DrawDebug()
{
#ifdef _DEBUG
#endif
}
