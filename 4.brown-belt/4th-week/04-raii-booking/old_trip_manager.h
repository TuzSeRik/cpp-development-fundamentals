#include <vector>

#include "old_booking_providers.h"

using namespace std;
// Supplemental information to give us information about Booking interface
class Trip {
private:
    HotelProvider& hotel_provider;
    FlightProvider& flight_provider;

public:
    vector<HotelProvider::BookingId> hotels;
    vector<FlightProvider::BookingId> flights;

    Trip(HotelProvider& hp, FlightProvider& fp)
            : hotel_provider(hp),
              flight_provider(fp)
    {
    }

    Trip(const Trip&) = delete;
    Trip(Trip&&) = default;

    Trip& operator=(const Trip&) = delete;

    void Cancel() {
        for (auto& id : hotels) {
            HotelProvider::Cancel(id);
        }
        hotels.clear();
        for (auto& id : flights) {
            FlightProvider::Cancel(id);
        }
        flights.clear();
    }

    ~Trip() {
        Cancel();
    }

};

class TripManager {
public:
    struct BookingData {
        string city_from;
        string city_to;
        string date_from;
        string date_to;
    };

    Trip Book(const BookingData& data) {
        Trip trip(hotel_provider, flight_provider);
        {
            FlightProvider::BookingData data;
            trip.flights.push_back(FlightProvider::Book(data));
        }
        {
            HotelProvider::BookingData data;
            trip.hotels.push_back(HotelProvider::Book(data));
        }
        {
            FlightProvider::BookingData data;
            trip.flights.push_back(FlightProvider::Book(data));
        }
        return trip;
    }

    static void Cancel(Trip& trip) {
        trip.Cancel();
    }

private:
    HotelProvider hotel_provider;
    FlightProvider flight_provider;
};
