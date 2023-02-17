#pragma once
#include <Windows.h>
#include <unordered_map>
#include <DirectXMath.h>


namespace Argent::Input
{
	enum class State
	{
		NONE,
		PRESS_ENTER,
		PRESS,
		RELEASE,
	};

	class Keyboard
	{
	public:
		enum Keys : unsigned char
		{
			None = 0,

			Back = 0x8,
			Tab = 0x9,

			Enter = 0xd,

			Pause = 0x13,
			CapsLock = 0x14,
			Kana = 0x15,
			ImeOn = 0x16,

			Kanji = 0x19,

			ImeOff = 0x1a,
			Escape = 0x1b,
			ImeConvert = 0x1c,
			ImeNoConvert = 0x1d,

			Space = 0x20,
			PageUp = 0x21,
			PageDown = 0x22,
			End = 0x23,
			Home = 0x24,
			Left = 0x25,
			Up = 0x26,
			Right = 0x27,
			Down = 0x28,
			Select = 0x29,
			Print = 0x2a,
			Execute = 0x2b,
			PrintScreen = 0x2c,
			Insert = 0x2d,
			Delete = 0x2e,
			Help = 0x2f,
			D0 = 0x30,
			D1 = 0x31,
			D2 = 0x32,
			D3 = 0x33,
			D4 = 0x34,
			D5 = 0x35,
			D6 = 0x36,
			D7 = 0x37,
			D8 = 0x38,
			D9 = 0x39,

			A = 0x41,
			B = 0x42,
			C = 0x43,
			D = 0x44,
			E = 0x45,
			F = 0x46,
			G = 0x47,
			H = 0x48,
			I = 0x49,
			J = 0x4a,
			K = 0x4b,
			L = 0x4c,
			M = 0x4d,
			N = 0x4e,
			O = 0x4f,
			P = 0x50,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			T = 0x54,
			U = 0x55,
			V = 0x56,
			W = 0x57,
			X = 0x58,
			Y = 0x59,
			Z = 0x5a,
			LeftWindows = 0x5b,
			RightWindows = 0x5c,
			Apps = 0x5d,

			Sleep = 0x5f,
			NumPad0 = 0x60,
			NumPad1 = 0x61,
			NumPad2 = 0x62,
			NumPad3 = 0x63,
			NumPad4 = 0x64,
			NumPad5 = 0x65,
			NumPad6 = 0x66,
			NumPad7 = 0x67,
			NumPad8 = 0x68,
			NumPad9 = 0x69,
			Multiply = 0x6a,
			Add = 0x6b,
			Separator = 0x6c,
			Subtract = 0x6d,

			Decimal = 0x6e,
			Divide = 0x6f,
			F1 = 0x70,
			F2 = 0x71,
			F3 = 0x72,
			F4 = 0x73,
			F5 = 0x74,
			F6 = 0x75,
			F7 = 0x76,
			F8 = 0x77,
			F9 = 0x78,
			F10 = 0x79,
			F11 = 0x7a,
			F12 = 0x7b,
			F13 = 0x7c,
			F14 = 0x7d,
			F15 = 0x7e,
			F16 = 0x7f,
			F17 = 0x80,
			F18 = 0x81,
			F19 = 0x82,
			F20 = 0x83,
			F21 = 0x84,
			F22 = 0x85,
			F23 = 0x86,
			F24 = 0x87,

			NumLock = 0x90,
			Scroll = 0x91,

			LeftShift = 0xa0,
			RightShift = 0xa1,
			LeftControl = 0xa2,
			RightControl = 0xa3,
			LeftAlt = 0xa4,
			RightAlt = 0xa5,
			BrowserBack = 0xa6,
			BrowserForward = 0xa7,
			BrowserRefresh = 0xa8,
			BrowserStop = 0xa9,
			BrowserSearch = 0xaa,
			BrowserFavorites = 0xab,
			BrowserHome = 0xac,
			VolumeMute = 0xad,
			VolumeDown = 0xae,
			VolumeUp = 0xaf,
			MediaNextTrack = 0xb0,
			MediaPreviousTrack = 0xb1,
			MediaStop = 0xb2,
			MediaPlayPause = 0xb3,
			LaunchMail = 0xb4,
			SelectMedia = 0xb5,
			LaunchApplication1 = 0xb6,
			LaunchApplication2 = 0xb7,

