#include "HWIDBanListDialog.h"
#include <ui_hwidbanlistdialog.h>
#include <QMessageBox.h>

#include "GUI.h"
#include "IUserDatabase.h"

CHWIDBanListDialog::CHWIDBanListDialog(QWidget* parent) : QDialog(parent)
{
	m_pUI = new Ui::HWIDBanListDialog();
	m_pUI->setupUi(this);

	connect(m_pUI->UnbanBtn, &QPushButton::clicked, this, &CHWIDBanListDialog::Unban);
	connect(m_pUI->HWIDList, &QListWidget::currentItemChanged, this, &CHWIDBanListDialog::OnSelectHWID);

	InitBanList();
}

CHWIDBanListDialog::~CHWIDBanListDialog()
{
	delete m_pUI;
}

void CHWIDBanListDialog::InitBanList()
{
	std::vector<std::vector<unsigned char>> banList = g_pUserDatabase->GetHWIDBanList();

	m_pUI->HWIDList->clear();

	for (auto& hwid : banList)
	{
		QByteArray arr(reinterpret_cast<const char*>(hwid.data()), hwid.size());
		m_pUI->HWIDList->addItem(new QListWidgetItem(QString(arr.toHex())));
	}
}

void CHWIDBanListDialog::Unban()
{
	QListWidgetItem* item = m_pUI->HWIDList->currentItem();
	if (item)
	{
		QByteArray arr = QByteArray::fromHex(item->text().toLocal8Bit());

		if (g_pUserDatabase->UpdateHWIDBanList(std::vector<unsigned char>(arr.begin(), arr.end()), true) <= 0)
		{
			QMessageBox::critical(this, "Error", "An error occured while updating ban list");
		}
		else
		{
			m_pUI->HWIDList->removeItemWidget(item);
			delete item;
		}
	}
}

void CHWIDBanListDialog::OnSelectHWID(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (!current)
		return;

	std::vector<CUserData> users;
	QByteArray arr = QByteArray::fromHex(current->text().toLocal8Bit());

	if (g_pUserDatabase->GetUsersAssociatedWithHWID(std::vector<unsigned char>(arr.begin(), arr.end()), users) <= 0)
	{
		QMessageBox::critical(this, "Error", "An error occured while getting users");
		return;
	}

	m_pUI->UserList->clearContents();
	m_pUI->UserList->model()->removeRows(0, m_pUI->UserList->rowCount());

	int row = 0;
	for (auto& data : users)
	{
		m_pUI->UserList->insertRow(row);
		m_pUI->UserList->setItem(row, 0, new QTableWidgetItem(QString::number(data.userID)));
		m_pUI->UserList->setItem(row, 1, new QTableWidgetItem(data.userName.c_str()));
		row++;
	}
}

