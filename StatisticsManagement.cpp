#include "StatisticsManagement.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

StatisticsManagement::StatisticsManagement(DatabaseConnection* dbConn) : db(dbConn) {}

void StatisticsManagement::generateFlightStatistics() {
    try {
        // Flight Statistics
        std::string query =
            "SELECT f.flight_code, COUNT(b.booking_id) AS bookings "
            "FROM bookings b "
            "JOIN flights f ON b.flight_id = f.flight_id "
            "GROUP BY f.flight_code";
        MYSQL_RES* results = db->fetchResults(query);

        if (!results) {
            throw std::runtime_error("Failed to fetch results for flight statistics.");
        }

        MYSQL_ROW row;
        std::cout << "\n===============================================\n";
        std::cout << "              FLIGHT STATISTICS REPORT\n";
        std::cout << "===============================================\n";
        std::cout << std::left << std::setw(20) << "Flight Code" << std::setw(15) << "Bookings\n";
        std::cout << "-----------------------------------------------\n";
        while ((row = mysql_fetch_row(results))) {
            std::cout << std::left << std::setw(20) << (row[0] ? row[0] : "N/A")
                << std::setw(15) << (row[1] ? row[1] : "0") << std::endl;
        }
        mysql_free_result(results);

        // Revenue by Flight
        query =
            "SELECT f.flight_code, SUM(b.total_price) AS total_revenue "
            "FROM bookings b "
            "JOIN flights f ON b.flight_id = f.flight_id "
            "GROUP BY f.flight_code";
        results = db->fetchResults(query);

        if (!results) {
            throw std::runtime_error("Failed to fetch results for revenue by flight.");
        }

        std::cout << "\n===============================================\n";
        std::cout << "               REVENUE BY FLIGHT\n";
        std::cout << "===============================================\n";
        std::cout << std::left << std::setw(20) << "Flight Code" << std::setw(20) << "Total Revenue\n";
        std::cout << "-----------------------------------------------\n";
        while ((row = mysql_fetch_row(results))) {
            std::cout << std::left << std::setw(20) << (row[0] ? row[0] : "N/A")
                << std::setw(20) << (row[1] ? row[1] : "0.00") << std::endl;
        }
        mysql_free_result(results);

        // Average Revenue per User
        query =
            "SELECT AVG(total_spent) AS avg_revenue_per_user "
            "FROM ( "
            " SELECT u.user_id, SUM(b.total_price) AS total_spent "
            " FROM users u "
            " JOIN bookings b ON u.user_id = b.user_id "
            " GROUP BY u.user_id "
            ") AS user_revenue";
        std::string result = db->fetchSingleResult(query);

        if (result.empty()) {
            throw std::runtime_error("Failed to calculate average revenue per user.");
        }

        double averageRevenue = std::stod(result);
        std::cout << "\n===============================================\n";
        std::cout << "       AVERAGE REVENUE PER USER\n";
        std::cout << "===============================================\n";
        std::cout << std::fixed << std::setprecision(2) << "Average Revenue per User: $"
            << averageRevenue << std::endl;

        // Top Destinations
        int topN = 5; // Example of topN value, you can modify based on user input
        if (topN <= 0) {
            std::cerr << "\033[1;31mError: \033[0mNumber of top destinations must be greater than 0.\n";
            return;
        }

        query =
            "SELECT f.arrival_location, COUNT(b.booking_id) AS total_bookings "
            "FROM bookings b "
            "JOIN flights f ON b.flight_id = f.flight_id "
            "GROUP BY f.arrival_location "
            "ORDER BY total_bookings DESC "
            "LIMIT " + std::to_string(topN);
        results = db->fetchResults(query);

        if (!results) {
            throw std::runtime_error("Failed to fetch results for top destinations.");
        }

        std::cout << "\n===============================================\n";
        std::cout << "             TOP DESTINATIONS\n";
        std::cout << "===============================================\n";
        std::cout << std::left << std::setw(25) << "Destination" << std::setw(15) << "Bookings\n";
        std::cout << "-----------------------------------------------\n";
        while ((row = mysql_fetch_row(results))) {
            std::cout << std::left << std::setw(25) << (row[0] ? row[0] : "N/A")
                << std::setw(15) << (row[1] ? row[1] : "0") << std::endl;
        }
        mysql_free_result(results);

        // Monthly Booking Trends
        query =
            "SELECT MONTH(b.booking_date) AS booking_month, COUNT(b.booking_id) AS total_bookings "
            "FROM bookings b "
            "GROUP BY booking_month "
            "ORDER BY booking_month";
        results = db->fetchResults(query);

        if (!results) {
            throw std::runtime_error("Failed to fetch results for booking trends.");
        }

        std::cout << "\n===============================================\n";
        std::cout << "          MONTHLY BOOKING TRENDS\n";
        std::cout << "===============================================\n";
        std::cout << std::left << std::setw(20) << "Month" << std::setw(20) << "Total Bookings\n";
        std::cout << "-----------------------------------------------\n";
        while ((row = mysql_fetch_row(results))) {
            std::cout << std::left << std::setw(20) << (row[0] ? row[0] : "N/A")
                << std::setw(20) << (row[1] ? row[1] : "0") << std::endl;
        }
        mysql_free_result(results);
    }
    catch (const std::exception& e) {
        std::cerr << "\033[1;31mError: \033[0m" << e.what() << std::endl;
    }
}
