#pragma once

#include <QDialog>

namespace Ui
{
	class SelectUserDialog;
}

class CSelectUserDialog : public QDialog
{
	Q_OBJECT

public:
	CSelectUserDialog(QWidget* parent = nullptr);
	~CSelectUserDialog();

	std::vector<int> GetSelectedUsers();

public slots:
	void SelectClicked();
	void AddUserClicked();
	void DeleteUserClicked();

private:
	Ui::SelectUserDialog* m_pUI;
	std::vector<int> m_SelectedUsers;
};