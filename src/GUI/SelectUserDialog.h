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
	CSelectUserDialog(QWidget* parent, const std::vector<int>& users);
	~CSelectUserDialog();

	std::vector<int> GetSelectedUsers();
	void InitUserList();

public slots:
	void SelectClicked();
	void AddUserClicked();
	void DeleteUserClicked();

private:
	Ui::SelectUserDialog* m_pUI;
	std::vector<int> m_SelectedUsers;
};