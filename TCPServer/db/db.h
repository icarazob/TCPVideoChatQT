#pragma once
#ifndef DB_H
#define DB_H
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
	bool InserClientInfo(const std::string name, int &insertedId) const;
	bool DeleteClient(std::string name);
	void ShowAllClients();
	std::vector<std::string> SelectNameOfAllClients();
	void ClearTableClient();
private:
	DB();
	~DB();
	DB(const DB&) = delete;
	void operator = (const DB&) = delete;

private:
	std::unique_ptr<QSqlDatabase> db;
	std::unique_ptr<QSqlQuery> query;
};
#endif // !DB_H