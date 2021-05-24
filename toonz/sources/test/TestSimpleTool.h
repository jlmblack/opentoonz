#pragma once

#ifndef TESTSIMPLETOOL_H
#define TESTSIMPLETOOL_H

#include "tools/tool.h"
#include "tools/tooloptions.h"
#include "toonz/txshsimplelevel.h"
#include "TestBrush.h"

// Tool Options Controller
class TestSimpleToolOptionsBox final : public ToolOptionsBox {
	
	Q_OBJECT

private:
	TTool* m_tool;

	QLabel* m_radiusLabel;
	ToolOptionSlider* m_radiusSlider;

public:
  TestSimpleToolOptionsBox(QWidget* parent, TTool* tool, 
													 TPaletteHandle* pltHandle,
													 ToolHandle* toolHandle);
  void updateStatus() override;
};


// Tool Class
class TestSimpleTool final: public TTool
{
public:
  TestSimpleTool(std::string name, int targetType);

	TTool::ToolType getToolType() const override;
	
	ToolOptionsBox* createOptionsBox() override;

	void mouseMove(const TPointD&, const TMouseEvent&) override;
  bool preLeftButtonDown() override;
	void leftButtonDown(const TPointD&, const TMouseEvent&) override;
	void leftButtonDrag(const TPointD&, const TMouseEvent&) override;
	void leftButtonUp(const TPointD&, const TMouseEvent&) override;
	
	int getCursorId() const override;
	
	void onActivate() override;
	void onDeactivate() override;
	void onImageChanged() override;
	void onEnter() override;
	void onLeave() override;
	void draw() override;
	bool isDragging() const override;

	TPropertyGroup* getProperties(int targetType) override;
	bool onPropertyChanged(std::string propertyName) override;

private:

	void startDrawing();
	void endDrawing();

	TXshSimpleLevel* getSimpleLevel();
	TRectD updateCursorInfo(const TPointD& pos);

	TPropertyGroup m_prop[1];

	const std::wstring TRIANGLE_KEY;
	const std::wstring SQUARE_KEY;
	const std::wstring PENTAGON_KEY;

	TDoubleProperty m_cursorRadius;
	TEnumProperty m_cursorType;

  TPointD m_cursorPos;
	TRectD m_cursorRect;

	TestBrush m_brush;
};


#endif