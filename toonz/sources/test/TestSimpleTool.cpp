#include <cmath>
#include <QHBoxLayout>
#include <QDebug>

#include "tools/toolutils.h"
#include "tools/tool.h"
#include "toonz/tapplication.h"
#include "toonz/tcolumnhandle.h"
#include "toonz/tframehandle.h"
#include "toonz/tpalettehandle.h"
#include "toonz/palettecontroller.h"
#include "tools/toolhandle.h"
#include "tools/toolcommandids.h"
#include "tools/tooloptions.h"
#include "tooloptionscontrols.h"
#include "toonz/txshlevelhandle.h"
#include "toonz/txshsimplelevel.h"

#include "TestSimpleTool.h"
#include "TestSimpleToolUndo.h"

#include "qcoreapplication.h"

#include "tgl.h"


namespace {
static const double PI = 3.1415926;
static const double cos18 = std::cos(18 * PI / 180);
static const double sin18 = std::sin(18 * PI / 180);
static const double cos54 = std::cos(54 * PI / 180);
static const double sin54 = std::sin(54 * PI / 180);
static const double cos30 = std::cos(30 * PI / 180);
static const double sin30 = std::sin(30 * PI / 180);


void drawTriangle(const TPointD& center, double radius) {
  double dx = radius * cos30;
  double dy = radius * sin30;
  
  glBegin(GL_LINE_LOOP);

  glVertex2d(center.x, center.y + radius);
  glVertex2d(center.x - dx, center.y - dy);
  glVertex2d(center.x + dx, center.y - dy);
  
  glEnd();
}


void drawSquare(const TPointD& center, double radius) {
  glBegin(GL_LINE_LOOP);

  glVertex2d(center.x + radius, center.y + radius);
  glVertex2d(center.x - radius, center.y + radius);
  glVertex2d(center.x - radius, center.y - radius);
  glVertex2d(center.x + radius, center.y - radius);

  glEnd();
}

void drawPentagon(const TPointD& center, double radius) {
  double dx1 = radius * cos18;
  double dx2 = radius * cos54;
  double dy1 = radius * sin18;
  double dy2 = radius * sin54;

  glBegin(GL_LINE_LOOP);

  glVertex2d(center.x, center.y + radius);
  glVertex2d(center.x + dx1, center.y + dy1);
  glVertex2d(center.x + dx2, center.y - dy2);
  glVertex2d(center.x - dx2, center.y - dy2);
  glVertex2d(center.x - dx1, center.y + dy1);
  
  glEnd();
}
}


//===============================================================
// TestSimpleToolOptionsBox

TestSimpleToolOptionsBox::TestSimpleToolOptionsBox(
  QWidget* parent, TTool* tool, TPaletteHandle* pltHandle, ToolHandle* toolHandle
) :
  ToolOptionsBox(parent), 
  m_tool(tool),
  m_radiusLabel(0), 
  m_radiusSlider(0)
{
  TPropertyGroup* props = tool->getProperties(0);
  ToolOptionControlBuilder builder(this, tool, pltHandle, toolHandle);
  if (tool && props) {
    props->accept(builder);
  }

  m_radiusSlider = dynamic_cast<ToolOptionSlider *>(m_controls.value("Radius"));
  m_radiusLabel = m_labels.value(m_radiusSlider->propertyName());

  hLayout()->addStretch(1);

}
  


void TestSimpleToolOptionsBox::updateStatus() {
  for (auto& it : m_controls)
  {
    it->updateStatus();
  }
}


//===============================================================
// TestSimpleTool

TestSimpleTool::TestSimpleTool(std::string name, int targetType): 
	TTool(name),
  TRIANGLE_KEY(L"triangle"),
  SQUARE_KEY(L"square"),
  PENTAGON_KEY(L"pentagon"),
  m_cursorPos(0, 0),
  m_cursorRect(-20, -20, 20, 20),
  m_cursorRadius("Radius", 1, 1000, 5),
  m_cursorType("Cursor Type")
{
  bind(targetType);

  m_prop[0].bind(m_cursorRadius);
  m_prop[0].bind(m_cursorType);

  m_cursorType.addValue(TRIANGLE_KEY);
  m_cursorType.addValue(SQUARE_KEY);
  m_cursorType.addValue(PENTAGON_KEY);
}


TTool::ToolType TestSimpleTool::getToolType() const 
{ return TTool::LevelWriteTool; }


ToolOptionsBox* TestSimpleTool::createOptionsBox()
{
  TPaletteHandle *currPalette =
    TTool::getApplication()->getPaletteController()->getCurrentLevelPalette();
  ToolHandle *currTool = TTool::getApplication()->getCurrentTool();

  return new TestSimpleToolOptionsBox(0, this, currPalette, currTool); 
}


void TestSimpleTool::mouseMove(const TPointD& pos, const TMouseEvent& e)
{
  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  TRectD updatedArea = updateCursorInfo(pos);

  invalidate(updatedArea);
}


bool TestSimpleTool::preLeftButtonDown()
{ 
  touchImage();
  if (m_isFrameCreated) {
    TTool::Application* app = TTool::getApplication();
    if (app->getCurrentFrame()->isEditingLevel()) {
      invalidate();
    }
  }

  return true;
}


