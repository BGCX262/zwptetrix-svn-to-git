#include "stdafx.h"
#include "resource.h"
#include "tetrix.h"
#include "tetrix_controller.h"

#define TETRIXNAME L"ZWPTetrix"

HINSTANCE g_inst;

Tetrix::Tetrix(HINSTANCE hInst)
{
	g_inst = hInst;
	HWND hWnd = CreateTetrixWindow(hInst);
	if (hWnd)
	{
		m_spController.reset(new TetrixController(hWnd));

		ShowWindow(hWnd, SW_NORMAL);
		UpdateWindow(hWnd);
	}
	else
	{
		MessageBoxW(NULL, L"Tetrix init error...", TETRIXNAME, MB_OK | MB_ICONERROR);
	}
}

int Tetrix::Run()
{
	if (m_spController.get())
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);	
		}
		return (int)msg.wParam;
	}
	return 0;
}

// ------------------------------------------------------------------------
// static
HWND Tetrix::CreateTetrixWindow(HINSTANCE hInst)
{
	static WCHAR szwTetrixWindowClassName[] = L"TetrixWindow";
	static WCHAR szwTetrixWindowTitleName[] = TETRIXNAME;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= TetrixController::TetrixWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= LoadIcon(hInst, MAKEINTRESOURCE(IDI_TETRIX));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName	= szwTetrixWindowClassName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_TETRIX));

	if (RegisterClassEx(&wcex))
	{
		int w = ::GetSystemMetrics(SM_CXSCREEN);   
		int h = ::GetSystemMetrics(SM_CYSCREEN); 
		HWND hWnd = CreateWindow(szwTetrixWindowClassName, szwTetrixWindowTitleName, WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION,
			w / 3, h / 8, w / 3, h * 3 / 4, NULL, NULL, hInst, NULL);
		return hWnd;
	}
	return NULL;
}