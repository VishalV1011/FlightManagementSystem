#include "UserManagement.h"
#include <iostream>
#include <sstream>

UserManagement::UserManagement() : isAdminUser(false), feat(std::make_unique<Features>(&dbConn)) {}

std::string UserManagement::getCurrentUserRole() {
    return this->currentUserRole;
}

int UserManagement::getUserID() {
    return this->userID;
}

bool UserManagement::login() {
    // User input for login
    std::cout << "Enter username: ";
    std::cin >> username;

    std::cout << "\nEnter password: ";
    password = feat->getPassword();  // Get password from Features class
    feat->trim(password);

    // Prepare the query to check for valid username, password, and role
    std::ostringstream query;
    query << "SELECT username, role, user_id FROM users WHERE username = '"
        << username << "' AND password = SHA2('" << password << "', 256)";

    // Fetch results from the database
    MYSQL_RES* result = dbConn.fetchResults(query.str());

    if (result && mysql_num_rows(result) > 0) {
        MYSQL_ROW row = mysql_fetch_row(result);
        this->currentUsername = row[0];  // Save the username
        this->currentUserRole = row[1];  // Determine the role (admin/customer)
        this->userID = std::stoi(row[2]);

        mysql_free_result(result);

        std::cout << "Login successful!" << std::endl;

        // Set the boolean flag based on role
        if (currentUserRole == "admin") {
            isAdminUser = true;
        }
        else {
            isAdminUser = false;
        }

        return true;
    }
    else {
        std::cout << "Invalid username or password." << std::endl;
        return false;
    }
}

void UserManagement::registerUser() {
    std::string username, password, fullName, email, phone;
    std::cout << "Enter Username: ";
    std::cin >> username;
    std::cout << "Enter Password: ";
    do {
        std::cout << "\nPassword must be:\n - At least 8 characters\n - At least one uppercase letter\n - At least one lowercase letter\n - At least one digit\n - At least one special character (e.g., !@#$%^&*())\n\nEnter Password: ";
        password = feat->getPassword();
        password = feat->trim(password);
    } while (!feat->isPasswordComplex(password));
    std::cout << "Enter Full Name: ";
    std::cin.ignore();
    std::getline(std::cin, fullName);
    std::cout << "Enter Email: ";
    std::cin >> email;
    std::cout << "Enter Phone Number: ";
    std::cin >> phone;

    std::ostringstream query;
    query << "INSERT INTO users (username, password, full_name, email, phone_number) VALUES ('"
        << username << "', SHA2('" << password << "', 256), '" << fullName << "', '"
        << email << "', '" << phone << "')";

    if (dbConn.executeQuery(query.str())) {
        std::cout << "Registration successful!\n";
    }
    else {
        std::cout << "Error: Could not register user.\n";
    }
}

void UserManagement::autoCreateRootAccount() {
    std::ostringstream query;
    query << "INSERT IGNORE INTO users (username, password, full_name, email, role) VALUES "
        << "('admin', SHA2('admin', 256), 'System Admin', 'admin@airline.com', 'admin')";
    dbConn.executeQuery(query.str());
}

void UserManagement::viewUsers() {
    std::ostringstream query;

    // Admin can see all users, including phone numbers and roles
    if (isAdminUser) {
        query << "SELECT user_id, username, full_name, email, phone_number, role FROM users";
    }
    // Non-admin can only view their own details
    else {
        query << "SELECT user_id, username, full_name, email, phone_number FROM users WHERE username = '"
            << currentUsername << "'";
    }

    MYSQL_RES* result = dbConn.fetchResults(query.str());
    if (result) {
        MYSQL_ROW row;

        // Set up column headers
        std::cout << std::left << std::setw(5) << "ID"
            << std::setw(15) << "Username"
            << std::setw(25) << "Full Name"
            << std::setw(30) << "Email"
            << std::setw(15) << "Phone Number";

        if (isAdminUser) {
            std::cout << std::setw(10) << "Role";
        }

        std::cout << "\n" << std::string(100, '-') << "\n";  // A separator line for visual appeal

        // Display user details in a neat and aligned manner
        while ((row = mysql_fetch_row(result))) {
            std::cout << std::left << std::setw(5) << row[0]
                << std::setw(15) << row[1]
                << std::setw(25) << row[2]
                << std::setw(30) << row[3]
                << std::setw(15) << (row[4] ? row[4] : "N/A");  // If phone_number is NULL, display "N/A"

            if (isAdminUser) {
                std::cout << std::setw(10) << (row[5] ? row[5] : "N/A");  // Display role if admin
            }

            std::cout << "\n";
        }

        mysql_free_result(result);
    }
    else {
        std::cout << "No users found.\n";
    }
}

