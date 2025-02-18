#include "DatabaseConnection.h"
#include <mysql.h>
#include <iostream>
#include <string>

DatabaseConnection::DatabaseConnection() {
    conn = mysql_init(0);
    if (!conn) {
        std::cerr << "Error: MySQL initialization failed!" << std::endl;
        exit(1);
    }

    conn = mysql_real_connect(conn, "localhost", "root", "", "airlines1", 3306, NULL, 0);
}

DatabaseConnection::~DatabaseConnection() {
    if (conn) {
        mysql_close(conn);
        std::cout << "Database Connection Closed!" << std::endl;
    }
}

void DatabaseConnection::checkConnection() {
    if (conn) {
        std::cout << "Database Connected Successfully!" << std::endl;
    }
    else {
        std::cerr << "Database Connection Failed: " << mysql_error(conn) << std::endl;
        exit(1);
    }
}

MYSQL* DatabaseConnection::getConnection() {
    return conn;
}

bool DatabaseConnection::executeQuery(const std::string& query) {
    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Query Execution Failed: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

MYSQL_RES* DatabaseConnection::fetchResults(const std::string& query) {
    if (!executeQuery(query)) return nullptr;
    return mysql_store_result(conn);
}

std::string DatabaseConnection::fetchSingleResult(const std::string& query) {
    MYSQL_RES* result = fetchResults(query);
    if (!result) {
        return ""; // Return an empty string if the query fails
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    std::string singleResult = (row && row[0]) ? row[0] : "";
    mysql_free_result(result);
    return singleResult;
}

// Implement the escapeString function
std::string DatabaseConnection::escapeString(const std::string& str) {
    char* escapedStr = new char[str.length() * 2 + 1]; // Allocate space for escaped string
    mysql_real_escape_string(conn, escapedStr, str.c_str(), str.length());
    std::string result(escapedStr); // Convert the char array to std::string
    delete[] escapedStr; // Free the allocated memory
    return result;
}
