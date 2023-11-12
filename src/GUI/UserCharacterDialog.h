#pragma once

#include <QDialog>

namespace Ui
{
	class UserCharacterDialog;
}

class CUserCharacterDialog : public QDialog
{
	Q_OBJECT

public:
	CUserCharacterDialog(QWidget* parent);
	~CUserCharacterDialog();

private:
	Ui::UserCharacterDialog* m_pUI;
};