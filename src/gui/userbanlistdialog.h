#pragma once

#include <QDialog>

namespace Ui
{
	class UserBanListDialog;
}

class CUserBanListDialog : public QDialog
{
	Q_OBJECT

public:
	CUserBanListDialog(QWidget* parent);
	~CUserBanListDialog();

public slots:
	void Unban();

private:
	void InitBanList();

private:
	Ui::UserBanListDialog* m_pUI;
};