			OemSemicolon = 0xba,
			OemPlus = 0xbb,
			OemComma = 0xbc,
			OemMinus = 0xbd,
			OemPeriod = 0xbe,
			OemQuestion = 0xbf,
			OemTilde = 0xc0,

			OemOpenBrackets = 0xdb,
			OemPipe = 0xdc,
			OemCloseBrackets = 0xdd,
			OemQuotes = 0xde,
			Oem8 = 0xdf,

			OemBackslash = 0xe2,

			ProcessKey = 0xe5,

			OemCopy = 0xf2,
			OemAuto = 0xf3,
			OemEnlW = 0xf4,

			Attn = 0xf6,
			Crsel = 0xf7,
			Exsel = 0xf8,
			EraseEof = 0xf9,
			Play = 0xfa,
			Zoom = 0xfb,

			Pa1 = 0xfd,
			OemClear = 0xfe,
		};

		Keyboard(const Keyboard&) = delete;
		Keyboard operator=(Keyboard&) = delete;

		static Keyboard& Instance()
		{
			static Keyboard instance;
			return instance;
		}
	private:

		Keyboard()
		{
			keyState[Keys::None					] = State::NONE;
			keyState[Keys::Back					] = State::NONE;
			keyState[Keys::Tab					] = State::NONE;
			keyState[Keys::Enter				] = State::NONE;
			keyState[Keys::Pause				] = State::NONE;
			keyState[Keys::CapsLock				] = State::NONE;
			keyState[Keys::Kana					] = State::NONE;
			keyState[Keys::ImeOn				] = State::NONE;
			keyState[Keys::Kanji				] = State::NONE;
			keyState[Keys::ImeOff				] = State::NONE;
			keyState[Keys::Escape				] = State::NONE;
			keyState[Keys::ImeConvert			] = State::NONE;
			keyState[Keys::ImeNoConvert			] = State::NONE;
			keyState[Keys::Space				] = State::NONE;
			keyState[Keys::PageUp				] = State::NONE;
			keyState[Keys::PageDown				] = State::NONE;
			keyState[Keys::End					] = State::NONE;
			keyState[Keys::Home					] = State::NONE;
			keyState[Keys::Left					] = State::NONE;
			keyState[Keys::Up					] = State::NONE;
			keyState[Keys::Right				] = State::NONE;
			keyState[Keys::Down					] = State::NONE;
			keyState[Keys::Select				] = State::NONE;
			keyState[Keys::Print				] = State::NONE;
			keyState[Keys::Execute				] = State::NONE;
			keyState[Keys::PrintScreen			] = State::NONE;
			keyState[Keys::Insert				] = State::NONE;
			keyState[Keys::Delete				] = State::NONE;
			keyState[Keys::Help					] = State::NONE;
			keyState[Keys::D0					] = State::NONE;
			keyState[Keys::D1					] = State::NONE;
			keyState[Keys::D2					] = State::NONE;
			keyState[Keys::D3					] = State::NONE;
			keyState[Keys::D4					] = State::NONE;
			keyState[Keys::D5					] = State::NONE;
			keyState[Keys::D6					] = State::NONE;
			keyState[Keys::D7					] = State::NONE;
			keyState[Keys::D8					] = State::NONE;
			keyState[Keys::D9					] = State::NONE;
			keyState[Keys::A					] = State::NONE;
			keyState[Keys::B					] = State::NONE;
			keyState[Keys::C					] = State::NONE;
			keyState[Keys::D					] = State::NONE;
			keyState[Keys::E					] = State::NONE;
			keyState[Keys::F					] = State::NONE;
			keyState[Keys::G					] = State::NONE;
			keyState[Keys::H					] = State::NONE;
			keyState[Keys::I					] = State::NONE;
			keyState[Keys::J					] = State::NONE;
			keyState[Keys::K					] = State::NONE;
			keyState[Keys::L					] = State::NONE;
			keyState[Keys::M					] = State::NONE;
			keyState[Keys::N					] = State::NONE;
			keyState[Keys::O					] = State::NONE;
			keyState[Keys::P					] = State::NONE;
			keyState[Keys::Q					] = State::NONE;
			keyState[Keys::R					] = State::NONE;
			keyState[Keys::S					] = State::NONE;
			keyState[Keys::T					] = State::NONE;
			keyState[Keys::U					] = State::NONE;
			keyState[Keys::V					] = State::NONE;
			keyState[Keys::W					] = State::NONE;
			keyState[Keys::X					] = State::NONE;
			keyState[Keys::Y					] = State::NONE;
			keyState[Keys::Z					] = State::NONE;
			keyState[Keys::LeftWindows			] = State::NONE;
			keyState[Keys::RightWindows			] = State::NONE;
			keyState[Keys::Apps					] = State::NONE;
			keyState[Keys::Sleep				] = State::NONE;
			keyState[Keys::NumPad0				] = State::NONE;
			keyState[Keys::NumPad1				] = State::NONE;
			keyState[Keys::NumPad2				] = State::NONE;
			keyState[Keys::NumPad3				] = State::NONE;
			keyState[Keys::NumPad4				] = State::NONE;
			keyState[Keys::NumPad5				] = State::NONE;
			keyState[Keys::NumPad6				] = State::NONE;
			keyState[Keys::NumPad7				] = State::NONE;
			keyState[Keys::NumPad8				] = State::NONE;
			keyState[Keys::NumPad9				] = State::NONE;
			keyState[Keys::Multiply				] = State::NONE;
			keyState[Keys::Add					] = State::NONE;
			keyState[Keys::Separator			] = State::NONE;
			keyState[Keys::Subtract				] = State::NONE;
			keyState[Keys::Decimal				] = State::NONE;
			keyState[Keys::Divide				] = State::NONE;
			keyState[Keys::F1					] = State::NONE;
			keyState[Keys::F2					] = State::NONE;
			keyState[Keys::F3					] = State::NONE;
			keyState[Keys::F4					] = State::NONE;
			keyState[Keys::F5					] = State::NONE;
			keyState[Keys::F6					] = State::NONE;
			keyState[Keys::F7					] = State::NONE;
			keyState[Keys::F8					] = State::NONE;
			keyState[Keys::F9					] = State::NONE;
			keyState[Keys::F10					] = State::NONE;
			keyState[Keys::F11					] = State::NONE;
			keyState[Keys::F12					] = State::NONE;
			keyState[Keys::F13					] = State::NONE;
			keyState[Keys::F14					] = State::NONE;
			keyState[Keys::F15					] = State::NONE;
			keyState[Keys::F16					] = State::NONE;
			keyState[Keys::F17					] = State::NONE;
			keyState[Keys::F18					] = State::NONE;
			keyState[Keys::F19					] = State::NONE;
			keyState[Keys::F20					] = State::NONE;
			keyState[Keys::F21					] = State::NONE;
			keyState[Keys::F22					] = State::NONE;
			keyState[Keys::F23					] = State::NONE;
			keyState[Keys::F24					] = State::NONE;
			keyState[Keys::NumLock				] = State::NONE;
			keyState[Keys::Scroll				] = State::NONE;
			keyState[Keys::LeftShift			] = State::NONE;
			keyState[Keys::RightShift			] = State::NONE;
			keyState[Keys::LeftControl			] = State::NONE;
			keyState[Keys::RightControl			] = State::NONE;
			keyState[Keys::LeftAlt				] = State::NONE;
			keyState[Keys::RightAlt				] = State::NONE;
			keyState[Keys::BrowserBack			] = State::NONE;
			keyState[Keys::BrowserForward		] = State::NONE;
			keyState[Keys::BrowserRefresh		] = State::NONE;
			keyState[Keys::BrowserStop			] = State::NONE;
			keyState[Keys::BrowserSearch		] = State::NONE;
			keyState[Keys::BrowserFavorites		] = State::NONE;
			keyState[Keys::BrowserHome			] = State::NONE;
			keyState[Keys::VolumeMute			] = State::NONE;
			keyState[Keys::VolumeDown			] = State::NONE;
			keyState[Keys::VolumeUp				] = State::NONE;
			keyState[Keys::MediaNextTrack		] = State::NONE;
			keyState[Keys::MediaPreviousTrack	] = State::NONE;
			keyState[Keys::MediaStop			] = State::NONE;
			keyState[Keys::MediaPlayPause		] = State::NONE;
			keyState[Keys::LaunchMail			] = State::NONE;
			keyState[Keys::SelectMedia			] = State::NONE;
			keyState[Keys::LaunchApplication1	] = State::NONE;
			keyState[Keys::LaunchApplication2	] = State::NONE;
			keyState[Keys::OemSemicolon			] = State::NONE;
			keyState[Keys::OemPlus				] = State::NONE;
			keyState[Keys::OemComma				] = State::NONE;
			keyState[Keys::OemMinus				] = State::NONE;
			keyState[Keys::OemPeriod			] = State::NONE;
			keyState[Keys::OemQuestion			] = State::NONE;
			keyState[Keys::OemTilde				] = State::NONE;
			keyState[Keys::OemOpenBrackets		] = State::NONE;
			keyState[Keys::OemPipe				] = State::NONE;
			keyState[Keys::OemCloseBrackets		] = State::NONE;
			keyState[Keys::OemQuotes			] = State::NONE;
			keyState[Keys::Oem8					] = State::NONE;
			keyState[Keys::OemBackslash			] = State::NONE;
			keyState[Keys::ProcessKey			] = State::NONE;
			keyState[Keys::OemCopy				] = State::NONE;
			keyState[Keys::OemAuto				] = State::NONE;
			keyState[Keys::OemEnlW				] = State::NONE;
			keyState[Keys::Attn					] = State::NONE;
			keyState[Keys::Crsel				] = State::NONE;
			keyState[Keys::Exsel				] = State::NONE;
			keyState[Keys::EraseEof				] = State::NONE;
			keyState[Keys::Play					] = State::NONE;
			keyState[Keys::Zoom					] = State::NONE;
			keyState[Keys::Pa1					] = State::NONE;
			keyState[Keys::OemClear				] = State::NONE;
		}
			
