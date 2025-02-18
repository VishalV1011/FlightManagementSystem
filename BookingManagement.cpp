#include "BookingManagement.h"

BookingManagement::BookingManagement() {}

void BookingManagement::setUserIDRole(int userID, const std::string& role) {
    this->userID = userID;
    this->role = role;

    std::cout << this->userID;
}

void BookingManagement::selectFlight() {

    flight.viewFlights();

    std::string arrivalLocation;
    std::cout << "Enter Arrival Location: ";
    std::cin >> arrivalLocation;

    std::ostringstream query;
    query << "SELECT flight_id, flight_code, departure_location, arrival_location, departure_time, price_per_seat FROM flights "
        << "WHERE arrival_location = '" << arrivalLocation << "'";

    MYSQL_RES* result = dbConn.fetchResults(query.str());
    if (result) {
        MYSQL_ROW row;
        int index = 1;
        std::cout << "Available Flights:\n";
        while ((row = mysql_fetch_row(result))) {
            std::cout << index++ << ". Flight ID: " << row[0] << ", Flight Code: " << row[1]
                << ", Departure: " << row[2] << ", Arrival: " << row[3]
                << ", Time: " << row[4] << ", Price per Seat: " << row[5] << "\n";
        }
        mysql_free_result(result);
    }
    else {
        std::cout << "No flights found for the given arrival location.\n";
    }
}

bool BookingManagement::isValidSeatFormat(const std::string& seat) {
    // Validate seat format: Letter followed by number (e.g., A1, B2)
    if (seat.empty() || seat.length() > 2) {
        std::cerr << "Error: Seat input is invalid.\n";
        return false;
    }

    char letter = seat[0];
    char number = seat[seat.length() - 1];

    if (!isalpha(letter) || !isdigit(number)) {
        std::cerr << "Error: Invalid seat format.\n";
        return false;
    }

    if (!isupper(letter)) {
        std::cerr << "Error: Invalid seat format. Write the letter in uppercase please.\n";
        return false;
    }

    if (letter != 'A' && letter != 'B' && letter != 'C' && letter != 'D') {
        std::cerr << "Error: Seat does not exist.\n";
        return false;
    }

    for (size_t i = 1; i < seat.length() - 1; ++i) {
        if (!isdigit(seat[i])) {
            std::cerr << "Error: Invalid seat format.\n";
            return false;
        }
    }

    return true;
}

bool BookingManagement::isFlightAvailable(int flightID, int quantity) {
    std::string query = "SELECT remaining_seats FROM flights WHERE flight_id = " + std::to_string(flightID) + ";";
    std::string result = dbConn.fetchSingleResult(query);

    if (result.empty()) {
        std::cerr << "Flight not found.\n";
        return false;
    }

    int remainingSeats = std::stoi(result);
    return quantity <= remainingSeats;
}

// Check if the specified seat is available for booking
bool BookingManagement::isSeatAvailable(int flightID, const std::string& seatNumber) {
    std::string escapedSeatNumber = dbConn.escapeString(seatNumber);
    std::string query = "SELECT seat_id FROM booked_seats WHERE flight_id = " + std::to_string(flightID) +
        " AND seat_number = '" + escapedSeatNumber + "';";

    std::string result = dbConn.fetchSingleResult(query);
    return result.empty(); // Seat is available if no result is found
}

// Reserve the specified seat for the flight
void BookingManagement::reserveSeat(int flightID, const std::string& seatNumber, int bookingID) {
    std::string escapedSeatNumber = dbConn.escapeString(seatNumber);

    // Insert the seat into the booked_seats table
    std::string query =
        "INSERT INTO booked_seats (flight_id, seat_number, booking_id, status, seat_status) VALUES (" +
        std::to_string(flightID) + ", '" + escapedSeatNumber + "', " + std::to_string(bookingID) +
        ", 'booked', 'booked');";

    if (dbConn.executeQuery(query)) {
        std::cout << "Seat " << seatNumber << " on flight " << flightID << " has been successfully reserved.\n";

        // Decrease the remaining seats count in the flights table
        std::string updateQuery = "UPDATE flights SET remaining_seats = remaining_seats - 1 WHERE flight_id = " +
            std::to_string(flightID) + ";";
        if (!dbConn.executeQuery(updateQuery)) {
            std::cerr << "Failed to update remaining seats for flight " << flightID << ".\n";
        }
    }
    else {
        std::cerr << "Failed to reserve seat " << seatNumber << " on flight " << flightID << ".\n";
    }
}

