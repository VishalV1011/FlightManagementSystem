#include "PaymentManagement.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

void PaymentManagement::setUserIDRole(int userID, const std::string& role) {
    this->userID = userID;
    this->role = role;
}

void PaymentManagement::viewPayments() {
    std::ostringstream query;
    query << "SELECT booking_id, total_price, payment_status FROM bookings";

    MYSQL_RES* result = dbConn.fetchResults(query.str());
    if (result) {
        MYSQL_ROW row;
        std::cout << "============================================\n";
        std::cout << "              Paid Bookings\n";
        std::cout << "============================================\n";
        std::cout << std::setw(15) << "Booking ID"
            << std::setw(20) << "Total Price"
            << std::setw(15) << "Status\n";
        std::cout << "--------------------------------------------\n";
        while ((row = mysql_fetch_row(result))) {
            std::cout << std::setw(15) << row[0]
                << std::setw(20) << row[1]
                << std::setw(15) << row[2] << "\n";
        }
        mysql_free_result(result);
        std::cout << "============================================\n";
    }
    else {
        std::cout << "\nNo paid bookings found.\n";
    }
}

void PaymentManagement::viewPendingPayments() {
    std::ostringstream query;
    query << "SELECT booking_id, total_price FROM bookings WHERE payment_status = 'Pending'";

    MYSQL_RES* result = dbConn.fetchResults(query.str());
    if (result) {
        MYSQL_ROW row;
        std::cout << "============================================\n";
        std::cout << "          Pending Payments\n";
        std::cout << "============================================\n";
        std::cout << std::setw(15) << "Booking ID"
            << std::setw(20) << "Total Price\n";
        std::cout << "--------------------------------------------\n";
        while ((row = mysql_fetch_row(result))) {
            std::cout << std::setw(15) << row[0]
                << std::setw(20) << row[1] << "\n";
        }
        mysql_free_result(result);
        std::cout << "============================================\n";
    }
    else {
        std::cout << "\nNo pending payments found.\n";
    }
}

void PaymentManagement::continuePayment() {
    std::ostringstream query;
    query << "SELECT booking_id, total_price FROM bookings WHERE user_id = '" << userID
        << "' AND payment_status = 'pending'";

    MYSQL_RES* result = dbConn.fetchResults(query.str());
    if (result) {
        MYSQL_ROW row;
        int index = 1;
        std::vector<int> bookingIDs;

        std::cout << "Pending Bookings:\n";
        while ((row = mysql_fetch_row(result))) {
            std::cout << index << ". Booking ID: " << row[0] << ", Total Price: $" << row[1] << "\n";
            bookingIDs.push_back(atoi(row[0])); // Store booking IDs for validation
            index++;
        }

        if (bookingIDs.empty()) {
            std::cout << "No pending payments.\n";
            mysql_free_result(result);
            return;
        }

        int selectedIndex;
        std::cout << "Select a booking to continue payment: ";
        std::cin >> selectedIndex;

        // Validate selection
        if (selectedIndex < 1 || selectedIndex > bookingIDs.size()) {
            std::cout << "Invalid selection.\n";
            mysql_free_result(result);
            return;
        }

        int selectedBookingID = bookingIDs[selectedIndex - 1];

        // Retrieve the total price for the selected booking
        std::ostringstream priceQuery;
        priceQuery << "SELECT total_price FROM bookings WHERE booking_id = " << selectedBookingID;
        double totalPrice = std::stod(dbConn.fetchSingleResult(priceQuery.str()));

        double discount = 0.0;
        char useVoucher;
        std::cout << "Do you have a voucher code? (Y/N): ";
        std::cin >> useVoucher;

        if (tolower(useVoucher) == 'y') {
            std::string voucherCode;
            std::cout << "Enter voucher code: ";
            std::cin.ignore(); // Clear the input buffer
            std::getline(std::cin, voucherCode);

            // Check if the voucher code is valid
            if (voucherCode == "VIPMEMB") {
                discount = totalPrice * 0.10;
                totalPrice -= discount;
                std::cout << "Voucher applied! You received a 10% discount.\n";
            }
            else {
                std::cout << "Invalid voucher code. No discount applied.\n";
            }
        }

        // Prompt user for payment method
        std::string paymentMethod;
        bool validPaymentMethod = false;

        do {
            std::cin.ignore();
            std::cout << "Enter payment method (Credit Card, PayPal, Bank Transfer): "; 
            std::getline(std::cin, paymentMethod);

            // Validate the payment method
            if (paymentMethod == "Credit Card" || paymentMethod == "PayPal" || paymentMethod == "Bank Transfer") {
                validPaymentMethod = true;
            }
            else {
                std::cout << "Invalid payment method. Please enter a valid option.\n";
            }
        } while (!validPaymentMethod);

        // Display a neatly formatted receipt
        std::cout << "\n------------------------------------------\n";
        std::cout << "                  RECEIPT                 \n";
        std::cout << "------------------------------------------\n";
        std::cout << std::left << std::setw(20) << "Booking ID" << ": " << selectedBookingID << '\n';
        std::cout << std::left << std::setw(20) << "Payment Method" << ": " << paymentMethod << '\n';
        std::cout << std::left << std::setw(20) << "Original Price" << ": $" << std::fixed << std::setprecision(2) << (totalPrice + discount) << '\n';
        std::cout << std::left << std::setw(20) << "Discount" << ": -$" << discount << '\n';
        std::cout << std::left << std::setw(20) << "Total Price" << ": $" << totalPrice << '\n';
        std::cout << "------------------------------------------\n";

        // Record the payment in the payments table
        std::ostringstream paymentInsertQuery;
        paymentInsertQuery << "INSERT INTO payments (booking_id, user_id, amount_paid, payment_method) "
            << "VALUES (" << selectedBookingID << ", " << userID << ", " << totalPrice << ", "
            << "'" << dbConn.escapeString(paymentMethod) << "')";

        if (dbConn.executeQuery(paymentInsertQuery.str())) {
            // Update the booking's payment status to 'completed'
            std::ostringstream updateBookingQuery;
            updateBookingQuery << "UPDATE bookings SET payment_status = 'completed' WHERE booking_id = " << selectedBookingID;
            dbConn.executeQuery(updateBookingQuery.str());

            std::cout << "Payment completed successfully.\n";
        }
        else {
            std::cout << "Failed to record payment. Please try again.\n";
        }

        mysql_free_result(result);
    }
    else {
        std::cout << "No pending payments.\n";
    }
}



