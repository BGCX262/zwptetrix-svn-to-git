#ifndef __TETRIX_SOUND_H__
#define __TETRIX_SOUND_H__


class TetrixController;

class TetrixSound
{
public:
	TetrixSound(TetrixController *pTetrixController);

	void PlayGameBack();
	void PlayClick();
	void PlayBegin();
	void PlayMoveHori();
	void PlayFlip();
	void PlayFastDown();
	void PlaySlowDown();
	void PlayMerge();
	void PlayDelete(UINT nArranges);
	void PlayEnd();

private:
	TetrixController *m_pTetrixController;
};



#endif