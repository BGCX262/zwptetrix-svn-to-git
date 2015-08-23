#include "stdafx.h"
#include "tetrix_controller.h"
#include "tetrix_model.h"
#include "tetrix_view.h"
#include "tetrix_sound.h"

class Timer
{
public:
	Timer(HWND hWnd, UINT id, UINT uElapse)
		: m_hWnd(hWnd)
		, m_id(id)
		, m_uElapse(uElapse)
	{
		::SetTimer(m_hWnd, m_id, m_uElapse, NULL);
	}

	~Timer()
	{
		Destroy();
	}

	void Reset(UINT uElapse)
	{
		if (m_uElapse != uElapse)
		{
			m_uElapse = uElapse;
			::SetTimer(m_hWnd, m_id, m_uElapse, NULL);
		}
	}

	UINT GetID() const
	{
		return m_id;
	}

	UINT GetElapse() const
	{
		return m_uElapse;
	}

private:
	void Destroy()
	{
		if (m_id)
		{
			::KillTimer(m_hWnd, m_id);
			m_id = 0;
			m_hWnd = NULL;
			m_uElapse = 0;
		}
	}

private:
	HWND m_hWnd;
	UINT m_id;
	UINT m_uElapse;
};


TetrixController::TetrixController(HWND hWnd)
: m_hWnd(hWnd)
, m_spTetrixModel(new TetrixModel(this))
, m_spTetrixViewer(new TetrixViewer(hWnd, this))
, m_spSound(new TetrixSound(this))
, m_status(GamePreparing)
{
	ASSERT(hWnd);
	::SetWindowLong(hWnd, GWL_USERDATA, (LONG)((void*)this));
}


void TetrixController::OnPaintEvent(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	::BeginPaint(m_hWnd, &ps);
	m_spTetrixViewer->Draw();
	::EndPaint(m_hWnd, &ps);
}

void TetrixController::OnKeyDownEvent(WPARAM wParam, LPARAM lParam)
{
	if (IsInGameScope())
	{
		// play event
		switch (wParam)
		{
		case 'A':
		case VK_LEFT:
			{
				++m_tetrixInfo.leftTimes;
				m_spSound->PlayMoveHori();
				m_spTetrixModel->MoveLeftMovingBlock();
			}
			break;
		case 'D':
		case VK_RIGHT:
			{  
				++m_tetrixInfo.rightTimes;
				m_spSound->PlayMoveHori();
				m_spTetrixModel->MoveRightMovingBlock();
			}
			break;
		case 'S':
		case VK_DOWN:
			{
				++m_tetrixInfo.downTimes;
				m_spSound->PlaySlowDown();
				m_spTetrixModel->MoveDownMovingBlock(1);
			}
			break;
		case 'W':
		case VK_UP:
			{
				m_spSound->PlayFlip();
				m_spTetrixModel->FlipMovingBlock();
				++m_tetrixInfo.flipTimes;
			}
			break;

		case VK_SPACE:
			{
				++m_tetrixInfo.fastdownTimes;
				m_spSound->PlayFastDown();
				m_spTetrixModel->BottomDownMovingBlock();
			}
			break;

		default:
			break;
		}
	}
	else
	{
		// control event
		switch (wParam)
		{
		default:
			break;
		}
	}


}


void TetrixController::OnTimerEvent(WPARAM wParam, LPARAM lParam)
{
	if (IsInGameScope())
	{
		// play event
		if (wParam == m_spTimer->GetID())
		{
			m_spTetrixModel->MoveDownMovingBlock();
		}
	}
	else
	{
		// control Event
	}
}


void TetrixController::OnDestroyEvent(WPARAM wParam, LPARAM lParam)
{
	::PostQuitMessage(0);
}

void TetrixController::OnLeftButtonDownEvent(WPARAM wParam, LPARAM lParam)
{
	if (GamePreparing == m_status || GameEnded == m_status)
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;
		if (abs(x - w / 2) <= w / 8)
		{
			int ph = w / 4 / 3;
			if (y >= h / 2 && y <= (h / 2 + ph))
			{
				m_spSound->PlayClick();
				StartGame();
			}
			else if (y >= (h / 2 + ph * 2) && y <= (h / 2 + ph * 3))
			{
				if (GamePreparing == m_status)
				{
					m_spSound->PlayClick();
					::PostQuitMessage(0);
				}
				else
				{
					m_spSound->PlayClick();
					BackToMenu();
				}
			}
		}
	}
}

