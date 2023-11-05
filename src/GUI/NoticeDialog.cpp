#include "NoticeDialog.h"
#include <ui_noticedialog.h>

CNoticeDialog::CNoticeDialog() : QDialog(NULL)
{
	m_pUI = new Ui::NoticeDialog();
	m_pUI->setupUi(this);
}

CNoticeDialog::~CNoticeDialog()
{
	delete m_pUI;
}
