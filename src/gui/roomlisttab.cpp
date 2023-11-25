#include "roomlisttab.h"
#include <ui_roomlisttab.h>

CRoomListTab::CRoomListTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::RoomListTab();
	m_pUI->setupUi(this);
}

CRoomListTab::~CRoomListTab()
{
	delete m_pUI;
}