void UserManagement::updateAccountDetails() {
    // Select the customer using Features::selectCustomer
    int userID = feat->selectCustomer();

    if (userID == -1) {
        std::cout << "No valid customer selected. Aborting update.\n";
        return;
    }

    // Fetch existing details for the selected user
    std::string currentPasswordHash, currentEmail, currentPhone;
    std::ostringstream fetchQuery;
    fetchQuery << "SELECT password, email, phone_number FROM users WHERE user_id = '" << userID << "'";

    MYSQL_RES* result = dbConn.fetchResults(fetchQuery.str());
    if (result) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (row) {
            currentPasswordHash = row[0] ? row[0] : "";  // Handle possible NULL for password
            currentEmail = row[1] ? row[1] : "";          // Handle possible NULL for email
            currentPhone = row[2] ? row[2] : "";          // Handle possible NULL for phone_number
        }
        mysql_free_result(result);
    }
    else {
        std::cout << "Error fetching current account details.\n";
        return;
    }

    // Prompt user for which attribute to update
    int choice = 0;
    std::cout << "Which attribute would you like to update?\n";
    std::cout << "1. Password\n";
    std::cout << "2. Email\n";
    std::cout << "3. Phone Number\n";
    std::cout << "Enter your choice (1-3): ";
    std::cin >> choice;
    std::cin.ignore();  // Clear the newline character left by std::cin

    // Check for invalid input
    if (choice < 1 || choice > 3) {
        std::cout << "Invalid choice. Aborting update.\n";
        return;
    }

    // Prompt user for new values based on their selection
    std::string newPassword = currentPasswordHash;
    std::string newEmail = currentEmail;
    std::string newPhone = currentPhone;

    switch (choice) {
    case 1:  // Update Password
        std::cout << "Enter New Password (leave empty to keep current): ";
        std::getline(std::cin, newPassword); // Read input into newPassword
        if (newPassword.empty()) {
            newPassword = currentPasswordHash;  // Keep the existing hashed password
        }
        else {
            newPassword = "SHA2('" + dbConn.escapeString(newPassword) + "', 256)";  // Hash and escape password
        }
        break;

    case 2:  // Update Email
        std::cout << "Enter New Email (leave empty to keep current): ";
        std::getline(std::cin, newEmail);
        if (newEmail.empty()) {
            newEmail = currentEmail;  // Keep existing email if empty
        }
        else {
            newEmail = dbConn.escapeString(newEmail);  // Escape email to prevent SQL injection
        }
        break;

    case 3:  // Update Phone Number
        std::cout << "Enter New Phone Number (leave empty to keep current): ";
        std::getline(std::cin, newPhone);
        if (newPhone.empty()) {
            newPhone = currentPhone;  // Keep existing phone number if empty
        }
        else {
            newPhone = dbConn.escapeString(newPhone);  // Escape phone number to prevent SQL injection
        }
        break;
    }

    // Construct the update query
    std::ostringstream updateQuery;
    updateQuery << "UPDATE users SET ";

    // Conditionally update only the selected attribute
    if (choice == 1) {
        updateQuery << "password = " << newPassword;
    }
    else if (choice == 2) {
        updateQuery << "email = '" << newEmail << "'";
    }
    else if (choice == 3) {
        updateQuery << "phone_number = '" << newPhone << "'";
    }

    // Apply WHERE clause to identify the correct user
    updateQuery << " WHERE user_id = '" << userID << "'";

    // Execute the update query
    if (dbConn.executeQuery(updateQuery.str())) {
        std::cout << "Account details updated successfully.\n";
    }
    else {
        std::cout << "Error: Could not update account details.\n";
    }
}

bool UserManagement::deleteAccount() {
    // Select the user to delete by their userID
    std::string usernameToDelete;
    if (currentUserRole == "admin") {

        int userIDToDelete = feat->selectCustomer();

        // Fetch the username based on the userID
        std::ostringstream fetchQuery;
        fetchQuery << "SELECT username FROM users WHERE user_id = '" << userIDToDelete << "'";

        MYSQL_RES* result = dbConn.fetchResults(fetchQuery.str());
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) {
                usernameToDelete = row[0] ? row[0] : "";  // Fetch the username for the given userID
            }
            mysql_free_result(result);
        }
        else {
            std::cout << "Error: Could not fetch user details.\n";
            return false;
        }

        // If no user found for the given userID
        if (usernameToDelete.empty()) {
            std::cout << "No user found with the given user ID.\n";
            return false;
        }

    }
    else if (currentUserRole == "customer") {
        usernameToDelete = currentUsername;
    }

    // Confirm deletion
    char confirm;
    std::cout << "Are you sure you want to delete the account with username '" << usernameToDelete << "'? (y/n): ";
    std::cin >> confirm;

    // If the user doesn't confirm, abort the deletion
    if (confirm != 'y' && confirm != 'Y') {
        return false;
    }

    // Delete the user based on the fetched username
    std::ostringstream deleteQuery;
    deleteQuery << "DELETE FROM users WHERE username = '" << usernameToDelete << "'";

    // Execute the delete query
    if (dbConn.executeQuery(deleteQuery.str())) {
        std::cout << "Account with username '" << usernameToDelete << "' deleted successfully.\n";
        return true;
    }
    else {
        std::cout << "Error: Could not delete the account.\n";
        return false;
    }
}

void UserManagement::logout() {
    username = "";
    password = "";

}

std::string UserManagement::getCurrentUsername() const {
    std::cout << currentUsername;
    return this->currentUsername;
}

bool UserManagement::isAdmin() const {
    return isAdminUser;
}
