#ifndef FEATURES_H
#define FEATURES_H

#include "DatabaseConnection.h"
#include <string>
#include <vector>
#include <iostream>
#include <conio.h>
#include <string>
#include <iomanip>
#include <sstream>

class Features {
private:
    DatabaseConnection* dbConn;
public:
    // Constructor
    Features();
    Features(DatabaseConnection* connection);

    // Function to mask text input
    std::string getPassword();
    bool isPasswordComplex(const std::string& password);
    std::string trim(const std::string& str);
    bool checkUsernameExists(const std::string& username);
    std::string getUsername();
    int selectCustomer();
};

#endif // FEATURES_H
