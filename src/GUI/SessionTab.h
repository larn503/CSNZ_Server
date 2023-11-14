#pragma once

#include <QWidget>
#undef slots
#include "Definitions.h"
#define slots Q_SLOTS

namespace Ui
{
	class SessionTab;
}

class CSessionTab : public QWidget
{
	Q_OBJECT

public:
	CSessionTab(QWidget* parent = nullptr);
	~CSessionTab();

public:
	void OnOpenUserCharacterDialog(int userID);

public slots:
	void Refresh();
	void Kick();
	void Ban();
	void ShowOnlyLoggedInToggled(bool checked);
	void keyPressEvent(QKeyEvent* event);
	void OnSessionListUpdated(const std::vector<Session>& sessions);
	void HandleContextMenu(const QPoint& pos);

private:
	Ui::SessionTab* m_pUI;
	bool m_bRefresing;
	std::vector<Session> m_Sessions;
};