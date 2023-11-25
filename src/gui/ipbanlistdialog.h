#pragma once

#include <QDialog>

namespace Ui
{
	class IPBanListDialog;
}

class QListWidgetItem;

class CIPBanListDialog : public QDialog
{
	Q_OBJECT

public:
	CIPBanListDialog(QWidget* parent);
	~CIPBanListDialog();

public slots:
	void Unban();
	void OnSelectIP(QListWidgetItem* current, QListWidgetItem* previous);

private:
	void InitBanList();

private:
	Ui::IPBanListDialog* m_pUI;
};