#ifndef PAYMENTMANAGEMENT_H
#define PAYMENTMANAGEMENT_H

#include "DatabaseConnection.h"
#include <string>

class PaymentManagement {
private:
    DatabaseConnection dbConn;
    int userID;
    std::string role;

public:
    void viewPayments();
    void viewPendingPayments();
    void continuePayment();
    void setUserIDRole(int userID, const std::string& role);
};

#endif