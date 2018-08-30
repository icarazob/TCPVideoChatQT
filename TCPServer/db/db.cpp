#include "db.h"
#include <iostream>



DB::DB():
	db(std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"))),
	query(nullptr)
{
	OpenDataBase();
	ClearTableClient();
}

DB::~DB()
{
	db->close();
}

DB & DB::GetInstance()
{
	static DB db;
	return db;
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

int DB::GetLastInsertedId() const
{
	int value = 0;

	query->prepare("SELECT MAX(id) from client;");

	query->exec();

	if (query->next())
	{
		value = query->value(0).toInt();
	}
	else
	{
		value = 0;
	}

	return value + 1;
}

bool DB::CheckIfClientWithSameNameExist(std::string name) const
{
	query->prepare("SELECT COUNT(1) FROM client WHERE name=:name;");

	query->bindValue(":name", QString::fromStdString(name));

	bool result = query->exec();

	if (result)
	{
		if (query->next())
		{
			int count = query->value(0).toInt();

			if (count >= 1)
			{
				return true;
			}
		}
	}

	return false;
}

bool DB::InserClientInfo(const std::string name, int & insertedId) const
{
	bool checkIfExist = CheckIfClientWithSameNameExist(name);
	if (checkIfExist)
	{
		return false;
	}

	int id = insertedId;

	if (id < 1)
	{
		id = GetLastInsertedId();
	}

	query->prepare("INSERT INTO client (id,name) VALUES(:id,:name);");
	
	query->bindValue(":id", id);
	query->bindValue(":name", QString::fromStdString(name));

	bool result = query->exec();

	if (result)
	{
		insertedId = id;
	}

	return result;

}

bool DB::DeleteClient(std::string name)
{
	query->prepare("DELETE FROM client where name=:name;");

	query->bindValue(":name", QString::fromStdString(name));

	bool result = query->exec();

	return result;
}

void DB::ShowAllClients()
{
	query->prepare("SELECT * FROM client;");

	bool res = query->exec();

	if (res)
	{
		std::vector<ClientInfo> vectorClients;
		while (query->next())
		{
			ClientInfo client;

			client.id = query->value(0).toInt();
			client.name = query->value(1).toString();

			vectorClients.push_back(client);
		}

		for (auto &client : vectorClients)
		{
			std::cout << "Id = " << client.id << " Name = " << client.name.toStdString() << std::endl;
		}

	}
}

std::vector<std::string> DB::SelectNameOfAllClients()
{
	std::vector<std::string> vectorNames;
	vectorNames.resize(0);

	query->prepare("SELECT name FROM client;");

	bool result = query->exec();

	if (result)
	{
		
		while (query->next())
		{
			QString name = query->value(0).toString();

			vectorNames.push_back(name.toStdString());
		}
	}

	return vectorNames;
}

void DB::ClearTableClient()
{
	query->prepare("DELETE FROM client;");

	query->exec();
}

// void DB::SelectTable()
// {
// 	query->prepare("SELECT * FROM client;");
// 	bool res = query->exec();
// 
// 	std::vector<ClientInfo> clients;
// 
// 	if (res)
// 	{
// 		while (query->next())
// 		{
// 			ClientInfo client;
// 			client.id = query->value(0).toInt();
// 			client.name = query->value(1).toString();
// 
// 			clients.push_back(client);
// 
// 		}
// 
// 
// 		std::cout << clients[0].id << " Name: " << clients[0].name.toStdString() << std::endl;
// 	}
// }
