#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;
// Struct, needed to store description of route in map
struct RouteDescription {
    bool linear{};
    vector<string> stops;
    int numberOfStops{};
    int numberOfUniqueStops{};
    double lengthOfRoute{};
};
// Representing route in two fields to decompose it to store in map easily
struct Route {
    // Key
    string number;
    // Value
    RouteDescription routeDescription;
};
// Main class
class Database {
private:
    // Used in task constants
    constexpr const static double PI = 3.1415926535;
    const static int RADIUS = 6371;
    // Storing stop as identifier and pair of coordinates
    unordered_map<string, pair<double, double>> storageOfStops;
    // Storing route as identifier and structure with description
    unordered_map<string, RouteDescription> storageOfRoutes;
    // Method, calculating distance between provided stops
    double calculateLength(const vector<string>& stops) {
        double length = 0;

        for (auto prevIt = begin(stops), it = next(begin(stops)); it != end(stops); prevIt++, it++) {
            auto previousPair = storageOfStops.at(*prevIt);
            auto currentPair = storageOfStops.at(*it);

            double lat_1 = previousPair.first * PI/180.0;
            double long_1 = previousPair.second * PI/180.0;
            double lat_2 = currentPair.first * PI/180.0;
            double long_2 = currentPair.second * PI/180.0;

            length += acos(sin(lat_1) * sin(lat_2) +
                           cos(lat_1) * cos(lat_2) *
                           cos(abs(long_1 - long_2))) * RADIUS * 1000;
        }

        return length;
    }
    // Because creating set and calculating length is expensive action, we do it only when needed
    Route& initializeRoute(Route&& route) {
        // Number of stops depends on type of route
        if (route.routeDescription.linear)
            route.routeDescription.numberOfStops = static_cast<int>(route.routeDescription.stops.size()) * 2 - 1;
        else
            route.routeDescription.numberOfStops = route.routeDescription.stops.size();
        // Initializing number of unique stops by set
        route.routeDescription.numberOfUniqueStops = unordered_set(begin(route.routeDescription.stops),
                                                                   end(route.routeDescription.stops)).size();
        // Calculating length of route
        route.routeDescription.lengthOfRoute = calculateLength(route.routeDescription.stops)
                                               * (1 + route.routeDescription.linear);

        return route;
    }

public:
    void InsertStop(pair<string, pair<double, double>>&& stop) {
        const auto& [name, coordinates] = stop;
        storageOfStops[name] = coordinates;
    }

    void InsertRoute(Route&& route) {
        const auto& [number, description] = route;
        storageOfRoutes[number] = description;
    }
    // If route uninitialized, initialize it
    Route getRoute(const string& number) {
        if (storageOfRoutes.count(number)) {
            Route result = {number, storageOfRoutes.at(number)};
            result = (result.routeDescription.numberOfStops != 0) ? result : initializeRoute(move(result));
            return result;
        }
        // If no route - return 'empty' route
        return {number};
    }
};
// Simplifying input of stops
istream& operator >>(istream& input, pair<string, pair<double, double>>& stop) {
    string stopName, tmp;
    input >> stopName;
    // Inputting name
    while (stopName.back() != ':') {
        input >> tmp;
        stopName.append(" " + tmp);
    }
    stopName.erase(stopName.size() - 1);
    // Deleting trailing ':'
    // Inputting coordinates
    double x, y;
    cin >> x;
    cin.ignore(2);
    cin >> y;

    stop = {stopName, {x, y}};
    return input;
}
// Simplifying input of routes
istream& operator >>(istream& mainInput, Route& route) {
    string number, tmp, stopName, line;
    // Taking whole command
    getline(mainInput, line);
    // Taking number of bus (which is not only number) and erasing this part
    route.number = line.substr(1, line.rfind(':') - 1);
    line.erase(0, line.find(':') + 1);
    // Remaining part sending to stream
    stringstream input(line);
    while (input) {
        input >> tmp;
        // Adding part of stop name, until sign of next stop is read
        if (tmp != ">" && tmp != "-")
            stopName.append(tmp + " ");
        else {
            // Then erasing trailing space, sending stop to vector and checking type of route
            stopName.erase(stopName.size() - 1);
            route.routeDescription.stops.push_back(move(stopName));
            route.routeDescription.linear = tmp == "-";
        }
    }
    // Repeating for last stop, because it's not ended by next stop sign
    stopName.erase(stopName.size() - 2, 2);
    stopName.erase(stopName.rfind(' '), tmp.size());
    route.routeDescription.stops.push_back(move(stopName));

    return mainInput;
}
// Simplifying output of route
ostream& operator <<(ostream& output, const Route& route) {
    cout << "Bus " << route.number << ": ";
    // Checking if route is empty
    if (route.routeDescription.numberOfStops != 0)
        cout << route.routeDescription.numberOfStops << " stops on route, "
             << route.routeDescription.numberOfUniqueStops << " unique stops, "
             << fixed << setprecision(6) << route.routeDescription.lengthOfRoute << " route length" << "\n";
    else
        cout << "not found" << "\n";

    return output;
}

int main() {
    Database db;
    int numberOfCommands;
    string command;
    // First block - input of data
    cin >> numberOfCommands;
    for (int i = 0; i < numberOfCommands; i++) {
        cin >> command;

        if (command == "Stop") {
            pair<string, pair<double, double>> stop;
            cin >> stop;
            db.InsertStop(move(stop));
        }

        if (command == "Bus") {
            Route route;
            cin >> route;
            db.InsertRoute(move(route));
        }
    }
    // Second block - output of data
    cin >> numberOfCommands;
    for (int i = 0; i < numberOfCommands; i++) {
        cin >> command;

        if (command == "Bus") {
            string number;
            getline(cin, number);
            number = number.substr(number.find(' ') + 1, string::npos);
            cout << db.getRoute(number);
        }
    }
}
