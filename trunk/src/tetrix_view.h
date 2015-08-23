#ifndef __TETRIX_VIEW_H__
#define __TETRIX_VIEW_H__

class TetrixController;
struct TetrixInfo;

class TetrixViewer
{
public:
	TetrixViewer(HWND hWnd, TetrixController *pTetrixController);
	void Clear();
	void Draw();
	void DrawBlocks(const Point *pts, UINT count);
	void DrawNextBlock(const Point *pts, UINT count);
	void DrawStart();
	void DrawEnd(const TetrixInfo *tetrixInfo);
	void DrawOuterBound();

	void DrawSpecialDestroy(const UINT *pDelLines, UINT s);

	void Flush();

private:
	TetrixController *m_pTetrixController;
	HWND m_hWnd;
	std::auto_ptr<Bitmap> m_spBmp;
	Rect m_blockRect;
	Rect m_nextBlockRect;

	std::auto_ptr<Bitmap> m_spTitleBmp;
	std::auto_ptr<Bitmap> m_spGamePreBmp;
	std::auto_ptr<Bitmap> m_spGameCopyRightBmp;
	std::auto_ptr<Bitmap> m_spGameEndBmp;
};

#endif