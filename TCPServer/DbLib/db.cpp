#include "db.h"
#include <iostream>
#include <QCoreApplication>


DB::DB():
	m_db(std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"))),
	m_query(nullptr)
{
	m_path = QCoreApplication::applicationDirPath();
	OpenDataBase();
	ClearTableClient();
}

DB::~DB()
{
	m_db->close();
}

DB & DB::GetInstance()
{
	static DB db;
	return db;
}

void DB::OpenDataBase()
{
	QString dbPath = m_path + "/database/chat.db";

	m_db->setDatabaseName(dbPath);

	bool opened = m_db->open();

	if (!opened)
	{
		std::cout << "Not open" << std::endl;
		exit(1);
	}


	m_query = std::make_unique<QSqlQuery>(*m_db);
}

int DB::GetLastInsertedId() const
{
	int value = 0;

	m_query->prepare("SELECT MAX(id) from client;");

	m_query->exec();

	if (m_query->next())
	{
		value = m_query->value(0).toInt();
	}
	else
	{
		value = 0;
	}

	return value + 1;
}

bool DB::CheckIfClientWithSameNameExist(std::string name) const
{
	m_query->prepare("SELECT COUNT(1) FROM client WHERE name=:name;");

	m_query->bindValue(":name", QString::fromStdString(name));

	bool result = m_query->exec();

	if (result)
	{
		if (m_query->next())
		{
			int count = m_query->value(0).toInt();

			if (count >= 1)
			{
				return true;
			}
		}
	}

	return false;
}

int DB::GetClientIDByName(std::string name)
{
	m_query->prepare("SELETCT id FROM client WHERE name:=name;");

	m_query->bindValue(":name", QString::fromStdString(name));

	bool result = m_query->exec();

	if (result)
	{
		if (m_query->next())
		{
			int id = m_query->value(0).toInt();
			return id;
		}
	}
	else
	{
		return -1;
	}
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

	m_query->prepare("INSERT INTO client (id,name) VALUES(:id,:name);");
	
	m_query->bindValue(":id", id);
	m_query->bindValue(":name", QString::fromStdString(name));

	bool result = m_query->exec();

	if (result)
	{
		insertedId = id;
	}

	return result;

}

bool DB::DeleteClient(std::string name)
{
	m_query->prepare("DELETE FROM client where name=:name;");

	m_query->bindValue(":name", QString::fromStdString(name));

	bool result = m_query->exec();

	return result;
}

void DB::ShowAllClients()
{
	m_query->prepare("SELECT * FROM client;");

	bool res = m_query->exec();

	if (res)
	{
		std::vector<ClientInfo> vectorClients;
		while (m_query->next())
		{
			ClientInfo client;

			client.id = m_query->value(0).toInt();
			client.name = m_query->value(1).toString();

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

	m_query->prepare("SELECT name FROM client;");

	bool result = m_query->exec();

	if (result)
	{
		
		while (m_query->next())
		{
			QString name = m_query->value(0).toString();

			vectorNames.push_back(name.toStdString());
		}
	}

	return vectorNames;
}

void DB::ClearTableClient()
{
	m_query->prepare("DELETE FROM client;");

	m_query->exec();
}

void DB::InsertClientHistory(int clientID, const std::string name, int &insertedId) const
{

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
