#include "stdafx.h"
#include "tetrix_view.h"
#include "tetrix_controller.h"
#include "resource.h"

#include <sstream>

class GdiplusManager 
{
public:
	GdiplusManager()
		: m_token(0)
	{
		GdiplusStartupInput gdpSi;
		GdiplusStartup(&m_token, &gdpSi, NULL);
	}

	~GdiplusManager()
	{
		GdiplusShutdown(m_token);
	}

private:
	unsigned long m_token;
};

static GdiplusManager gs_GdipMgr;
extern HINSTANCE g_inst;

Bitmap *LoadImageFromRes(UINT pResourceID, HMODULE hInstance, LPCTSTR pResourceType)
{
	LPCTSTR pResourceName = MAKEINTRESOURCE(pResourceID);

	HRSRC hResource = FindResource(hInstance, pResourceName, pResourceType);

	if(!hResource)
	{
		return NULL;
	}

	DWORD dwResourceSize = SizeofResource(hInstance, hResource);

	if(!dwResourceSize)
	{
		return NULL;
	}

	const void* pResourceData = LockResource(LoadResource(hInstance, hResource));

	if(!pResourceData)
	{
		return NULL;
	}

	HGLOBAL hResourceBuffer = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);

	if(!hResourceBuffer)
	{
		GlobalFree(hResourceBuffer);
		return NULL;
	}

	void* pResourceBuffer = GlobalLock(hResourceBuffer);

	if(!pResourceBuffer)
	{
		GlobalUnlock(hResourceBuffer);
		GlobalFree(hResourceBuffer);
		return NULL;
	}

	CopyMemory(pResourceBuffer, pResourceData, dwResourceSize);
	IStream* pIStream = NULL;

	if(CreateStreamOnHGlobal(hResourceBuffer, FALSE, &pIStream)==S_OK)
	{
		Bitmap *pBitmap = Bitmap::FromStream(pIStream);
		Bitmap *pClone = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat32bppARGB);
		delete pBitmap;
		pIStream->Release();
		GlobalUnlock(hResourceBuffer);
		GlobalFree(hResourceBuffer);
		return pClone;
	}
	GlobalUnlock(hResourceBuffer);
	GlobalFree(hResourceBuffer);

	return NULL;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo *pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

