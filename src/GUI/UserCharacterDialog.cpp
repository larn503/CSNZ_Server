#include "UserCharacterDialog.h"
#include <ui_usercharacterdialog.h>
#include <QMessageBox.h>
#include <QDateTime.h>

#include "GUI.h"
#include "IUserDatabase.h"

CUserCharacterDialog::CUserCharacterDialog(QWidget* parent, int userID) : QDialog(parent)
{
	m_pUI = new Ui::UserCharacterDialog();
	m_pUI->setupUi(this);

	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

	m_nUserID = userID;

	Init();
}

void CUserCharacterDialog::Init()
{
	CUserCharacter character;
	character.flag = UFLAG_ALL;
	if (g_pUserDatabase->GetCharacter(m_nUserID, character) <= 0)
	{
		QMessageBox::critical(this, "Error", "Failed to get user character");
		close();
	}

	CUserData data;
	data.flag = UDATA_FLAG_USERNAME | UDATA_FLAG_REGISTERTIME | UDATA_FLAG_LASTLOGONTIME | UDATA_FLAG_FIRSTLOGONTIME;
	if (g_pUserDatabase->GetUserData(m_nUserID, data) <= 0)
	{
		QMessageBox::critical(this, "Error", "Failed to get user data");
		close();
	}

	m_pUI->Username->setText(QString::fromStdString(data.userName));
	m_pUI->Nickname->setText(QString::fromStdString(character.gameName));
	m_pUI->Level->setText(QString::number(character.level));
	m_pUI->Points->setText(QString::number(character.points));
	m_pUI->Cash->setText(QString::number(character.cash));
	m_pUI->ClanName->setText(QString::fromStdString(character.clanName));
	m_pUI->RegisterDate->setText(QDateTime::fromSecsSinceEpoch(data.registerTime * 60).toString("yyyy-MM-dd hh:mm:ss"));
	m_pUI->LastLoginDate->setText(QDateTime::fromSecsSinceEpoch(data.lastLogonTime * 60).toString("yyyy-MM-dd hh:mm:ss"));
	m_pUI->FirstLoginDate->setText(QDateTime::fromSecsSinceEpoch(data.firstLogonTime * 60).toString("yyyy-MM-dd hh:mm:ss"));
}

CUserCharacterDialog::~CUserCharacterDialog()
{
	delete m_pUI;
}