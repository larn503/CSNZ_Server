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

private:
	Ui::SessionTab* m_pUI;
};