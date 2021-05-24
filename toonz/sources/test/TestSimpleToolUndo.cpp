#include "toonz/txshsimplelevel.h"
#include "toonz/txsheethandle.h"

#include "TestSimpleToolUndo.h"
#include "TestBrush.h"

TestSimpleToolUndo::TestSimpleToolUndo(
	TXshSimpleLevel *level, const TFrameId &frameId, 
	const TestBrush& brush
)
 :
	ToolUtils::TToolUndo(level, frameId),
	m_styleId(brush.getStyleId()),
	m_radius(brush.getRadius()),
	m_points(brush.getPoints()), 
	m_valueMap(brush.getValueMap())
{

}


TestSimpleToolUndo::~TestSimpleToolUndo()
{

}


void TestSimpleToolUndo::undo() const
{

	TTool::Application *app = TTool::getApplication();

	TRasterCM32P image = getImage();
	if (!image) {
		return;
	}

	int width = image->getSize().lx;
	int height = image->getSize().ly;

	for (auto& it : m_valueMap)
	{
		int index = it.first;

		int y = index / width;
		int x = index % width;

		TPixelCM32* outPix = image->pixels(y) + x;
		*outPix = it.second;
	}

	// update canvas
	app->getCurrentXsheet()->notifyXsheetChanged();
	
	// update thumbnails
	notifyImageChanged();
}



void TestSimpleToolUndo::redo() const
{
	TRasterCM32P image = getImage();
	if (!image) {
		return;
	}
	TestBrush brush(image, m_styleId, m_radius);

	for (const TPointD& pt : m_points) {
		brush.draw(pt, m_level.getPointer());
	}

	TTool::Application *app = TTool::getApplication();
	// update canvas
	app->getCurrentXsheet()->notifyXsheetChanged();
	
	// update thumbnails
	notifyImageChanged();
}


int TestSimpleToolUndo::getSize() const
{
	return ((sizeof(int) + sizeof(TPixelCM32)) *  m_valueMap.size() + 
		sizeof(TPointD) * m_points.size());
}



TRasterCM32P TestSimpleToolUndo::getImage() const
{
	if (m_level->isFid(m_frameId)){
		TToonzImageP toonzImage = (TToonzImageP)m_level->getFrame(m_frameId, true);
		if (toonzImage) {
			return toonzImage->getRaster();
		}
	}

	return 0;
}


QString TestSimpleToolUndo::getHistoryString() 
{
	return QString("Test Brush");
}



QString TestSimpleToolUndo::getToolName()
{
	return QString("Test Brush");
}

