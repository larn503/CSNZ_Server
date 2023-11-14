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
	CUserCharacterDialog(QWidget* parent, int userID);
	~CUserCharacterDialog();

private:
	void Init();

private:
	Ui::UserCharacterDialog* m_pUI;
	int m_nUserID;
};