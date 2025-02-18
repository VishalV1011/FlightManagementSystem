#include "Interface.h"
#include <iostream>
#include <string>

Interface::Interface() {
    database = new DatabaseConnection();
    database->checkConnection();
    statisticsManagement = new StatisticsManagement(database);

    userManagement.autoCreateRootAccount();
}

void Interface::showMainMenu() {
    std::cout << "\n==============================\n";
    std::cout << "   WELCOME TO SHAA AIRLINES\n";
    std::cout << "==============================\n";
    std::cout << "1. Login\n";
    std::cout << "2. Register\n";
    std::cout << "3. Exit\n";
    std::cout << "==============================\n";
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1:
        showLoginMenu();
        break;
    case 2:
        showRegisterMenu();
        break;
    case 3:
        std::cout << "Exiting the system...\n";
        exit(0);
    default:
        std::cout << "Invalid choice, please try again.\n";
        showMainMenu();
    }
}

void Interface::showLoginMenu() {
    std::cout << "\n==============================\n";
    std::cout << "         LOGIN MENU\n";
    std::cout << "==============================\n";

    bool success = userManagement.login();

    if (success) {
        this->userID = userManagement.getUserID();
        this->currentUserRole = userManagement.getCurrentUserRole();
        paymentManagement.setUserIDRole(this->userID, this->currentUserRole);
        bookingManagement.setUserIDRole(this->userID, this->currentUserRole);

        if (currentUserRole == "admin") {
            handleAdminMenu();
        }
        else if (currentUserRole == "customer") {
            handleCustomerMenu();
        }
        else {
            std::cout << "Invalid role!\n";
            showMainMenu();
        }
    }
    else {
        std::cout << "\nLogin Failed! Please try again.\n";
        showMainMenu();
    }
}

void Interface::showRegisterMenu() {
    std::cout << "\n==============================\n";
    std::cout << "        REGISTER MENU\n";
    std::cout << "==============================\n";

    userManagement.registerUser();
    std::cout << "Registration successful!\n";
    showMainMenu();
}

void Interface::handleAdminMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "           ADMIN MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. Manage Users\n";
        std::cout << "2. Manage Flights\n";
        std::cout << "3. Manage Bookings\n";
        std::cout << "4. Manage Payments\n";
        std::cout << "5. Manage Statistics\n";
        std::cout << "6. Logout\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: handleUsersMenu(); break;
        case 2: handleFlightMenu(); break;
        case 3: handleBookingMenu(); break;
        case 4: handlePaymentMenu(); break;
        case 5: handleStatisticsMenu(); break;
        case 6:
            std::cout << "Logging out...\n";
            userManagement.logout();
            currentUsername.clear();
            currentUserRole.clear();
            showMainMenu();
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 6);
}

void Interface::handleCustomerMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "         CUSTOMER MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. View All Flights\n";
        std::cout << "2. Make a Booking\n";
        std::cout << "3. View Bookings\n";
        std::cout << "4. View Payment Details\n";
        std::cout << "5. View Pending Payment Details\n";
        std::cout << "6. Continue Payment\n";
        std::cout << "7. Cancel Booking\n";
        std::cout << "8. Delete Account\n";
        std::cout << "9. Logout\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: flightManagement.viewFlights(); break;
        case 2: bookingManagement.makeBooking(); break;
        case 3: bookingManagement.viewBookings(); break;
        case 4: paymentManagement.viewPayments(); break;
        case 5: paymentManagement.viewPendingPayments(); break;
        case 6: paymentManagement.continuePayment(); break;
        case 7: bookingManagement.cancelBooking(); break;
        case 8: {
            if (userManagement.deleteAccount()) {
                std::cout << "\nDeleting account and logging out...\n";
                showMainMenu();
            }
            break;
        }
        case 9:
            std::cout << "Logging out...\n";
            currentUsername.clear();
            currentUserRole.clear();
            showMainMenu();
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (true);
}

void Interface::handleBookingMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "         BOOKING MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. View Flight Details\n";
        std::cout << "2. View Bookings\n";
        std::cout << "3. Exit to Main Menu\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: bookingManagement.selectFlight(); break;
        case 2: bookingManagement.viewBookings(); break;
        case 3:
            std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 3);
}

void Interface::handleFlightMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "      FLIGHT MANAGEMENT MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. Add Flight\n";
        std::cout << "2. View Flight Details\n";
        std::cout << "3. Update Flight\n";
        std::cout << "4. Delete Flight\n";
        std::cout << "5. Return to Main Menu\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: flightManagement.addFlight(); break;
        case 2: flightManagement.viewFlights(); break;
        case 3: flightManagement.updateFlight(); break;
        case 4: flightManagement.deleteFlight(); break;
        case 5:
            std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 5);
}

void Interface::handlePaymentMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "          PAYMENT MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. View Payment Status\n";
        std::cout << "2. View Pending Payments\n";
        std::cout << "3. Return to Main Menu\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: paymentManagement.viewPayments(); break;
        case 2: paymentManagement.viewPendingPayments(); break;
        case 3:
            std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 3);
}

void Interface::handleUsersMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "        USER MANAGEMENT\n";
        std::cout << "==============================\n";
        std::cout << "1. Update User Details\n";
        std::cout << "2. View User Details\n";
        std::cout << "3. Delete Account\n";
        std::cout << "4. Return to Main Menu\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: userManagement.updateAccountDetails(); break;
        case 2: userManagement.viewUsers(); break;
        case 3: userManagement.deleteAccount(); break;
        case 4:
            std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 4);
}

void Interface::handleStatisticsMenu() {
    int choice;
    do {
        std::cout << "\n==============================\n";
        std::cout << "        STATISTICS MENU\n";
        std::cout << "==============================\n";
        std::cout << "1. General Flight Statistics\n";
        std::cout << "2. Return to Main Menu\n";
        std::cout << "==============================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: statisticsManagement->generateFlightStatistics(); break;
        case 2:
            std::cout << "Returning to Main Menu...\n";
            break;
        default: std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 2);
}