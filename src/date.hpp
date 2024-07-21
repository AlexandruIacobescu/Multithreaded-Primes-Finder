#pragma once

#include <iostream>
#include <ctime>

class Date {
private:
    short year;
    short month;
    short day;

public:
    // Constructor
    Date(short yyyy, short mm, short dd) {
        if (isValidDate(yyyy, mm, dd)) {
            year = yyyy;
            month = mm;
            day = dd;
        } else {
            throw std::invalid_argument("Invalid date arguments");
        }
    }

    // Method to check if the date is valid
    bool isValidDate(short yyyy, short mm, short dd) {
        // Basic validation for month and day
        if (mm < 1 || mm > 12 || dd < 1 || dd > 31) {
            return false;
        }

        // Additional checks for specific months
        if (mm == 4 || mm == 6 || mm == 9 || mm == 11) {
            return dd <= 30;    
        } else if (mm == 2) {
            // Leap year check
            if ((yyyy % 4 == 0 && yyyy % 100 != 0) || (yyyy % 400 == 0)) {
                return dd <= 29;
            } else {
                return dd <= 28;
            }
        }

        return true;
    }

    // Method to convert Date object to string
    std::string to_string(std::string format = "yyyy-mm-dd") const {
        return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
    }

    // Overloading << operator
    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << date.to_string();
        return os;
    }

    // Static method to get current system date
    static Date get_current_date() {
        std::time_t current_time = std::time(nullptr);
        std::tm* local_time = std::localtime(&current_time);
        short year = local_time->tm_year + 1900;
        short month = local_time->tm_mon + 1;
        short day = local_time->tm_mday;
        return Date(year, month, day);
    }
};
