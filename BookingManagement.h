#ifndef BOOKINGMANAGEMENT_H
#define BOOKINGMANAGEMENT_H

#include "DatabaseConnection.h"
#include "UserManagement.h"
#include "PaymentManagement.h"
#include "FlightManagement.h"
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class BookingManagement {
private:
    DatabaseConnection dbConn;
    UserManagement user;
    PaymentManagement payment;
    FlightManagement flight;
    std::string currentUsername;
    std::string role;
    int userID;

public:
    BookingManagement();
    void setUserIDRole(int userID, const std::string& role);
    void selectFlight();
    void makeBooking();
    void requestRefund();
    void cancelBooking();
    void viewBookings();
    bool isValidSeatFormat(const std::string& seat);
    bool isFlightAvailable(int flightID, int quantity);
    bool isSeatAvailable(int flightID, const std::string& seatNumber);
    void reserveSeat(int flightID, const std::string& seatNumber, int bookingID);
};

#endif