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
	DB();
	~DB()
	{
		db->close();
	}
	void OpenDataBase();
	void SelectTable();
private:
	std::unique_ptr<QSqlDatabase> db;
	std::unique_ptr<QSqlQuery> query;
};
#endif // !DB_H