-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 28, 2025 at 06:31 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `airlines1`
--

-- --------------------------------------------------------

--
-- Table structure for table `booked_seats`
--

CREATE TABLE `booked_seats` (
  `seat_id` int(11) NOT NULL,
  `booking_id` int(11) NOT NULL,
  `flight_id` int(11) NOT NULL,
  `seat_number` varchar(10) NOT NULL,
  `status` enum('available','booked') NOT NULL DEFAULT 'booked',
  `seat_status` enum('available','booked') NOT NULL DEFAULT 'booked'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `booked_seats`
--

INSERT INTO `booked_seats` (`seat_id`, `booking_id`, `flight_id`, `seat_number`, `status`, `seat_status`) VALUES
(1, 1, 1, 'A1', 'booked', 'booked'),
(2, 1, 1, 'B1', 'booked', 'booked'),
(3, 2, 1, 'D10', 'booked', 'booked'),
(4, 3, 1, 'D20', 'booked', 'booked'),
(5, 5, 1, 'A20', 'booked', 'booked'),
(6, 6, 1, 'A4', 'booked', 'booked'),
(7, 6, 1, 'A9', 'booked', 'booked'),
(13, 10, 1, 'D8', 'booked', 'booked'),
(14, 11, 1, 'B6', 'booked', 'booked'),
(15, 12, 1, 'B7', 'booked', 'booked'),
(16, 13, 1, 'B9', 'booked', 'booked'),
(17, 14, 1, 'C5', 'booked', 'booked'),
(18, 16, 1, 'C7', 'booked', 'booked'),
(20, 18, 7, 'A1', 'booked', 'booked'),
(21, 18, 7, 'B1', 'booked', 'booked');

-- --------------------------------------------------------

--
-- Table structure for table `bookings`
--

CREATE TABLE `bookings` (
  `booking_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `flight_id` int(11) NOT NULL,
  `total_price` decimal(10,2) NOT NULL,
  `booking_date` timestamp NOT NULL DEFAULT current_timestamp(),
  `payment_status` enum('pending','completed','refunded') NOT NULL DEFAULT 'pending'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `bookings`
--

INSERT INTO `bookings` (`booking_id`, `user_id`, `flight_id`, `total_price`, `booking_date`, `payment_status`) VALUES
(1, 2, 1, 300.00, '2025-01-23 09:55:58', 'completed'),
(2, 2, 1, 150.00, '2025-01-23 12:22:35', 'completed'),
(3, 2, 1, 150.00, '2025-01-23 13:15:23', 'completed'),
(4, 2, 1, 300.00, '2025-01-23 13:20:26', 'completed'),
(5, 2, 1, 150.00, '2025-01-23 13:26:32', 'completed'),
(6, 2, 1, 300.00, '2025-01-23 13:30:10', 'completed'),
(9, 2, 1, 300.00, '2025-01-23 14:11:32', 'refunded'),
(10, 2, 1, 150.00, '2025-01-23 14:25:09', 'completed'),
(11, 2, 1, 150.00, '2025-01-23 14:50:15', 'completed'),
(12, 2, 1, 150.00, '2025-01-23 14:51:23', 'completed'),
(13, 2, 1, 150.00, '2025-01-23 14:52:30', 'completed'),
(14, 2, 1, 150.00, '2025-01-23 14:54:07', 'completed'),
(15, 2, 1, 0.00, '2025-01-23 14:57:51', 'completed'),
(16, 2, 1, 150.00, '2025-01-23 14:58:12', 'completed'),
(17, 2, 1, 150.00, '2025-01-23 18:16:20', 'refunded'),
(18, 2, 7, 120.00, '2025-01-24 03:51:46', 'completed');

-- --------------------------------------------------------

--
-- Table structure for table `flights`
--

CREATE TABLE `flights` (
  `flight_id` int(11) NOT NULL,
  `flight_code` varchar(20) NOT NULL,
  `departure_location` varchar(100) NOT NULL,
  `arrival_location` varchar(100) NOT NULL,
  `departure_date` date NOT NULL,
  `departure_time` datetime NOT NULL,
  `total_seats` int(11) NOT NULL CHECK (`total_seats` >= 0 and `total_seats` <= 50),
  `price_per_seat` decimal(10,2) NOT NULL CHECK (`price_per_seat` > 0),
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `remaining_seats` int(11) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `flights`
--

INSERT INTO `flights` (`flight_id`, `flight_code`, `departure_location`, `arrival_location`, `departure_date`, `departure_time`, `total_seats`, `price_per_seat`, `created_at`, `updated_at`, `remaining_seats`) VALUES
(1, '370', 'kl', 'penang', '2025-02-10', '2025-02-07 16:50:00', 30, 100.00, '2025-01-23 09:55:27', '2025-01-24 02:07:21', 31),
(4, '107', 'kl', 'sabah', '2025-03-10', '2025-03-10 10:10:00', 20, 150.00, '2025-01-24 02:06:05', '2025-01-24 02:07:55', 20),
(5, '110', 'kl', 'perak', '2025-02-05', '2025-02-05 13:50:00', 30, 75.00, '2025-01-24 03:46:01', '2025-01-24 03:46:01', 30),
(6, '111', 'kl', 'terengganu', '2025-03-03', '2025-03-03 01:30:00', 25, 90.00, '2025-01-24 03:47:06', '2025-01-24 03:47:06', 25),
(7, '231', 'kl', 'malacca', '2025-03-01', '2025-03-01 10:10:00', 30, 60.00, '2025-01-24 03:48:02', '2025-01-24 03:52:06', 28),
(8, '459', 'kl', 'langkawi', '2025-02-20', '2025-02-20 18:30:00', 30, 200.00, '2025-01-24 03:49:08', '2025-01-24 03:49:08', 30),
(10, '131', 'kl', 'penang', '2025-01-29', '2025-01-29 12:30:00', 30, 150.00, '2025-01-24 04:05:03', '2025-01-24 04:05:03', 30);

-- --------------------------------------------------------

--
-- Table structure for table `payments`
--

CREATE TABLE `payments` (
  `payment_id` int(11) NOT NULL,
  `booking_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `amount_paid` decimal(10,2) NOT NULL,
  `payment_date` timestamp NOT NULL DEFAULT current_timestamp(),
  `payment_method` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `payments`
--

INSERT INTO `payments` (`payment_id`, `booking_id`, `user_id`, `amount_paid`, `payment_date`, `payment_method`) VALUES
(1, 1, 2, 300.00, '2025-01-23 09:56:19', 'Paypal'),
(2, 2, 2, 150.00, '2025-01-23 12:24:33', 'Credit Card'),
(3, 3, 2, 150.00, '2025-01-23 13:16:24', 'PayPal'),
(5, 9, 2, 300.00, '2025-01-23 14:12:44', 'Bank Transfer'),
(6, 4, 2, 270.00, '2025-01-23 14:26:02', 'Bank Transfer'),
(7, 6, 2, 270.00, '2025-01-23 14:58:43', 'Bank Transfer'),
(8, 5, 2, 135.00, '2025-01-23 15:01:19', 'PayPal'),
(9, 10, 2, 150.00, '2025-01-23 17:24:11', 'PayPal'),
(10, 11, 2, 135.00, '2025-01-23 17:28:05', 'Credit Card'),
(11, 13, 2, 150.00, '2025-01-23 17:28:34', 'PayPal'),
(12, 12, 2, 135.00, '2025-01-23 18:16:53', 'PayPal'),
(13, 15, 2, 0.00, '2025-01-23 18:23:13', 'Credit Card'),
(14, 16, 2, 150.00, '2025-01-23 18:23:29', 'Credit Card'),
(15, 14, 2, 150.00, '2025-01-24 03:51:34', 'PayPal'),
(16, 18, 2, 108.00, '2025-01-24 04:28:40', 'PayPal');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE `users` (
  `user_id` int(11) NOT NULL,
  `username` varchar(50) NOT NULL,
  `password` varchar(255) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `email` varchar(100) NOT NULL,
  `phone_number` varchar(15) DEFAULT NULL,
  `role` enum('customer','admin') NOT NULL DEFAULT 'customer',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp(),
  `updated_at` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`user_id`, `username`, `password`, `full_name`, `email`, `phone_number`, `role`, `created_at`, `updated_at`) VALUES
(1, 'admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'System Admin', 'admin@airline.com', NULL, 'admin', '2025-01-23 09:52:27', '2025-01-23 09:52:27'),
(2, 'sha', '6b990f1a6bb3a503fc8b12583b44b5be532707c6774a128f866f6fceb7d5a470', 'sharveena', 'sha@gmail.com', '0123591807', 'customer', '2025-01-23 09:53:06', '2025-01-23 09:53:06'),
(35, 'vv', 'ebf5b9a3b89509d9e3f62da144cc25897e526498e9ef937b7223a6f73a3ed8ce', 'vishal', 'vv@gmail.com', '0123456789', 'customer', '2025-01-23 17:45:41', '2025-01-23 17:45:41');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `booked_seats`
--
ALTER TABLE `booked_seats`
  ADD PRIMARY KEY (`seat_id`),
  ADD UNIQUE KEY `unique_seat` (`flight_id`,`seat_number`),
  ADD KEY `booking_id` (`booking_id`);

--
-- Indexes for table `bookings`
--
ALTER TABLE `bookings`
  ADD PRIMARY KEY (`booking_id`),
  ADD KEY `flight_id` (`flight_id`),
  ADD KEY `idx_bookings_user_id` (`user_id`);

--
-- Indexes for table `flights`
--
ALTER TABLE `flights`
  ADD PRIMARY KEY (`flight_id`),
  ADD UNIQUE KEY `flight_code` (`flight_code`),
  ADD KEY `idx_flights_departure_arrival` (`departure_location`,`arrival_location`);

--
-- Indexes for table `payments`
--
ALTER TABLE `payments`
  ADD PRIMARY KEY (`payment_id`),
  ADD KEY `booking_id` (`booking_id`),
  ADD KEY `idx_payments_user_id` (`user_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`user_id`),
  ADD UNIQUE KEY `username` (`username`),
  ADD UNIQUE KEY `email` (`email`),
  ADD KEY `idx_users_username` (`username`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `booked_seats`
--
ALTER TABLE `booked_seats`
  MODIFY `seat_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=22;

--
-- AUTO_INCREMENT for table `bookings`
--
ALTER TABLE `bookings`
  MODIFY `booking_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=19;

--
-- AUTO_INCREMENT for table `flights`
--
ALTER TABLE `flights`
  MODIFY `flight_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `payments`
--
ALTER TABLE `payments`
  MODIFY `payment_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=17;

--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `user_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=57;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `booked_seats`
--
ALTER TABLE `booked_seats`
  ADD CONSTRAINT `booked_seats_ibfk_1` FOREIGN KEY (`booking_id`) REFERENCES `bookings` (`booking_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `booked_seats_ibfk_2` FOREIGN KEY (`flight_id`) REFERENCES `flights` (`flight_id`) ON DELETE CASCADE;

--
-- Constraints for table `bookings`
--
ALTER TABLE `bookings`
  ADD CONSTRAINT `bookings_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `bookings_ibfk_2` FOREIGN KEY (`flight_id`) REFERENCES `flights` (`flight_id`) ON DELETE CASCADE;

--
-- Constraints for table `payments`
--
ALTER TABLE `payments`
  ADD CONSTRAINT `payments_ibfk_1` FOREIGN KEY (`booking_id`) REFERENCES `bookings` (`booking_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `payments_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
