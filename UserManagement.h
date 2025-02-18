#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "DatabaseConnection.h"
#include "Features.h"
#include <string>

class UserManagement {
private:
    DatabaseConnection dbConn;
    std::unique_ptr<Features> feat;
    std::string currentUsername, currentUserRole;
    bool isAdminUser;
    std::string username, password;
    int userID;

public:
    UserManagement();
    bool login();
    std::string getCurrentUserRole();
    int getUserID();
    void registerUser();
    void autoCreateRootAccount();
    void viewUsers();
    void updateAccountDetails();
    bool deleteAccount();
    std::string getCurrentUsername() const;
    void logout();
    bool isAdmin() const;
};

#endif
