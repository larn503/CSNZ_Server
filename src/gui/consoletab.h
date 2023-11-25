#pragma once

#include <QWidget>
#include <QCompleter>

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
	bool eventFilter(QObject* obj, QEvent* event);

private:
	Ui::ConsoleTab* m_pUI;
	QCompleter* m_pCommandList;
	QCompleter* m_pCommandHistory;
};