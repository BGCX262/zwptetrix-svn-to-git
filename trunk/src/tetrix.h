#ifndef __TETRIX_H__
#define __TETRIX_H__

class TetrixController;
class TetrixModel;
class TetrixViewer;
class TetrixSound;

class Tetrix
{
public:
	Tetrix(HINSTANCE hinst);
	int Run();

private:
	Tetrix(const Tetrix&);
	Tetrix& operator=(const Tetrix&);

private:
	static HWND CreateTetrixWindow(HINSTANCE hInst);

private:
	std::auto_ptr<TetrixController> m_spController;
};
#endif