#include "SelectUserDialog.h"
#include "GUI.h"
#include "IUserDatabase.h"

#include <ui_selectuserdialog.h>

CSelectUserDialog::CSelectUserDialog(QWidget* parent, const std::vector<int>& users) : QDialog(parent)
{
	m_pUI = new Ui::SelectUserDialog();
	m_pUI->setupUi(this);
	
	m_SelectedUsers = users;

	connect(m_pUI->CancelBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(m_pUI->ConfirmBtn, SIGNAL(clicked()), this, SLOT(SelectClicked()));
	connect(m_pUI->AddBtn, SIGNAL(clicked()), this, SLOT(AddUserClicked()));
	connect(m_pUI->DeleteBtn, SIGNAL(clicked()), this, SLOT(DeleteUserClicked()));

	InitUserList();
}

CSelectUserDialog::~CSelectUserDialog()
{
	delete m_pUI;
}

std::vector<int> CSelectUserDialog::GetSelectedUsers()
{
	return m_SelectedUsers;
}

void CSelectUserDialog::InitUserList()
{
	std::vector<UserSession> sessions;
	if (g_pUserDatabase->GetUserSessions(sessions) <= 0)
		GUI()->ShowMessageBox("Error", "something went wrong");

	int userListCounter = 0, selectedUserCounter = 0;
	for (auto session : sessions)
	{
		if (std::find(m_SelectedUsers.begin(), m_SelectedUsers.end(), session.userID) != m_SelectedUsers.end())
		{
			// add to selected list
			m_pUI->SelectedUserTable->insertRow(selectedUserCounter);
			m_pUI->SelectedUserTable->setItem(selectedUserCounter, 0, new QTableWidgetItem(QString::number(session.userID)));
			m_pUI->SelectedUserTable->setItem(selectedUserCounter, 1, new QTableWidgetItem(session.userName.c_str()));
			selectedUserCounter++;
		}
		else
		{
			m_pUI->UserTable->insertRow(userListCounter);
			m_pUI->UserTable->setItem(userListCounter, 0, new QTableWidgetItem(QString::number(session.userID)));
			m_pUI->UserTable->setItem(userListCounter, 1, new QTableWidgetItem(session.userName.c_str()));
			userListCounter++;
		}
	}
}

void CSelectUserDialog::SelectClicked()
{
	close();
	setResult(1); // set flag that we can read selected user table
}

void CSelectUserDialog::AddUserClicked()
{
	QItemSelectionModel* selections = m_pUI->UserTable->selectionModel();
	QModelIndexList selected = selections->selectedRows();
	while (!selected.isEmpty())
	{
		const QModelIndex& idx = selected.at(0);

		// insert new row in the SelectedUserTable
		m_pUI->SelectedUserTable->insertRow(m_pUI->SelectedUserTable->rowCount());

		// iterate row cells
		for (int j = 0; j < 2; j++)
		{
			// add items
			int row = idx.row();
			QTableWidgetItem* item = m_pUI->UserTable->item(row, j);

			m_pUI->SelectedUserTable->setItem(m_pUI->SelectedUserTable->rowCount() - 1, j, new QTableWidgetItem(item->text()));
		}

		// update selected users vector
		int userid = m_pUI->UserTable->item(idx.row(), 0)->text().toInt();
		m_SelectedUsers.push_back(userid);

		m_pUI->UserTable->removeRow(idx.row());
		selected = selections->selectedRows();
	}
}

void CSelectUserDialog::DeleteUserClicked()
{
	// move rows back to user list
	QItemSelectionModel* selections = m_pUI->SelectedUserTable->selectionModel();
	QModelIndexList selected = selections->selectedRows();
	while (!selected.isEmpty())
	{
		const QModelIndex& idx = selected.at(0);

		// insert new row in the UserTable
		m_pUI->UserTable->insertRow(m_pUI->UserTable->rowCount());

		// iterate row cells
		for (int j = 0; j < 2; j++)
		{
			// add items
			int row = idx.row();
			QTableWidgetItem* item = m_pUI->SelectedUserTable->item(row, j);

			m_pUI->UserTable->setItem(m_pUI->UserTable->rowCount() - 1, j, new QTableWidgetItem(item->text()));
		}

		// update selected users vector
		int userid = m_pUI->SelectedUserTable->item(idx.row(), 0)->text().toInt();
		m_SelectedUsers.erase(std::remove(m_SelectedUsers.begin(), m_SelectedUsers.end(), userid), m_SelectedUsers.end());

		m_pUI->SelectedUserTable->removeRow(idx.row());
		selected = selections->selectedRows();
	}
}