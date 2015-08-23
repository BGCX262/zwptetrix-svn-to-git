#include "stdafx.h"
#include "tetrix_sound.h"
#include "tetrix_controller.h"
#include "resource.h"

#pragma comment(lib, "Winmm.lib")

extern HINSTANCE g_inst;

TetrixSound::TetrixSound(TetrixController *pTetrixController)
: m_pTetrixController(pTetrixController)
{
	ASSERT(pTetrixController);
}

void TetrixSound::PlayGameBack()
{

}

void TetrixSound::PlayClick()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE1), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlayBegin()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE9), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlayMoveHori()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE3), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlayFlip()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE2), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlayFastDown()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE4), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlaySlowDown()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE7), g_inst, SND_RESOURCE | SND_ASYNC);
}

void TetrixSound::PlayMerge()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE5), g_inst, SND_RESOURCE | SND_ASYNC);
}


void TetrixSound::PlayDelete(UINT nArranges)
{
// 	for (unsigned i = 0; i < nArranges; ++i)
// 	{
		::PlaySound(MAKEINTRESOURCE(IDR_WAVE6), g_inst, SND_RESOURCE | SND_ASYNC);
/*	}*/
}


void TetrixSound::PlayEnd()
{
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE8), g_inst, SND_RESOURCE | SND_ASYNC);
}

