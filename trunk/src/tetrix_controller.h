#ifndef __TETRIX_CONTROLLER_H__
#define __TETRIX_CONTROLLER_H__

class TetrixModel;
class TetrixViewer;
class Timer;
class TetrixSound;

enum GameStatus
{
	GamePreparing,
	GameRunning,
	GameEnded
};

struct TetrixInfo
{
	TetrixInfo()
	{
		Reset();
	}

	void Reset()
	{
		blockNums = 0;
		leftTimes = 0;
		rightTimes = 0;
		downTimes = 0;
		fastdownTimes = 0;
		flipTimes = 0;
		singleLineDelTimes = 0;
		doubleLineDelTimes = 0;
		threeLineDelTimes = 0;
		fourLineDelTimes = 0;
	}

	int CalcScore() const
	{
		return singleLineDelTimes * 10 + 
			doubleLineDelTimes * 20 +
			threeLineDelTimes * 40 +
			fourLineDelTimes * 60;
	}

	UINT blockNums;
	UINT leftTimes;
	UINT rightTimes;
	UINT downTimes;
	UINT fastdownTimes;
	UINT flipTimes;
	UINT singleLineDelTimes;
	UINT doubleLineDelTimes;
	UINT threeLineDelTimes;
	UINT fourLineDelTimes;
};

class TetrixController
{
public:
	TetrixController(HWND hWnd);

	void OnPaintEvent(WPARAM wParam, LPARAM lParam);
	void OnKeyDownEvent(WPARAM wParam, LPARAM lParam);
	void OnTimerEvent(WPARAM wParam, LPARAM lParam);
	void OnDestroyEvent(WPARAM wParam, LPARAM lParam);
	void OnLeftButtonDownEvent(WPARAM wParam, LPARAM lParam);

	void OnDataCommand(UINT nCode, WPARAM wParam, LPARAM lParam);


	GameStatus GetStatus() const;

	static LRESULT CALLBACK TetrixWindowProc(HWND, UINT, WPARAM, LPARAM);

private:
	void StartGame();
	void EndGame();
	bool IsInGameScope();
	void BackToMenu();

private:
	HWND m_hWnd;
	std::auto_ptr<TetrixModel> m_spTetrixModel;
	std::auto_ptr<TetrixViewer> m_spTetrixViewer;
	std::auto_ptr<TetrixSound> m_spSound;

	std::auto_ptr<Timer> m_spTimer;
	
	TetrixInfo m_tetrixInfo;
	GameStatus m_status;
};

#endif