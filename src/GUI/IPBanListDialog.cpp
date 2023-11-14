#include "IPBanListDialog.h"
#include <ui_ipbanlistdialog.h>
#include <QMessageBox.h>

#include "GUI.h"
#include "IUserDatabase.h"

CIPBanListDialog::CIPBanListDialog(QWidget* parent) : QDialog(parent)
{
	m_pUI = new Ui::IPBanListDialog();
	m_pUI->setupUi(this);

	connect(m_pUI->UnbanBtn, &QPushButton::clicked, this, &CIPBanListDialog::Unban);
	connect(m_pUI->IPList, &QListWidget::currentItemChanged, this, &CIPBanListDialog::OnSelectIP);

	InitBanList();
}

CIPBanListDialog::~CIPBanListDialog()
{
	delete m_pUI;
}

void CIPBanListDialog::InitBanList()
{
	std::vector<std::string> banList = g_pUserDatabase->GetIPBanList();
	
	m_pUI->IPList->clear();

	for (auto& ip : banList)
	{
		m_pUI->IPList->addItem(new QListWidgetItem(QString::fromStdString(ip)));
	}
}

void CIPBanListDialog::Unban()
{
	QListWidgetItem* item = m_pUI->IPList->currentItem();
	if (item)
	{
		if (g_pUserDatabase->UpdateIPBanList(item->text().toStdString(), true) <= 0)
		{
			QMessageBox::critical(this, "Error", "An error occured while updating ban list");
		}
		else
		{
			m_pUI->IPList->removeItemWidget(item);
			delete item;
		}
	}
}

void CIPBanListDialog::OnSelectIP(QListWidgetItem* current, QListWidgetItem* previous)
{
	m_pUI->UserList->clearContents();
	m_pUI->UserList->model()->removeRows(0, m_pUI->UserList->rowCount());

	if (!current)
	{
		return;
	}

	std::vector<CUserData> users;
	if (g_pUserDatabase->GetUsersAssociatedWithIP(current->text().toStdString(), users) <= 0)
	{
		QMessageBox::critical(this, "Error", "An error occured while getting users");
		return;
	}

	int row = 0;
	for (auto& data : users)
	{
		m_pUI->UserList->insertRow(row);
		m_pUI->UserList->setItem(row, 0, new QTableWidgetItem(QString::number(data.userID)));
		m_pUI->UserList->setItem(row, 1, new QTableWidgetItem(data.userName.c_str()));
		row++;
	}
}
