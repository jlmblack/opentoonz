#pragma once

#include <vector>
#include <map>

#include "tundo.h"
#include "trastercm.h"
#include "tools/toolutils.h"
#include "ttoonzimage.h"
#include "trasterimage.h"


#include "TestBrush.h"


class TestSimpleToolUndo : public ToolUtils::TToolUndo {

public:
	TestSimpleToolUndo(
		TXshSimpleLevel *level, const TFrameId &frameId, 
		const TestBrush& btush
	);
		
	~TestSimpleToolUndo();

	void undo()const override;
	void redo()const override;
	int getSize()const override;
	QString getHistoryString()override;
	QString getToolName()override;

private:

	TRasterCM32P getImage() const;

	int m_styleId;
	int m_radius;
	std::vector<TPointD> m_points;
	std::map<int, TPixelCM32> m_valueMap;
};