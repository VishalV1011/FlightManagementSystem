#ifndef FLIGHTMANAGEMENT_H
#define FLIGHTMANAGEMENT_H

#include "DatabaseConnection.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>

class FlightManagement {
private:
    DatabaseConnection dbConn;

public:
    std::string getCurrentDate();
    bool isValidTime(const std::string& time);
    void addFlight();
    void updateFlight();
    void deleteFlight();
    void viewFlights();
};

#endif
