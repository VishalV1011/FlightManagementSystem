#include "FlightManagement.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <ctime>

// Utility to fetch the current date in YYYY-MM-DD format
std::string FlightManagement::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_t);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d");
    return oss.str();
}

// Validate time format HH:MM:SS
bool FlightManagement::isValidTime(const std::string& time) {
    int hours, minutes, seconds;
    char delimiter;
    std::istringstream ss(time);
    return (ss >> hours >> delimiter >> minutes >> delimiter >> seconds) &&
        (delimiter == ':' && hours >= 0 && hours < 24 && minutes >= 0 && minutes < 60 && seconds >= 0 && seconds < 60);
}

// Add a flight
void FlightManagement::addFlight() {
    std::string flightCode, departureLocation, arrivalLocation, departureDate, departureTime;
    int totalSeats;
    double pricePerSeat;

    try {
        std::cout << "Enter Flight Code (unique): ";
        std::cin >> flightCode;
        if (flightCode.empty()) throw std::invalid_argument("Flight code cannot be empty.");

        std::cout << "Enter Departure Location: ";
        std::cin >> departureLocation;
        if (departureLocation.empty()) throw std::invalid_argument("Departure location cannot be empty.");

        std::cin.ignore();
        std::cin.get();

        std::cout << "Enter Arrival Location: ";
        std::cin >> arrivalLocation;
        if (arrivalLocation.empty()) throw std::invalid_argument("Arrival location cannot be empty.");

        std::cout << "Enter Total Seats: ";
        std::cin >> totalSeats;
        if (totalSeats < 0) throw std::invalid_argument("Total seats cannot be negative.");
        if (totalSeats > 50) throw std::invalid_argument("Max seats are 50.");

        std::cout << "Enter Price per Seat: ";
        std::cin >> pricePerSeat;
        if (pricePerSeat <= 0) throw std::invalid_argument("Price per seat must be greater than 0.");

        // Validate departure date
        bool validDate = false;
        while (!validDate) {
            std::cout << "Enter Departure Date (YYYY-MM-DD): ";
            std::cin >> departureDate;

            // Get current date using localtime_s
            auto now = std::chrono::system_clock::now();
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            std::tm localTime;

            // Use localtime_s for thread-safe conversion
            if (localtime_s(&localTime, &currentTime) != 0) {
                throw std::runtime_error("Failed to get local time.");
            }

            std::ostringstream currentDateStream;
            currentDateStream << std::put_time(&localTime, "%Y-%m-%d");
            std::string currentDate = currentDateStream.str();

            if (departureDate > currentDate) {
                validDate = true;
            }
            else {
                std::cerr << "Error: Departure date must be later than today.\n";
            }
        }

        std::cout << "Enter Departure Time (HH:MM:SS): ";
        std::cin >> departureTime;
        if (!isValidTime(departureTime)) throw std::invalid_argument("Invalid departure time format.");

        // Insert new flight
        std::ostringstream query;
        query << "INSERT INTO flights (flight_code, departure_location, arrival_location, departure_date, departure_time, total_seats, price_per_seat, remaining_seats) "
            << "VALUES ('" << dbConn.escapeString(flightCode) << "', '" << dbConn.escapeString(departureLocation) << "', '"
            << dbConn.escapeString(arrivalLocation) << "', '" << departureDate << "', '"
            << departureDate << " " << departureTime << "', " << totalSeats << ", " << pricePerSeat << ", " << totalSeats << ")";

        if (dbConn.executeQuery(query.str())) {
            std::cout << "Flight added successfully.\n";
        }
        else {
            throw std::runtime_error("Failed to add the flight.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

// Update flight details
void FlightManagement::updateFlight() {
    std::string flightID, flightCode, departureLocation, arrivalLocation, departureDate, departureTime;
    int totalSeats;
    double pricePerSeat;

    viewFlights();

    try {
        std::cout << "Enter Flight ID to update: ";
        std::cin >> flightID;
        if (flightID.empty()) throw std::invalid_argument("Flight ID cannot be empty.");

        // Fetch existing flight details
        std::string existingFlightCode = dbConn.fetchSingleResult("SELECT flight_code FROM flights WHERE flight_id = " + dbConn.escapeString(flightID));
        if (existingFlightCode.empty()) throw std::runtime_error("Flight ID not found.");

        std::cout << "Enter New Flight Code (leave empty to keep existing: " << existingFlightCode << "): ";
        std::cin.ignore();
        std::getline(std::cin, flightCode);
        if (flightCode.empty()) flightCode = existingFlightCode;

        std::cout << "Enter New Departure Location: ";
        std::getline(std::cin, departureLocation);

        std::cout << "Enter New Arrival Location: ";
        std::getline(std::cin, arrivalLocation);

        std::cout << "Enter New Departure Date (YYYY-MM-DD): ";
        std::getline(std::cin, departureDate);

        std::cout << "Enter New Departure Time (HH:MM:SS): ";
        std::getline(std::cin, departureTime);
        if (!departureTime.empty() && !isValidTime(departureTime)) throw std::invalid_argument("Invalid time format.");

        std::cout << "Enter New Total Seats: ";
        std::string seatsInput;
        std::getline(std::cin, seatsInput);
        if (!seatsInput.empty()) totalSeats = std::stoi(seatsInput);

        std::cout << "Enter New Price per Seat: ";
        std::string priceInput;
        std::getline(std::cin, priceInput);
        if (!priceInput.empty()) pricePerSeat = std::stod(priceInput);

        // Update flight details
        std::ostringstream query;
        query << "UPDATE flights SET "
            << "flight_code = '" << dbConn.escapeString(flightCode) << "', "
            << "departure_location = '" << dbConn.escapeString(departureLocation) << "', "
            << "arrival_location = '" << dbConn.escapeString(arrivalLocation) << "', "
            << "departure_time = '" << departureDate << " " << departureTime << "', "
            << "total_seats = " << totalSeats << ", "
            << "price_per_seat = " << pricePerSeat
            << " WHERE flight_id = " << dbConn.escapeString(flightID);

        if (dbConn.executeQuery(query.str())) {
            std::cout << "Flight updated successfully.\n";
        }
        else {
            throw std::runtime_error("Failed to update the flight.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

// Delete a flight
void FlightManagement::deleteFlight() {

    viewFlights();

    std::string flightID;
    try {
        std::cout << "Enter Flight ID to delete: ";
        std::cin >> flightID;

        std::ostringstream query;
        query << "DELETE FROM flights WHERE flight_id = " << dbConn.escapeString(flightID);

        if (dbConn.executeQuery(query.str())) {
            std::cout << "Flight deleted successfully.\n";
        }
        else {
            throw std::runtime_error("Failed to delete the flight.");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

// View all flights
void FlightManagement::viewFlights() {
    try {
        std::string query = "SELECT flight_id, flight_code, departure_location, arrival_location, departure_time, total_seats, price_per_seat FROM flights";

        MYSQL_RES* result = dbConn.fetchResults(query);
        if (result) {
            MYSQL_ROW row;

            // Printing table header with better spacing
            std::cout << std::left << std::setw(10) << "Flight ID"
                << std::setw(15) << "Flight Code"
                << std::setw(20) << "Departure Location"
                << std::setw(20) << "Arrival Location"
                << std::setw(25) << "Departure Date & Time"
                << std::setw(15) << "Total Seats"
                << std::setw(15) << "Price ($)"
                << std::endl;

            // Line separator for aesthetics
            std::cout << std::string(100, '-') << std::endl;

            // Loop through rows and print flight details
            while ((row = mysql_fetch_row(result))) {
                std::cout << std::left << std::setw(10) << row[0]
                    << std::setw(15) << row[1]
                    << std::setw(20) << row[2]
                    << std::setw(20) << row[3]
                    << std::setw(25) << row[4]
                    << std::setw(15) << row[5]
                    << std::setw(15) << row[6]
                    << std::endl;
            }

            mysql_free_result(result);
        }
        else {
            std::cout << "No flights found.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}