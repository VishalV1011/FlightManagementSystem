#ifndef INTERFACE_H
#define INTERFACE_H

#include "DatabaseConnection.h"
#include "UserManagement.h"
#include "FlightManagement.h"
#include "BookingManagement.h"
#include "PaymentManagement.h"
#include "StatisticsManagement.h"
#include <iostream>
#include <string>

class Interface {
private:
    DatabaseConnection* database;
    UserManagement userManagement;
    FlightManagement flightManagement;
    BookingManagement bookingManagement;
    PaymentManagement paymentManagement;
    StatisticsManagement* statisticsManagement;
    std::string currentUsername;
    std::string currentUserRole; 
    // "admin" or "customer"

    int userID;

public:
    Interface();
    // Main Menu functions
    void showMainMenu();
    void showLoginMenu();
    void showRegisterMenu();

    // Admin Menu functions
    void handleAdminMenu();
    void viewRefundRequests();
    void approveRefund();
    void rejectRefund();

    // Customer Menu functions
    void handleCustomerMenu();

    // Booking and Payment functions
    void handleBookingMenu();
    void handleFlightMenu();
    void handlePaymentMenu();
    void handleUsersMenu();
    void handleStatisticsMenu();
    void generateFlightStatistics();
};

#endif // INTERFACE_H
