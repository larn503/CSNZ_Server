#include "UserCharacterDialog.h"
#include <ui_usercharacterdialog.h>

#include "IUserDatabase.h"

CUserCharacterDialog::CUserCharacterDialog(QWidget* parent) : QDialog(parent)
{
	m_pUI = new Ui::UserCharacterDialog();
	m_pUI->setupUi(this);
}

CUserCharacterDialog::~CUserCharacterDialog()
{
	delete m_pUI;
}