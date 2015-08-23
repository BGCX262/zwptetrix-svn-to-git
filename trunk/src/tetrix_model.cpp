#include "stdafx.h"
#include "tetrix_model.h"
#include "tetrix_controller.h"

class RandomNumberGenerator
{
public:
	RandomNumberGenerator()
	{
		srand(GetTickCount());
	}

	int Generate() const
	{
		return rand();
	}

private:
};

class BuildingBlock
{
public:
	BuildingBlock()
	{
		Reset();
	}

	void Reset()
	{
		for (unsigned i = 0; i < 24; ++i)
		{
			for (unsigned j = 0; j < 12; ++j)
			{
				m_matrix[i][j] = 0;
			}
		}
	}

	void GeneratePoints(std::vector<Point>& vecPts)
	{
		for (unsigned i = 0; i < 24; ++i)
		{
			for (unsigned j = 0; j < 12; ++j)
			{
				if (m_matrix[i][j] != 0)
				{
					vecPts.push_back(Point(j, i));
				}
			}
		}
	}

	bool CheckArrangesToDelete(std::vector<UINT>& vecLinesToDel)
	{
		for (int i = 23; i >= 0; --i)
		{
			unsigned j = 0;
			for (; j < 12; ++j)
			{
				if (m_matrix[i][j] == 0)
				{
					break;
				}
			}
			if (12 == j)
			{
				vecLinesToDel.insert(vecLinesToDel.begin(), i);
			}
		}
		return !vecLinesToDel.empty();
	}

	void DeleteArrange(UINT *pLines, UINT s)
	{
		for (unsigned i = 0; i < s; ++i)
		{
			const UINT line = *(pLines + i);
			ASSERT(line >= 0 && line < 24);
			for (unsigned j = line; j > 0; --j)
			{
				for (unsigned k = 0; k < 12; ++k)
				{
					m_matrix[j][k] = m_matrix[j-1][k];
				}
			}
			for (unsigned k = 0; k < 12; ++k)
			{
				m_matrix[0][k] = 0;
			}
		}
	}

public:
	UINT m_matrix[24][12];
};



static int gs_blocks[][4][4][2] = {
	{
		/*
		 #
		###
		 

		*/
		{{1,0}, {0,1}, {1,1}, {2,1}},
		{{1,0}, {0,1}, {1,1}, {1,2}},
		{{1,2}, {0,1}, {1,1}, {2,1}},
		{{1,0}, {1,1}, {2,1}, {1,2}}
	},
	{
		/*
		 #
		 ##
		  #

		*/
		{{1,0}, {1,1}, {2,1}, {2,2}},
		{{1,1}, {2,1}, {0,2}, {1,2}},
		{{1,0}, {1,1}, {2,1}, {2,2}},
		{{1,1}, {2,1}, {0,2}, {1,2}}
	},
	{
		/*
		  #
		 ##
		 #

		*/
		{{2,0}, {1,1}, {2,1}, {1,2}},
		{{0,1}, {1,1}, {1,2}, {2,2}},
		{{2,0}, {1,1}, {2,1}, {1,2}},
		{{0,1}, {1,1}, {1,2}, {2,2}}
	},
	{
		/*
		 #
		 #
		 ##

		*/
		{{1,0}, {1,1}, {1,2}, {2,2}},
		{{2,1}, {0,2}, {1,2}, {2,2}},
		{{1,1}, {2,1}, {2,2}, {2,3}},
		{{1,1}, {2,1}, {3,1}, {1,2}}
	},
	{
		/*
		  #
		  #
		 ##

		*/
		{{2,0}, {2,1}, {1,2}, {2,2}},
		{{0,1}, {1,1}, {2,1}, {2,2}},
		{{1,1}, {2,1}, {1,2}, {1,3}},
		{{1,1}, {1,2}, {2,2}, {3,2}}
	},
	{
		/*
		 #
		 #
		 #
	     #
		*/
		{{1,0}, {1,1}, {1,2}, {1,3}},
		{{0,2}, {1,2}, {2,2}, {3,2}},
		{{1,0}, {1,1}, {1,2}, {1,3}},
		{{0,2}, {1,2}, {2,2}, {3,2}}
	},
	{
		/*
		 ##
		 ##


		*/
		{{1,0}, {2,0}, {1,1}, {2,1}},
		{{1,0}, {2,0}, {1,1}, {2,1}},
		{{1,0}, {2,0}, {1,1}, {2,1}},
		{{1,0}, {2,0}, {1,1}, {2,1}}
	}
};

