#include "SessionTab.h"
#include <ui_sessiontab.h>

CSessionTab::CSessionTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::SessionTab();
	m_pUI->setupUi(this);
}

CSessionTab::~CSessionTab()
{
	delete m_pUI;
}