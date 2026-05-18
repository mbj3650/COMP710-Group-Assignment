// COMP710 GP Framework 2025
#include <vector>
#include <string>
#ifndef LOGMANAGER_H
#define LOGMANAGER_H
class LogManager
{
	// Member methods:
public:
	static LogManager& GetInstance();
	static void DestroyInstance();
	void Log(const char* pcMessage);
	void DebugDraw();
protected:
	std::vector<std::string> m_logHistory;
	bool m_bShowLogMessages;
private:
	LogManager();
	~LogManager();
	LogManager(const LogManager& logManager);
	LogManager& operator=(const LogManager& logManager);
	// Member data:
public:
protected:
	static LogManager* sm_pInstance;
private:
};
#endif