TetrixViewer::TetrixViewer(HWND hWnd, TetrixController *pTetrixController)
: m_pTetrixController(pTetrixController)
, m_hWnd(hWnd)
, m_spGameEndBmp(LoadImageFromRes(IDB_PNG_GAME_END, g_inst, L"PNG"))
, m_spGameCopyRightBmp(LoadImageFromRes(IDB_PNG_GAME_COPY, g_inst, L"PNG"))
, m_spGamePreBmp(LoadImageFromRes(IDB_PNG_GAME_PRE, g_inst, L"PNG"))
, m_spTitleBmp(LoadImageFromRes(IDB_PNG_GAME_TITLE, g_inst, L"PNG"))
{
	ASSERT(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;
	int rh = w > h ? h : w;
	m_spBmp.reset(new Bitmap(w, h, PixelFormat24bppRGB));
	const int blockW = rh / 2;
	const int blockH = rh;
	const int blockX = (w - blockW) / 2;
	const int blockY = blockW / 3 + 20;
	m_blockRect = Rect(blockX, blockY, blockW, blockH);
	const int nextW = rh / 6;
	const int nextH = rh / 6;
	const int nextX = (w - nextW) / 2;
	const int nextY = 0;
	m_nextBlockRect = Rect(nextX, nextY, nextW, nextH);

	Clear();
	DrawStart();
}

void TetrixViewer::Clear()
{
	Graphics g(m_spBmp.get());
	g.Clear(Color::Black);
}

void TetrixViewer::Draw()
{
	Graphics g(m_hWnd);
	g.DrawImage(m_spBmp.get(), 0, 0);
}

void TetrixViewer::DrawBlocks(const Point *pts, UINT count)
{
	if (count > 0)
	{
		Graphics g(m_spBmp.get());
		g.FillRectangle(&SolidBrush(Color::Black), m_blockRect);
		g.TranslateTransform(m_blockRect.X, m_blockRect.Y);
		const REAL step = (REAL)m_blockRect.Width / 12;
		for (unsigned i = 0; i < count; ++i)
		{
			const int& x = pts[i].X;
			const int& y = pts[i].Y;
			Rect rect(x * step + 1, y * step + 1, step - 2, step - 2);
			g.FillRectangle(&SolidBrush(Color(255, 255 - y * 10, 20 + y * 6, 10)), rect);
		}
	}
}

void TetrixViewer::DrawNextBlock(const Point *pts, UINT count)
{
	if (count > 0)
	{
		Graphics g(m_spBmp.get());
		g.FillRectangle(&SolidBrush(Color::Black), m_nextBlockRect);
		g.TranslateTransform(m_nextBlockRect.X, m_nextBlockRect.Y);
		const REAL step = (REAL)m_nextBlockRect.Width / 4;
		for (unsigned i = 0; i < count; ++i)
		{
			const int& x = pts[i].X;
			const int& y = pts[i].Y;
			Rect rect(x * step + 1, y * step + 1, step - 2, step - 2);
			g.FillRectangle(&SolidBrush(Color(Color::Gray)), rect);
		}
	}
}

void TetrixViewer::DrawStart()
{
	// StartBmp	
	Graphics g(m_spBmp.get());
	const UINT backW = m_spBmp->GetWidth();
	const UINT backH = m_spBmp->GetHeight();
	g.DrawImage(m_spTitleBmp.get(), 0, backH / 4, backW, backW * m_spTitleBmp->GetHeight() / m_spTitleBmp->GetWidth());
	int cw = backW;
	int ch = cw * m_spGameCopyRightBmp->GetHeight() / m_spGameCopyRightBmp->GetWidth();
	g.DrawImage(m_spGameCopyRightBmp.get(), ((int)backW - cw) / 2, (int)backH  - ch, cw, ch);
	g.DrawImage(m_spGamePreBmp.get(), (int)backW * 3 / 8, (int)backH / 2, (int)backW / 4, (int)backW / 4);
}

void TetrixViewer::DrawEnd(const TetrixInfo *tetrixInfo)
{
	Graphics g(m_spBmp.get());
	const UINT backW = m_spBmp->GetWidth();
	const UINT backH = m_spBmp->GetHeight();
	g.FillRectangle(&SolidBrush(Color(192, 32, 32, 32)), m_blockRect);
	g.DrawImage(m_spGameEndBmp.get(), (int)backW * 3 / 8, (int)backH / 2, (int)backW / 4, (int)backW / 4);
	

	REAL h = (REAL)(backW < backH ? backW : backH) / 24;
	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, h * 0.75);
	SolidBrush sb(Color::Gray);
	std::wstringstream wstrstream;
	std::wstring wstr;
	REAL y = 0;

	wstrstream << L"TotalBlocks: " << tetrixInfo->blockNums;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"Flips: " << tetrixInfo->flipTimes;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"KeyLeft: " << tetrixInfo->leftTimes;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"KeyRight: " << tetrixInfo->rightTimes;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"KeyDown: " << tetrixInfo->downTimes;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"KeyFastDown: " << tetrixInfo->fastdownTimes;
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"DestroyOneLine: " << tetrixInfo->singleLineDelTimes << L" x 10";
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"DestroyTwoLine: " << tetrixInfo->doubleLineDelTimes << L" x 20";
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"DestroyThreeLine: " << tetrixInfo->threeLineDelTimes << L" x 40";
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"DestroyFourLine: " << tetrixInfo->fourLineDelTimes << L" x 60";
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &sb);
	y += h;

	wstrstream.str(L"");
	wstrstream.clear();
	wstrstream << L"Score: " << tetrixInfo->CalcScore();
	wstr = wstrstream.str();
	g.DrawString(wstr.c_str(), wstr.size(), &font, PointF(0, y), &SolidBrush(Color::Yellow));
	y += h;
}

void TetrixViewer::DrawOuterBound()
{
	Graphics g(m_spBmp.get());
	for (int i = 0; i < 20; ++i)
	{
		g.DrawRectangle(&Pen(Color(255, 0, 0, 255 - i * 12)), 
			m_blockRect.X - i, m_blockRect.Y - i, 
			m_blockRect.Width - 1 + i * 2, 
			m_blockRect.Height - 1 + i * 2);
	}
}

void TetrixViewer::DrawSpecialDestroy(const UINT *pDelLines, UINT s)
{
	Graphics g(m_spBmp.get());
	g.TranslateTransform(m_blockRect.X, m_blockRect.Y);
	const REAL step = (REAL)m_blockRect.Width / 12;
	for (unsigned k = 0; k < 15; ++k)
	{
		SolidBrush sb(Color(0 + k * 12, 30, 30, 30));
		for (UINT i = 0; i < s; ++i)
		{
			for (unsigned j = 0; j < 12; ++j)
			{
				g.FillRectangle(&sb, Rect(j * step + 1, pDelLines[i] * step + 1, step - 2, step - 2));
			}
		}
		Draw();
		Sleep(50);
	}

}

void TetrixViewer::Flush()
{
	CLSID cls;
	GetEncoderClsid(L"image/bmp", &cls);
	WCHAR swzFileName[MAX_PATH] = {'\\', '\0'};
	UINT nLen = ::GetModuleFileNameW(NULL, swzFileName, MAX_PATH);
	while (nLen && swzFileName[--nLen] != '\\')
	{
		swzFileName[nLen] = '\0';
	}
	wcscat(swzFileName, L"ZWPTetrix.bmp");
	m_spBmp->Save(swzFileName, &cls);
}

// -------------------------------------------------------------------------

