#include "Features.h"

Features::Features(DatabaseConnection* connection) : dbConn(connection) {
    if (dbConn == nullptr) {
        std::cerr << "Error: Database connection is not initialized!\n";
        exit(1); // Or handle the error in another way
    }
}
// to hide password input
std::string Features::getPassword() {
    std::string input;
    char ch;

    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!input.empty()) {
                std::cout << "\b \b";
                input.pop_back();
            }
        }
        else {
            input.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return input;
}

// Ensuring Complex Password
bool Features::isPasswordComplex(const std::string& password) {
    if (password.length() < 8) {
        std::cout << "Password must be at least 8 characters long.\n";
        return false;
    }

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    // Check for character types
    for (char ch : password) {
        if (std::isupper(ch)) hasUpper = true;
        else if (std::islower(ch)) hasLower = true;
        else if (std::isdigit(ch)) hasDigit = true;
        else if (std::ispunct(ch)) hasSpecial = true;
    }

    if (!hasUpper) {
        std::cout << "Password must contain at least one uppercase letter.\n";
        return false;
    }
    if (!hasLower) {
        std::cout << "Password must contain at least one lowercase letter.\n";
        return false;
    }
    if (!hasDigit) {
        std::cout << "Password must contain at least one digit.\n";
        return false;
    }
    if (!hasSpecial) {
        std::cout << "Password must contain at least one special character (e.g., !@#$%^&*()).\n";
        return false;
    }

    return true;
}

// Define the trim function (To avoid blank space error)
std::string Features::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, last - first + 1);
}

bool Features::checkUsernameExists(const std::string& username) {
    bool usernameExists;
    // Check if username already exists in the database
    std::string checkUsernameQuery = "SELECT COUNT(*) FROM user WHERE username = '" + username + "'";
    MYSQL_RES* res = dbConn->fetchResults(checkUsernameQuery);
    MYSQL_ROW row;
    if (res && (row = mysql_fetch_row(res))) {
        int count = std::stoi(row[0]);
        if (count > 0) {
            std::cout << "Username already exists! Please choose a different username.\n";
            usernameExists = true;
        }
        else {
            usernameExists = false;
        }
    }
    if (res) mysql_free_result(res);

    return usernameExists;
}

std::string Features::getUsername() {
    std::string username;
    std::cout << "\n\nEnter username: " << std::endl;
    std::cin.ignore();
    std::getline(std::cin, username);
    username = trim(username);
    return username;
}

int Features::selectCustomer() {
    // Ensure that dbConn is not nullptr
    if (!dbConn) {
        std::cerr << "Error: Database connection is not initialized.\n";
        return -1;
    }

    std::vector<std::pair<int, std::string>> customers; // Store user_id and username pairs
    std::ostringstream query;

    // Query to fetch customers
    query << "SELECT user_id, username FROM users WHERE role = 'customer'";

    // Use DatabaseConnection to fetch results
    MYSQL_RES* result = dbConn->fetchResults(query.str());
    if (!result) {
        std::cerr << "Error: Failed to fetch results from database. MySQL error: " << mysql_error(dbConn->getConnection()) << "\n";
        return -1; // Return -1 if fetching results fails
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0] != nullptr && row[1] != nullptr) {
            try {
                int userId = std::stoi(row[0]);     // user_id
                std::string username = row[1];       // username
                customers.emplace_back(userId, username);
            }
            catch (const std::exception& e) {
                std::cerr << "Error processing row: " << e.what() << "\n";
            }
        }
        else {
            std::cerr << "Error: Invalid data in row, skipping...\n";
        }
    }

    mysql_free_result(result);

    if (customers.empty()) {
        std::cout << "No customers found in the database.\n";
        return -1; // Return -1 if no customers exist
    }

    // Display the list of customers
    std::cout << "\n=== Customer List ===\n";
    std::cout << std::setw(10) << "User ID" << std::setw(20) << "Username\n";
    std::cout << "-----------------------------\n";
    for (const auto& customer : customers) {
        std::cout << std::setw(10) << customer.first << std::setw(20) << customer.second << "\n";
    }

    // Prompt the user to select a customer by username
    std::string selectedUsername;
    std::cout << "Enter the username of the customer to select: ";
    std::cin >> selectedUsername;

    // Find and return the user_id of the selected customer
    for (const auto& customer : customers) {
        if (customer.second == selectedUsername) {
            std::cout << "Customer selected: " << customer.second << " (User ID: " << customer.first << ")\n";
            return customer.first;
        }
    }

    std::cout << "Error: Username not found in the customer list.\n";
    return -1;
}


