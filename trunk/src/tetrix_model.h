#ifndef __TETRIX_MODEL_H__
#define __TETRIX_MODEL_H__

class TetrixController;
class BuildingBlock;
class MovingBlock;
class RandomNumberGenerator;

#define DATA_CHANGED 0x00000001
#define DATA_TOP_OVERFLOW 0x00000002
#define DATA_ARRANGE_TO_DELETE 0x00000003
#define DATA_ARRANGE_DELETED 0x00000004
#define DATA_NEXTBLOCK_GENERATED 0x00000005
#define DATA_TO_MERGE 0x00000006

class TetrixModel
{
public:
	TetrixModel(TetrixController *pTetrixController);
	
	void MoveDownMovingBlock(UINT num = 1);
	void MoveLeftMovingBlock();
	void MoveRightMovingBlock();
	void FlipMovingBlock();
	void BottomDownMovingBlock();

	void Reset();

private:
	TetrixModel(const TetrixModel&);
	TetrixModel& operator=(const TetrixModel&);

	void MergeMovingBlockToBuildingBlock();
	void SendDataChangedEvent();
	void SendDataTopOverView();
	void SendDataArrangeToDelete(const UINT *pDelLines, UINT s);
	void SendDataArrangeDeleted(const UINT *pDelLines, UINT s);
	void SendDataNextGeneratedEvent();

private:
	std::auto_ptr<RandomNumberGenerator> m_spRandomNumberGenerator;
	std::auto_ptr<BuildingBlock> m_spBuildingBlock;
	std::auto_ptr<MovingBlock> m_spMovingBlock;
	std::auto_ptr<MovingBlock> m_spNextMovingBlock;
	TetrixController *m_pTetrixController;
};


#endif