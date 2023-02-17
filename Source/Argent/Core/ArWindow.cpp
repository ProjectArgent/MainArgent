#include "ArWindow.h"
#include "../Other/Misc.h"

namespace Argent::Window
{
	ArWindow::ArWindow(HINSTANCE hInstance, LONG width, LONG height)
	{
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.lpszClassName = L"Dx12 Third";


		if( !RegisterClassExW(&wc)) _ASSERT_EXPR(FALSE, L"Window is Already Registered");


		RECT rc{ 0, 0, width, height };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		hWnd = CreateWindowExW(0, wc.lpszClassName, L"Dx12", 
		                       WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE,
		                       CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, 
		                       rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
		ShowWindow(hWnd, SW_SHOW);

		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}
}
