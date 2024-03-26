#pragma once

#include <string>
#include <vector>

class CCommand
{
public:
	CCommand(const std::string& name, const std::string& desc, const std::string& usage, const std::function<void(CCommand*, const std::vector<std::string>&)>& func);
	~CCommand();

	std::string GetName();
	std::string GetDescription();
	std::string GetUsage();

	void Exec(const std::vector<std::string>& args);

private:
	std::string m_Name;
	std::string m_Description;
	std::string m_Usage;
	std::function<void(CCommand*, const std::vector<std::string>&)> m_Func;
};

class CCommandList
{
public:
	CCommandList();
	~CCommandList();

	std::vector<std::string> GetCommandList();
	void AddCommand(CCommand* cmd);
	void RemoveCommand(CCommand* cmd);
	CCommand* GetCommand(const std::string& name);

private:
	std::vector<CCommand*> m_Commands;
};

static CCommandList* CmdList()
{
	extern CCommandList* g_pCommandList;
	return g_pCommandList;
}