void TetrixController::OnDataCommand(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if (IsInGameScope())
	{
		switch (nCode)
		{
		case DATA_CHANGED:
			m_spTetrixViewer->DrawBlocks((const Point *)wParam, (UINT)lParam);
			m_spTetrixViewer->Draw();
			break;
		case DATA_TOP_OVERFLOW:
			m_spSound->PlayEnd();
			EndGame();
			break;
		case DATA_ARRANGE_TO_DELETE:
			m_spSound->PlayDelete((UINT)lParam);
			m_spTetrixViewer->DrawSpecialDestroy((const UINT*)wParam, (UINT)lParam);
			break;
		case DATA_ARRANGE_DELETED:
			{
				switch((UINT)lParam)
				{
				case 0:
					++m_tetrixInfo.singleLineDelTimes;
					break;
				case 1:
					++m_tetrixInfo.doubleLineDelTimes;
					break;
				case 2:
					++m_tetrixInfo.threeLineDelTimes;
					break;
				case 3:
					++m_tetrixInfo.fourLineDelTimes;
					break;
				default:
					ASSERT(FALSE);
					break;
				}
				int nEscape = 500 - m_tetrixInfo.CalcScore() / 100 * 25;
				m_spTimer->Reset(nEscape < 0 ? 15 : nEscape);
			}
			break;
		case DATA_NEXTBLOCK_GENERATED:
			m_spTetrixViewer->DrawNextBlock((const Point *)wParam, (UINT)lParam);
			++m_tetrixInfo.blockNums;
			m_spTetrixViewer->Draw();
			break;

		case DATA_TO_MERGE:
			m_spSound->PlayMerge();
			break;
		default:
			break;
		}
	}
}



// static 
LRESULT TetrixController::TetrixWindowProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		{
			LONG lV = ::GetWindowLong(hWnd, GWL_USERDATA);
			TetrixController *pTetrixController = reinterpret_cast<TetrixController*>(lV);
			ASSERT(pTetrixController);
			pTetrixController->OnPaintEvent(wParam, lParam);
		}
		break;

	case WM_KEYDOWN:
		{
			LONG lV = ::GetWindowLong(hWnd, GWL_USERDATA);
			TetrixController *pTetrixController = reinterpret_cast<TetrixController*>(lV);
			ASSERT(pTetrixController);
			pTetrixController->OnKeyDownEvent(wParam, lParam);
		}
		break;
	case WM_TIMER:
		{
			LONG lV = ::GetWindowLong(hWnd, GWL_USERDATA);
			TetrixController *pTetrixController = reinterpret_cast<TetrixController*>(lV);
			ASSERT(pTetrixController);
			pTetrixController->OnTimerEvent(wParam, lParam);
		}
		break;
	case WM_DESTROY:
		{
			LONG lV = ::GetWindowLong(hWnd, GWL_USERDATA);
			TetrixController *pTetrixController = reinterpret_cast<TetrixController*>(lV);
			ASSERT(pTetrixController);
			pTetrixController->OnDestroyEvent(wParam, lParam);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			LONG lV = ::GetWindowLong(hWnd, GWL_USERDATA);
			TetrixController *pTetrixController = reinterpret_cast<TetrixController*>(lV);
			ASSERT(pTetrixController);
			pTetrixController->OnLeftButtonDownEvent(wParam, lParam);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

GameStatus TetrixController::GetStatus() const
{
	return m_status;
}

// -----------------------------------------------------------------------
void TetrixController::StartGame()
{
	m_spTetrixViewer->Clear();
	m_spTetrixViewer->DrawOuterBound();
	m_spSound->PlayBegin();
	m_status = GameRunning;
	m_tetrixInfo.Reset();
	m_spTetrixModel->Reset();
	m_spTimer.reset(new Timer(m_hWnd, 0, 500));
}

void TetrixController::EndGame()
{
	m_spTimer.reset();
	m_status = GameEnded;
	m_spTetrixViewer->DrawEnd(&m_tetrixInfo);
	m_spTetrixViewer->Draw();
	m_spTetrixViewer->Flush();
}

bool TetrixController::IsInGameScope()
{
	return m_status == GameRunning;
}

void TetrixController::BackToMenu()
{
	m_status = GamePreparing;
	m_spTetrixViewer->Clear();
	m_spTetrixViewer->DrawStart();
	m_spTetrixViewer->Draw();
}