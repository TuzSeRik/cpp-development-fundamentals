#pragma once

#include <stdexcept>
#include <string>

using namespace std;
// Supplemental information to give us information about Booking interface
class FlightProvider {
public:
    using BookingId = int;

    struct BookingData {
        string city_from;
        string city_to;
        string date;
    };

    static BookingId Book(const BookingData& data) {
        if (counter >= capacity) {
            throw runtime_error("Flight overbooking");
        }
        ++counter;
        return counter;
    }

    static void Cancel(const BookingId& id) {
        --counter;
        if (counter < 0) {
            throw logic_error("Too many flights have been canceled");
        }
    }

public:
    static int capacity;
    static int counter;
};

class HotelProvider {
public:
    using BookingId = int;

    struct BookingData {
        string city;
        string date_from;
        string date_to;
    };

    static BookingId Book(const BookingData& data) {
        if (counter >= capacity) {
            throw runtime_error("Hotel overbooking");
        }
        ++counter;
        return counter;
    }

    static void Cancel(const BookingId& id) {
        --counter;
        if (counter < 0) {
            throw logic_error("Too many hotels have been canceled");
        }
    }

public:
    static int capacity;
    static int counter;
};
