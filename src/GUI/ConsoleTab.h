#pragma once

#include <QWidget>

namespace Ui
{
	class ConsoleTab;
}

class CConsoleTab : public QWidget
{
    Q_OBJECT

public:
	CConsoleTab(QWidget* parent = nullptr);
	~CConsoleTab();
	
public slots:
	void Log(int level, const std::string& msg);
	void SubmitClicked();
	void TextChanged(const QString& text);

private:
	Ui::ConsoleTab* m_pUI;
};