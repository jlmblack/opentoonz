#pragma once

#include "pane.h"


class TestPanelFactory final : public TPanelFactory
{
public:
	TestPanelFactory();
	void initialize(TPanel* panel) override;
	
};
