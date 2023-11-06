#pragma once

#include <QWidget>

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

public slots:
	void Refresh();
	void Kick();
	void Ban();
	void ShowOnlyLoggedInToggled(bool checked);
	void keyPressEvent(QKeyEvent* event);

private:
	Ui::SessionTab* m_pUI;
};