// Handle the entire booking process
void BookingManagement::makeBooking() {
    int flightID;
    int quantity;
    double totalPrice;

    selectFlight();

    // Ask for Flight ID
    std::cout << "Enter Flight ID: ";
    std::cin >> flightID;

    // Ask for the quantity of seats to book
    std::cout << "Enter the number of seats to book: ";
    std::cin >> quantity;

    // Check if enough seats are available
    if (!isFlightAvailable(flightID, quantity)) {
        std::cerr << "Not enough seats available. Please try a smaller quantity or another flight.\n";
        return;
    }

    // Calculate total price
    std::string priceQuery = "SELECT price_per_seat FROM flights WHERE flight_id = " + std::to_string(flightID) + ";";
    std::string priceResult = dbConn.fetchSingleResult(priceQuery);

    if (priceResult.empty()) {
        std::cerr << "Flight not found or failed to fetch price. Cannot proceed with booking.\n";
        return;
    }

    double pricePerSeat = std::stod(priceResult);
    totalPrice = pricePerSeat * quantity;

    // Create a booking in the bookings table
    std::string createBookingQuery =
        "INSERT INTO bookings (user_id, flight_id, total_price, payment_status) VALUES (" +
        std::to_string(userID) + ", " + std::to_string(flightID) + ", " + std::to_string(totalPrice) +
        ", 'pending');";

    if (!dbConn.executeQuery(createBookingQuery)) {
        std::cerr << "Failed to create booking. Cannot proceed with seat reservation.\n";
        return;
    }

    // Retrieve the booking ID of the newly created booking
    std::string bookingIDQuery = "SELECT LAST_INSERT_ID();";
    std::string bookingIDResult = dbConn.fetchSingleResult(bookingIDQuery);

    if (bookingIDResult.empty()) {
        std::cerr << "Failed to retrieve booking ID. Cannot proceed with seat reservation.\n";
        return;
    }

    int bookingID = std::stoi(bookingIDResult);

    // Loop to book the specified number of seats
    for (int i = 0; i < quantity; ++i) {
        std::string seatNumber;

        do {
            // Prompt for seat number
            std::cout << "\nNote that:\n- A & D rows are window seats\n- B & C are aisle seats\n\nEnter seat number (e.g., A1): ";
            std::cin >> seatNumber;
        } while (!isValidSeatFormat(seatNumber));

        // Check if the seat is available
        if (!isSeatAvailable(flightID, seatNumber)) {
            std::cerr << "Seat " << seatNumber << " is already booked. Please choose another seat.\n";
            --i; // Retry the same iteration
            continue;
        }
        // Reserve the seat
        reserveSeat(flightID, seatNumber, bookingID);
    }

    std::cout << "Booking process completed successfully. Your total cost is: $" << totalPrice << "\n";

    char choice;

    std::cout << "\nDo you want to continue with your payment? [y/n]";
    std::cin >> choice;
    if (choice == 'y') {
        payment.setUserIDRole(userID, role);
        payment.continuePayment();
    }
    else {
        std::cout << "\n\nYou can continue your payment by selecting the option in the main menu." << std::endl;
    }
}