void TestSimpleTool::leftButtonDown(const TPointD& pos, const TMouseEvent&)
{
  TRectD updatedArea = updateCursorInfo(pos);

  TImageP img = getImage(true);
  if (!img) {
    img = touchImage();
  }
  if (!img) return;

  if (!m_brush.isDrawing()) {
    startDrawing();
  }

  TToonzImageP toonzImg(img);
  TRasterCM32P ras = toonzImg->getRaster();
  
  TPointD realPos = pos + toonzImg->getRaster()->getCenterD();
  TXshSimpleLevel* level = getSimpleLevel();

  TRectD drawedRect = m_brush.draw(realPos, level);

  updatedArea = updatedArea + drawedRect;
  invalidate(updatedArea);
}


void TestSimpleTool::leftButtonDrag(const TPointD& pos, const TMouseEvent&)
{
  TRectD updatedArea = updateCursorInfo(pos);

  TToonzImageP toonzImg   = TImageP(getImage(true));
  TPointD realPos = pos + toonzImg->getRaster()->getCenterD();
  TXshSimpleLevel* level = getSimpleLevel();

  TRectD drawdRect = m_brush.draw(realPos, level);
  updatedArea = updatedArea + drawdRect;
  
  invalidate(updatedArea);
}


void TestSimpleTool::leftButtonUp(const TPointD& pos, const TMouseEvent&)
{
  endDrawing();

  TRectD updatedArea = updateCursorInfo(pos);
  invalidate(updatedArea);
}


void TestSimpleTool::startDrawing()
{
  if (m_brush.isDrawing()){
    endDrawing();
  }

  TTool::Application* app = TTool::getApplication();

  int col   = app->getCurrentColumn()->getColumnIndex();
  m_enabled = col >= 0 || app->getCurrentFrame()->isEditingLevel();
  if (!m_enabled) return;

  TImageP img = getImage(true);

  if (!img) {
    return;
  }

  TToonzImageP toonzImg(img);
  TRasterCM32P ras = toonzImg->getRaster();

  int styleId = app->getCurrentLevelStyleIndex();
  m_brush.start(ras, app->getCurrentLevelStyleIndex(), m_cursorRadius.getValue());
}


void TestSimpleTool::endDrawing()
{
  if (!m_brush.isDrawing()) {
    return;
  }
  
  m_brush.end();
  TXshSimpleLevel* level = getSimpleLevel();
  TFrameId frameId = getCurrentFid();

  TUndo* undo = new TestSimpleToolUndo(level, frameId, m_brush);
  TUndoManager::manager()->add(undo);
  notifyImageChanged();
}


TXshSimpleLevel* TestSimpleTool::getSimpleLevel()
{
  TTool::Application* app = TTool::getApplication();
  TXshLevel* level = app->getCurrentLevel()->getLevel();
  TXshSimpleLevelP simpleLevel = level->getSimpleLevel();
  return simpleLevel.getPointer();
}


int TestSimpleTool::getCursorId() const
{ return ToolCursor::PenCursor; }


void TestSimpleTool::onActivate()
{
  qDebug() << TTool::getApplication();
  startDrawing();
}


void TestSimpleTool::onDeactivate()
{
  endDrawing();
}


void TestSimpleTool::onImageChanged()
{

}


void TestSimpleTool::onEnter()
{

}


void TestSimpleTool::onLeave()
{

}


void TestSimpleTool::draw()
{ 
  glColor3d(1.f, 0.f, 0.f); 

  if (m_cursorType.getValue() == TRIANGLE_KEY) {
    drawTriangle(m_cursorPos, m_cursorRadius.getValue());
  }
  else if (m_cursorType.getValue() == SQUARE_KEY) {
    drawSquare(m_cursorPos, m_cursorRadius.getValue());
  }
  else if(m_cursorType.getValue() == PENTAGON_KEY){
    drawPentagon(m_cursorPos, m_cursorRadius.getValue());
  }
}


bool TestSimpleTool::isDragging() const
{
  return false;
}



TPropertyGroup* TestSimpleTool::getProperties(int targetType)
{
  return &m_prop[0];
}


bool TestSimpleTool::onPropertyChanged(std::string propertyName)
{
  if(propertyName == m_cursorRadius.getName()){
    TRectD updatingArea = updateCursorInfo(m_cursorPos);
    invalidate(updatingArea);
  }
  else if (propertyName == m_cursorType.getName()) {
    TRectD updatingArea = updateCursorInfo(m_cursorPos);
    invalidate(updatingArea);
  }

  return true;
}


TRectD TestSimpleTool::updateCursorInfo(const TPointD& pos)
{
  TPointD cursorSize(m_cursorRadius.getValue(), m_cursorRadius.getValue());
  TRectD newCursorRect(pos - cursorSize, pos + cursorSize);
  TRectD updatedArea = m_cursorRect + newCursorRect;
  m_cursorRect = newCursorRect;
  m_cursorPos = pos;
  return updatedArea;
}



// instanciate
TestSimpleTool testTool(T_Test_Simple_Tool, TTool::ToonzImage | TTool::EmptyTarget);