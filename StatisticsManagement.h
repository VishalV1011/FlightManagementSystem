#ifndef STATISTICSMANAGEMENT_H
#define STATISTICSMANAGEMENT_H

#include "DatabaseConnection.h"

class StatisticsManagement {
public:
    StatisticsManagement(DatabaseConnection* dbConn);
    void generateFlightStatistics();                // Group bookings by flights

private:
    DatabaseConnection* db;
};

#endif
