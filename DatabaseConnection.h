#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <mysql.h>
#include <iostream>
#include <string>  // for std::string

class DatabaseConnection {
private:
    MYSQL* conn;

public:
    DatabaseConnection();
    ~DatabaseConnection();

    void checkConnection();
    MYSQL* getConnection();
    bool executeQuery(const std::string& query);
    MYSQL_RES* fetchResults(const std::string& query);
    std::string fetchSingleResult(const std::string& query);

    // New function to escape strings
    std::string escapeString(const std::string& str);


};

#endif 
