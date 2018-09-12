#pragma once
#ifndef DbLLib_H
#define DbLLib_H
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSql>
#include <vector>
#include <QString>
#include <memory>

class DB {

	struct ClientInfo {
		int id;
		QString name;
	};

public:

	static DB& GetInstance();
	void OpenDataBase();

	int GetLastInsertedId() const;
	bool CheckIfClientWithSameNameExist(std::string name) const;
	int GetClientIDByName(std::string name);

	bool DeleteClient(std::string name);
	void ShowAllClients();
	std::vector<std::string> SelectNameOfAllClients();
	void ClearTableClient();

	//insert information
	void InsertClientHistory(int clientID,const std::string name, int &insertedId) const;
	bool InserClientInfo(const std::string name, int & insertedId) const;

private:
	DB();
	~DB();
	DB(const DB&) = delete;
	void operator = (const DB&) = delete;

private:
	std::unique_ptr<QSqlDatabase> m_db;
	std::unique_ptr<QSqlQuery> m_query;
	QString m_path;
};
#endif // !DbLLib_H