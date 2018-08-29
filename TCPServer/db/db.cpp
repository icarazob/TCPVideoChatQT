#include "db.h"
#include <iostream>



DB::DB():
	db(std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"))),
	query(nullptr)
{

}

void DB::OpenDataBase()
{
	QString dbPath = "E:\\Database\\chat.db";

	db->setDatabaseName(dbPath);

	bool opened = db->open();

	if (!opened)
	{
		std::cout << "Not open" << std::endl;
	}


	query = std::make_unique<QSqlQuery>(*db);

}

void DB::SelectTable()
{
	query->prepare("SELECT * FROM client;");
	bool res = query->exec();

	std::vector<ClientInfo> clients;

	if (res)
	{
		while (query->next())
		{
			ClientInfo client;
			client.id = query->value(0).toInt();
			client.name = query->value(1).toString();

			clients.push_back(client);

		}


		std::cout << clients[0].id << " Name: " << clients[0].name.toStdString() << std::endl;
	}
}