class MovingBlock
{
public:
	MovingBlock(UINT nBlockIndex)
		: m_xOff(4)
		, m_yOff(0)
		, m_nBlockIndex(nBlockIndex)
		, m_nBlockOffset(0)
	{

	}

	bool IsValid(BuildingBlock *pBuildingBlock) const
	{
		return IsValid(m_xOff, m_yOff, m_nBlockIndex, m_nBlockOffset, pBuildingBlock);
	}

	bool IsLeftAble(BuildingBlock *pBuildingBlock) const
	{
		return IsValid(m_xOff-1, m_yOff, m_nBlockIndex, m_nBlockOffset, pBuildingBlock);
	}

	void Left()
	{
		--m_xOff;
	}

	bool IsRightAble(BuildingBlock *pBuildingBlock) const
	{
		return IsValid(m_xOff+1, m_yOff, m_nBlockIndex, m_nBlockOffset, pBuildingBlock);
	}

	void Right()
	{
		++m_xOff;
	}

	bool IsDownAble(BuildingBlock *pBuildingBlock) const
	{
		return IsValid(m_xOff, m_yOff+1, m_nBlockIndex, m_nBlockOffset, pBuildingBlock);		
	}

	void Down()
	{
		++m_yOff;
	}

	bool IsFlipAble(BuildingBlock *pBuildingBlock) const
	{
		return IsValid(m_xOff, m_yOff, m_nBlockIndex, (m_nBlockOffset + 1) & 3, pBuildingBlock);
	}

	void Flip()
	{
		++m_nBlockOffset;
		m_nBlockOffset &= 3; 
	}

	void FlushTo(BuildingBlock *pBuildingBlock)
	{
		for (unsigned i = 0; i < 4; ++i)
		{
			int x = m_xOff + gs_blocks[m_nBlockIndex][m_nBlockOffset][i][0];
			int y = m_yOff + gs_blocks[m_nBlockIndex][m_nBlockOffset][i][1];
			pBuildingBlock->m_matrix[y][x] = 1;
		}
	}

	void GeneratePoints(std::vector<Point>& vecPts, bool bPure = false)
	{
		for (unsigned i = 0; i < 4; ++i)
		{
			int x = gs_blocks[m_nBlockIndex][m_nBlockOffset][i][0];
			int y = gs_blocks[m_nBlockIndex][m_nBlockOffset][i][1];
			if (!bPure)
			{
				x += m_xOff;
				y += m_yOff;
			}
			vecPts.push_back(Point(x, y));
		}
	}

private:
	static bool IsValid(int xOff, int yOff, UINT nBlockIndex, UINT nBlockOffset, BuildingBlock *pBuildingBlock)
	{
		for (unsigned i = 0; i < 4; ++i)
		{
			int x = xOff + gs_blocks[nBlockIndex][nBlockOffset][i][0];
			int y = yOff + gs_blocks[nBlockIndex][nBlockOffset][i][1];
			if (x < 0 || x > 11 || y < 0 || y > 23 ||
				(pBuildingBlock->m_matrix[y][x] == 1))
			{
				return false;
			}
		}
		return true;
	}

public:
	int m_xOff;
	int m_yOff;
	UINT m_nBlockIndex;
	UINT m_nBlockOffset;
};


// AccessController
/*
struct Enterer
{
	Enterer(CRITICAL_SECTION *cs)
	{
		m_cs = cs;
		::EnterCriticalSection(m_cs);
	}

	~Enterer()
	{
		::LeaveCriticalSection(m_cs);
	}

	CRITICAL_SECTION *m_cs;
};
typedef std::auto_ptr<Enterer> AutoEnterer;

class SynchronizeController
{
public:
	SynchronizeController()
	{
		::InitializeCriticalSection(&m_criticalSection);
	}

	AutoEnterer Enter()
	{
		return AutoEnterer(new Enterer(&m_criticalSection));
	}

	void Leave()
	{
	}

private:
	CRITICAL_SECTION m_criticalSection;
};
*/

TetrixModel::TetrixModel(TetrixController *pTetrixController)
: m_spRandomNumberGenerator(new RandomNumberGenerator)
, m_spBuildingBlock(new BuildingBlock)
, m_spMovingBlock(new MovingBlock(m_spRandomNumberGenerator->Generate() % 7))
, m_spNextMovingBlock(new MovingBlock(m_spRandomNumberGenerator->Generate() % 7))
, m_pTetrixController(pTetrixController)
{
	ASSERT(pTetrixController);
}

