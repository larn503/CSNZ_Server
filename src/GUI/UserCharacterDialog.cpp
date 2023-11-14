#include "UserCharacterDialog.h"
#include <ui_usercharacterdialog.h>
#include <QMessageBox.h>

#include "GUI.h"
#include "IUserDatabase.h"

CUserCharacterDialog::CUserCharacterDialog(QWidget* parent, int userID) : QDialog(parent)
{
	m_pUI = new Ui::UserCharacterDialog();
	m_pUI->setupUi(this);

	m_nUserID = userID;

	Init();
}

void CUserCharacterDialog::Init()
{
	CUserCharacter character;
	character.flag = UFLAG_ALL;
	if (g_pUserDatabase->GetCharacter(m_nUserID, character) <= 0)
	{
		// error
		QMessageBox::critical(this, "Error", "Failed to get user character");
		close();
	}

	CUserData data;
	data.flag = UDATA_FLAG_USERNAME | UDATA_FLAG_REGISTERTIME | UDATA_FLAG_LASTLOGONTIME | UDATA_FLAG_FIRSTLOGONTIME;
	if (g_pUserDatabase->GetUserData(m_nUserID, data) <= 0)
	{
		// error
		QMessageBox::critical(this, "Error", "Failed to get user data");
		close();
	}

	m_pUI->Username->setText(QString::fromStdString(data.userName));
	m_pUI->Nickname->setText(QString::fromStdString(character.gameName));
	m_pUI->Level->setText(QString::number(character.level));
	m_pUI->Points->setText(QString::number(character.points));
	m_pUI->Cash->setText(QString::number(character.cash));
	m_pUI->ClanName->setText(QString::fromStdString(character.clanName));
	m_pUI->RegisterDate->setText(QString::number(data.registerTime));
	m_pUI->LastLoginDate->setText(QString::number(data.lastLogonTime));
	m_pUI->FirstLoginDate->setText(QString::number(data.firstLogonTime));
}

CUserCharacterDialog::~CUserCharacterDialog()
{
	delete m_pUI;
}