#pragma once

#include <utility>
// Pretty simple RAII implementation for Booking
namespace RAII {
    template<class Provider>
    class Booking {
    public:
        Booking(Provider* provider, int id) : provider(std::move(provider)), id(id) {}

        Booking(const Booking&) = delete;
        Booking& operator =(const Booking&) = delete;

        Booking(Booking&& other) noexcept {
            provider = std::move(other.provider);
            other.provider = nullptr;

            id = std::move(other.id);
        }

        Booking& operator =(Booking&& other) noexcept {
            provider = std::move(other.provider);
            other.provider = nullptr;

            id = std::move(other.id);

            return *this;
        }

        ~Booking() {
            if (provider)
                provider->CancelOrComplete(*this);
        }

    private:
        Provider* provider;
        int id;
    };
}
