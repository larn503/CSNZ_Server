#pragma once

#include <QDialog>

namespace Ui
{
	class HWIDBanListDialog;
}

class QListWidgetItem;

class CHWIDBanListDialog : public QDialog
{
	Q_OBJECT

public:
	CHWIDBanListDialog(QWidget* parent);
	~CHWIDBanListDialog();

public slots:
	void Unban();
	void OnSelectHWID(QListWidgetItem* current, QListWidgetItem* previous);

private:
	void InitBanList();

private:
	Ui::HWIDBanListDialog* m_pUI;
};