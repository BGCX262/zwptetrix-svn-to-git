#include "stdafx.h"
#include "tetrix.h"


int APIENTRY wWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	Tetrix tetrix(hInstance);
	return tetrix.Run();
}



