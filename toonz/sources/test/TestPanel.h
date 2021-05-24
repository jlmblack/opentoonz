#pragma once

#include <QWidget>

class TestPanel final : public QWidget {
	Q_OBJECT

public:
	TestPanel(QWidget* parent);
	~TestPanel();
};