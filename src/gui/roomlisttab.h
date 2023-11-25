#pragma once

#include <QWidget>

namespace Ui
{
	class RoomListTab;
}

class CRoomListTab : public QWidget
{
	Q_OBJECT

public:
	CRoomListTab(QWidget* parent = nullptr);
	~CRoomListTab();

private:
	Ui::RoomListTab* m_pUI;
};