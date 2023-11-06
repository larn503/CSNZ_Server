#include "SelectUserDialog.h"
#include <ui_selectuserdialog.h>

CSelectUserDialog::CSelectUserDialog(QWidget* parent) : QDialog(parent)
{
	m_pUI = new Ui::SelectUserDialog();
	m_pUI->setupUi(this);
	
	connect(m_pUI->CancelBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(m_pUI->ConfirmBtn, SIGNAL(clicked()), this, SLOT(SelectClicked()));
	connect(m_pUI->AddBtn, SIGNAL(clicked()), this, SLOT(AddUserClicked()));
	connect(m_pUI->DeleteBtn, SIGNAL(clicked()), this, SLOT(DeleteUserClicked()));

	// test
	m_pUI->UserTable->insertRow(0);
	m_pUI->UserTable->setItem(0, 0, new QTableWidgetItem("1"));
	m_pUI->UserTable->setItem(0, 1, new QTableWidgetItem("test1"));

	m_pUI->UserTable->insertRow(1);
	m_pUI->UserTable->setItem(1, 0, new QTableWidgetItem("2"));
	m_pUI->UserTable->setItem(1, 1, new QTableWidgetItem("test2"));
}

CSelectUserDialog::~CSelectUserDialog()
{
	delete m_pUI;
}

std::vector<int> CSelectUserDialog::GetSelectedUsers()
{
	return m_SelectedUsers;
}

void CSelectUserDialog::SelectClicked()
{
	for (int i = 0; i < m_pUI->SelectedUserTable->rowCount(); i++)
	{
		m_SelectedUsers.push_back(m_pUI->SelectedUserTable->item(i, 0)->text().toInt()); // userID
	}

	close();
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

		m_pUI->SelectedUserTable->removeRow(idx.row());
		selected = selections->selectedRows();
	}
}