		~Keyboard() = default;


	public:

		void Update()
		{
			UpdateKeyStates();
		}

		void Clear()
		{
			for(auto it = keyState.begin(); it != keyState.end(); ++it)
			{
				(*it).second = State::NONE;
			}
		}


		bool IsKeyPress(Keys key)
		{
			return keyState.find(key)->second == State::PRESS;
		}
		bool IsKeyRelease(Keys key)
		{
			return keyState.find(key)->second == State::RELEASE;
		}

		bool IsKeyPressEnter(Keys key)
		{
			return keyState.find(key)->second == State::PRESS_ENTER;
		}

		void UpdateKeyStates()
		{
			for(auto it = keyState.begin(); it != keyState.end(); ++it)
			{
				const SHORT State = GetKeyState((*it).first);
				if(State & 0x8000)
				{
					switch ((*it).second)
					{
					case State::NONE:
						(*it).second = State::PRESS_ENTER;
						break;

					case State::PRESS_ENTER:
					case State::PRESS:
						(*it).second = State::PRESS;
						break;

					case State::RELEASE:
						(*it).second = State::NONE;
						break;
					}
				}
				else
				{
					switch ((*it).second)
					{
					case State::NONE:
					case State::RELEASE:
						(*it).second = State::NONE;
						break;

					case State::PRESS:
					case State::PRESS_ENTER:
						(*it).second = State::RELEASE;
						break;
					}
				}
			}
		}

