#include "ConsoleTab.h"
#include "../Console.h"
#include <ui_consoletab.h>
#include <string>

CConsoleTab::CConsoleTab(QWidget* parent) : QWidget(parent)
{
	m_pUI = new Ui::ConsoleTab();
	m_pUI->setupUi(this);

	connect(m_pUI->SubmitBtn, &QPushButton::clicked, this, &CConsoleTab::SubmitClicked);
	connect(m_pUI->Entry, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged(const QString&)));
}

CConsoleTab::~CConsoleTab()
{
	delete m_pUI;
}

// prints log message to console
void CConsoleTab::Log(int level, const std::string& msg)
{
	// set color for message
	switch (level)
	{
	case CON_ERROR:
	case CON_FATAL_ERROR:
		m_pUI->History->setTextColor(Qt::red);
		break;
	case CON_WARNING:
		m_pUI->History->setTextColor(Qt::yellow);
		break;
	default:
		m_pUI->History->setTextColor(Qt::black);
	};

	// move to the end and insert text
	m_pUI->History->moveCursor(QTextCursor::End);
	m_pUI->History->insertPlainText(QString::fromLocal8Bit(msg.c_str()));

	// restore color
	m_pUI->History->setTextColor(Qt::black);
}

void CConsoleTab::SubmitClicked()
{

}

void CConsoleTab::TextChanged(const QString& text)
{
	// TODO: command suggestions (handle text changing and find way to get command list)
}
