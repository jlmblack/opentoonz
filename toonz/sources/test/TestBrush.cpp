#include <QDebug>

#include "tools/tool.h"
#include "tools/toolutils.h"
#include "toonz/tapplication.h"
#include "toonz/txsheethandle.h"

#include "TestBrush.h"


// ====================================================================
// TestBrush
TestBrush::TestBrush():
	m_drawing(false),
	m_targetRaster(0),
	m_styleId(0),
	m_tone(0),
	m_toneIncrement(1)
{

}


TestBrush::TestBrush(const TRasterCM32P& raster, int styleId, int radius):
	m_drawing(true),
	m_targetRaster(raster),
	m_styleId(styleId),
	m_radius(radius),
	m_tone(0),
	m_toneIncrement(1)
{
}

TestBrush::~TestBrush()
{

}


void TestBrush::start(const TRasterCM32P& raster, int styleId, int radius)
{
	m_drawing = true;
	m_targetRaster = raster;
	m_styleId = styleId;
	m_radius = radius;
	m_tone = 0;
	m_toneIncrement = 1;

	m_points.clear();
	m_valueMap.clear();
}


void TestBrush::end()
{
	m_drawing = false;
}


TRectD TestBrush::draw(const TPointD& pos, TXshSimpleLevel* level)
{
	if (!m_drawing) {
		return TRectD(0, 0, 0, 0);
	}

	int width = m_targetRaster->getSize().lx;
	int height = m_targetRaster->getSize().ly;

	int y = pos.y;
	int x = pos.x;

	if (y < 0 || height <= y || x < 0 || width <= x) {
		return TRectD(0, 0, 0, 0);
	}

	int start_y = std::max(0, y - m_radius);
	int end_y = std::min(height - 1, y + m_radius);

	int start_x = std::max(0, x - m_radius);
	int end_x = std::min(width - 1, x + m_radius);

	m_points.push_back(pos);

	int otherStyleId = m_styleId + 1;
	if (level->getPalette()->getStyleCount() <= otherStyleId) {
		otherStyleId = m_styleId - 1;
	}


	for (int iy = start_y; iy <= end_y; ++iy) {
		TPixelCM32* outPix = m_targetRaster->pixels(iy) + start_x;
		for (int ix = start_x; ix <= end_x; ++ix, ++outPix) {
			int index = iy * width + ix;
			if (m_valueMap.find(index) == m_valueMap.end()) {
				m_valueMap[index] = *outPix;
			}
			*outPix = TPixelCM32(otherStyleId, m_styleId, m_tone);
		}
	}

	TRectD updatedRect = TRectD(start_x, start_y, end_x, end_y);

	m_tone += m_toneIncrement;
	if (m_tone >= 256) {
		m_tone = 254;
		m_toneIncrement = -1;
	}
	else if (m_tone < 0) {
		m_tone = 0;
		m_toneIncrement = 1;
	}

	return updatedRect;
}


bool TestBrush::isDrawing() const
{
	return m_drawing;
}


int TestBrush::getStyleId() const
{
	return m_styleId;
}


int TestBrush::getRadius() const
{
	return m_radius;
}


std::vector<TPointD> TestBrush::getPoints() const
{
	return m_points;
}


std::map<int, TPixelCM32> TestBrush::getValueMap() const
{
	return m_valueMap;
}