void TetrixModel::MoveDownMovingBlock(UINT num /* = 1*/)
{
	for (unsigned i = 0; i < num; )
	{
		if (m_spMovingBlock->IsDownAble(m_spBuildingBlock.get()))
		{
			m_spMovingBlock->Down();
		}
		else
		{
			if (i > 0)
			{
				SendDataChangedEvent();
			}
			break;
		}
		++i;
		if (num == i)
		{
			SendDataChangedEvent();
			return;
		}
	}
	MergeMovingBlockToBuildingBlock();
}

void TetrixModel::MoveLeftMovingBlock()
{
	if (m_spMovingBlock->IsLeftAble(m_spBuildingBlock.get()))
	{
		m_spMovingBlock->Left();
		SendDataChangedEvent();
	}
}

void TetrixModel::MoveRightMovingBlock()
{
	if (m_spMovingBlock->IsRightAble(m_spBuildingBlock.get()))
	{
		m_spMovingBlock->Right();
		SendDataChangedEvent();
	}
}

void TetrixModel::FlipMovingBlock()
{
	if (m_spMovingBlock->IsFlipAble(m_spBuildingBlock.get()))
	{
		m_spMovingBlock->Flip();
		SendDataChangedEvent();
	}
}

void TetrixModel::BottomDownMovingBlock()
{
	while (m_spMovingBlock->IsDownAble(m_spBuildingBlock.get()))
	{
		m_spMovingBlock->Down();
	}
	MergeMovingBlockToBuildingBlock();
}


void TetrixModel::Reset()
{
	m_spBuildingBlock->Reset();
	m_spMovingBlock.reset(new MovingBlock(m_spRandomNumberGenerator->Generate() % 7));
	m_spNextMovingBlock.reset(new MovingBlock(m_spRandomNumberGenerator->Generate() % 7));
	SendDataChangedEvent();
	SendDataNextGeneratedEvent();
}

// --------------------------------------------------------------------------
void TetrixModel::MergeMovingBlockToBuildingBlock()
{
	m_pTetrixController->OnDataCommand(DATA_TO_MERGE, 0, 0);
	m_spMovingBlock->FlushTo(m_spBuildingBlock.get());
	SendDataChangedEvent();
	UINT nArrangeDelFrom = 0, nArrangeDelTo = 0;
	std::vector<UINT> vecLineToDel;
	bool bNeedDelArrange = m_spBuildingBlock->CheckArrangesToDelete(vecLineToDel);
	const unsigned s = vecLineToDel.size();
	if (s > 0)
	{
		SendDataArrangeToDelete(&vecLineToDel[0], s);
		m_spBuildingBlock->DeleteArrange(&vecLineToDel[0], s);
		SendDataArrangeDeleted(&vecLineToDel[0], s);
	}
	m_spMovingBlock = m_spNextMovingBlock;
	m_spNextMovingBlock.reset(new MovingBlock(m_spRandomNumberGenerator->Generate() % 7));
	SendDataChangedEvent();
	SendDataNextGeneratedEvent();
	if (false == m_spMovingBlock->IsValid(m_spBuildingBlock.get()))
	{
		SendDataTopOverView();
	}
}

void TetrixModel::SendDataChangedEvent()
{
	std::vector<Point> vecPts;
	vecPts.reserve(8);
	m_spBuildingBlock->GeneratePoints(vecPts);
	m_spMovingBlock->GeneratePoints(vecPts);

	m_pTetrixController->OnDataCommand(DATA_CHANGED, (WPARAM)&vecPts[0], vecPts.size());
}

void TetrixModel::SendDataNextGeneratedEvent()
{
	std::vector<Point> vecPts;
	vecPts.reserve(4);
	m_spNextMovingBlock->GeneratePoints(vecPts, true);
	m_pTetrixController->OnDataCommand(DATA_NEXTBLOCK_GENERATED, (WPARAM)&vecPts[0], vecPts.size());
}

void TetrixModel::SendDataTopOverView()
{
	m_pTetrixController->OnDataCommand(DATA_TOP_OVERFLOW, 0, 0);
}

void TetrixModel::SendDataArrangeToDelete(const UINT *pDelLines, UINT s)
{
	m_pTetrixController->OnDataCommand(DATA_ARRANGE_TO_DELETE, (WPARAM)pDelLines, s);
}

void TetrixModel::SendDataArrangeDeleted(const UINT *pDelLines, UINT s)
{
	m_pTetrixController->OnDataCommand(DATA_ARRANGE_DELETED, (WPARAM)pDelLines, s);
}