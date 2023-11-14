#include "UserBanListDialog.h"
#include <ui_userbanlistdialog.h>
#include <QMessageBox.h>
#include <QDateTime.h>

#include "GUI.h"
#include "IUserDatabase.h"

CUserBanListDialog::CUserBanListDialog(QWidget* parent) : QDialog(parent)
{
	m_pUI = new Ui::UserBanListDialog();
	m_pUI->setupUi(this);

	connect(m_pUI->UnbanBtn, &QPushButton::clicked, this, &CUserBanListDialog::Unban);

	InitBanList();
}

CUserBanListDialog::~CUserBanListDialog()
{
	delete m_pUI;
}

void CUserBanListDialog::InitBanList()
{
	std::map<int, UserBan> banList = g_pUserDatabase->GetUserBanList();
	
	int row = 0;
	for (auto& b : banList)
	{
		int userID = b.first;
		UserBan ban = b.second;

		m_pUI->UserList->insertRow(row);
		m_pUI->UserList->setItem(row, 0, new QTableWidgetItem(QString::number(userID)));
		m_pUI->UserList->setItem(row, 1, new QTableWidgetItem(""));
		m_pUI->UserList->setItem(row, 2, new QTableWidgetItem(QString::number(ban.banType)));
		m_pUI->UserList->setItem(row, 3, new QTableWidgetItem(QDateTime::fromSecsSinceEpoch(ban.term * 60).toString("yyyy-MM-dd hh:mm:ss")));
		m_pUI->UserList->setItem(row, 4, new QTableWidgetItem(ban.reason.c_str()));
	}
}

void CUserBanListDialog::Unban()
{
	std::vector<int> userList;

	QItemSelectionModel* selections = m_pUI->UserList->selectionModel();
	QModelIndexList selected = selections->selectedRows();
	while (!selected.isEmpty())
	{
		const QModelIndex& idx = selected.at(0);

		int userID = m_pUI->UserList->item(idx.row(), 0)->text().toInt();
		userList.push_back(userID);

		m_pUI->UserList->removeRow(idx.row());
		selected = selections->selectedRows();
	}

	UserBan ban;
	ban.banType = 0; // delete ban
	for (auto userID : userList)
	{
		g_pUserDatabase->UpdateUserBan(userID, ban);
	}
}