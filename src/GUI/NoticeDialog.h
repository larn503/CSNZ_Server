#pragma once

#include <QDialog>

namespace Ui
{
	class NoticeDialog;
}

class CNoticeDialog : public QDialog
{
	Q_OBJECT

public:
	CNoticeDialog();
	~CNoticeDialog();

private:
	Ui::NoticeDialog* m_pUI;
};