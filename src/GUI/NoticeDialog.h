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
	CNoticeDialog(QWidget* parent = nullptr);
	~CNoticeDialog();

public slots:
	void SendClicked();
	void SelectUsersClicked();
	void TextChanged();

private:
	Ui::NoticeDialog* m_pUI;
	std::vector<int> m_SelectedUsers;
};