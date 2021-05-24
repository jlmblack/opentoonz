#pragma once

#include <map>
#include <vector>

#include "trastercm.h"
#include "toonz/txshsimplelevel.h"

#undef DVAPI
#undef DVVAR
#ifdef TOONZLIB_EXPORTS
#define DVAPI DV_EXPORT_API
#define DVVAR DV_EXPORT_VAR
#else
#define DVAPI DV_IMPORT_API
#define DVVAR DV_IMPORT_VAR
#endif


class DVAPI TestBrush {

public:
	TestBrush();
	TestBrush(const TRasterCM32P& raster, int styleId, int radius);
	~TestBrush();

	void start(const TRasterCM32P& raster, int styleId, int radius);
	void end();

	TRectD draw(const TPointD& pos, TXshSimpleLevel* level);

	bool isDrawing()const;
	int getStyleId()const;
	int getRadius()const;
	std::vector<TPointD> getPoints()const;
	std::map<int, TPixelCM32> getValueMap()const;

private:
	bool m_drawing;
	TRasterCM32P m_targetRaster;
	int m_styleId;
	int m_radius;
	int m_tone;
	int m_toneIncrement;

	std::vector<TPointD> m_points;
	std::map<int, TPixelCM32> m_valueMap;
};