void BookingManagement::cancelBooking() {
    std::string bookingID;

    // Display all bookings for the user to select
    viewBookings();

    try {
        // Input the booking ID to cancel
        std::cout << "Enter Booking ID to cancel: ";
        std::cin >> bookingID;
        if (bookingID.empty()) throw std::invalid_argument("Booking ID cannot be empty.");

        // Confirm the cancellation request
        char confirm;
        std::cout << "Are you sure you want to cancel the entire booking with ID " << bookingID << "? (y/n): ";
        std::cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "Booking cancellation aborted.\n";
            return;  // Exit if the user did not confirm
        }

        // Begin database operations
        DatabaseConnection dbConn;
        dbConn.checkConnection();

        // Step 1: Delete all rows in booked_seats associated with the booking ID
        std::ostringstream deleteSeatsQuery;
        deleteSeatsQuery << "DELETE FROM booked_seats WHERE booking_id = '"
            << dbConn.escapeString(bookingID) << "'";

        if (!dbConn.executeQuery(deleteSeatsQuery.str())) {
            throw std::runtime_error("Failed to delete booked seats. Please try again.");
        }

        // Step 2: Update the status in the bookings table to 'refunded'
        std::ostringstream updateBookingStatusQuery;
        updateBookingStatusQuery << "UPDATE bookings SET payment_status = 'refunded' WHERE booking_id = '"
            << dbConn.escapeString(bookingID) << "'";

        if (!dbConn.executeQuery(updateBookingStatusQuery.str())) {
            throw std::runtime_error("Failed to update booking status. Please try again.");
        }

        // Success message
        std::cout << "Booking cancellation successful. All related seats have been released, and the status is set to 'refunded'.\n";

    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Input Error: " << e.what() << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

void BookingManagement::viewBookings() {
    // Fetch the user's role from the database
    std::ostringstream roleQuery;
    roleQuery << "SELECT role FROM users WHERE user_id = '" << userID << "'";
    std::string userRole = dbConn.fetchSingleResult(roleQuery.str());

    // UI headers and dividers
    auto printDivider = [](int length) {
        std::cout << std::string(length, '-') << '\n';
        };

    auto printHeader = [printDivider](const std::string& title) {  // Capture printDivider
        printDivider(50);
        std::cout << "| " << title << std::string(48 - title.length(), ' ') << " |\n";
        printDivider(50);
        };

    auto printBookingDetails = [printDivider](const MYSQL_ROW& row) {  // Capture printDivider
        std::cout << "\nBooking ID     : " << row[0] << '\n'
            << "Flight ID      : " << row[1] << '\n'
            << "Seats          : " << (row[4] ? row[4] : "No seats booked") << '\n'
            << "Total Price    : $" << row[2] << '\n'
            << "Payment Status : " << row[3]
            << (std::string(row[3]) == "refunded" ? " (Refunded)" : "") << '\n';
        printDivider(50);
        };

    if (userRole == "customer") {
        // Display customer's bookings
        printHeader("Your Bookings");

        std::ostringstream query;
        query << "SELECT b.booking_id, b.flight_id, b.total_price, b.payment_status, "
            << "GROUP_CONCAT(bs.seat_number ORDER BY bs.seat_number) "
            << "FROM bookings b "
            << "LEFT JOIN booked_seats bs ON b.booking_id = bs.booking_id "
            << "WHERE b.user_id = '" << userID << "' "
            << "GROUP BY b.booking_id";

        MYSQL_RES* result = dbConn.fetchResults(query.str());
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                printBookingDetails(row);
            }
            mysql_free_result(result);
        }
        else {
            std::cout << "No bookings found.\n";
        }
    }
    else if (userRole == "admin") {
        // Admin interface to view customer bookings
        printHeader("Admin: View Customer Bookings");

        std::cout << "Enter the username of the customer: ";
        std::string customerUsername;
        std::cin >> customerUsername;

        // Validate customer's username
        std::ostringstream customerQuery;
        customerQuery << "SELECT user_id FROM users WHERE username = '" << customerUsername << "' AND role = 'customer'";
        std::string customerID = dbConn.fetchSingleResult(customerQuery.str());

        if (customerID.empty()) {
            std::cout << "Invalid customer username or the user is not a customer.\n";
            return;
        }

        // Display bookings for the specified customer
        printHeader("Bookings for Customer: " + customerUsername);

        std::ostringstream query;
        query << "SELECT b.booking_id, b.flight_id, b.total_price, b.payment_status, "
            << "GROUP_CONCAT(bs.seat_number ORDER BY bs.seat_number) "
            << "FROM bookings b "
            << "LEFT JOIN booked_seats bs ON b.booking_id = bs.booking_id "
            << "WHERE b.user_id = " << customerID << " "
            << "GROUP BY b.booking_id";

        MYSQL_RES* result = dbConn.fetchResults(query.str());
        if (result) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                printBookingDetails(row);
            }
            mysql_free_result(result);
        }
        else {
            std::cout << "No bookings found for this customer.\n";
        }
    }
    else {
        printHeader("Error");
        std::cout << "Invalid user role.\n";
    }
}

