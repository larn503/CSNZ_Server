#pragma once

#include <QDialog>
#undef slots
#include "definitions.h"
#define slots Q_SLOTS

namespace Ui
{
	class BanDialog;
}

class CBanDialog : public QDialog
{
	Q_OBJECT

public:
	CBanDialog(QWidget* parent, const Session& session);
	~CBanDialog();

public:
	void CheckInfo();

public slots:
	void Ban();
	void OnBanTypeChanged(const QString& text);

private:
	Ui::BanDialog* m_pUI;
	Session m_Session;
};