		std::unordered_map<Keys, State> keyState;
	};

	class Mouse
	{
	public:
		enum class Mouses
		{
			mLeftButton = 0x01,
			mRightButton = 0x02,
			mMiddleButton = 0x04,
		};

		enum class WheelStates
		{
			wNone,
			wPlus,
			wMinus
		};

	private:
		Mouse();
		~Mouse() = default;

	public:
		static Mouse& Instance()
		{
			static Mouse instance;
			return instance;
		}


		void Update();
		bool IsButtonPress(Mouses m);
		bool IsButtonPressEneter(Mouses m);
		bool IsButtomRelease(Mouses m);
		bool IsMouseWheelRotate() const { return isWheelRotate; }
	
		void SetIsWheelRotateOn() { isWheelRotate = true; }
		void SetIsWheelRotateOff() { isWheelRotate = false; }
		void SetRowWheelRotateValue(float v) { rowWheelRotateValue = v; }

		[[nodiscard]] DirectX::XMFLOAT2 GetPosition() const { return position; }
		[[nodiscard]] DirectX::XMFLOAT2 GetMoveVec() const { return moveVec; }
		[[nodiscard]] DirectX::XMFLOAT2 GetPostPosition() const { return postPosition; }
		[[nodiscard]] WheelStates GetWheelState() const { return wheelState; }

		float GetRowWheelRotateValue() const { return rowWheelRotateValue; }

		std::unordered_map<Mouses, State> mouseState;
	private:
		DirectX::XMFLOAT2 position;
		DirectX::XMFLOAT2 moveVec;
		DirectX::XMFLOAT2 postPosition;

		bool isWheelRotate;
		float rowWheelRotateValue;
		WheelStates wheelState;
	};
}
