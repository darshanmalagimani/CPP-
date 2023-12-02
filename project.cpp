#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <regex>
#include <ctime>
#include <sstream>

namespace ConferenceSystem {

    class ConferenceBase {
    public:
        virtual ~ConferenceBase() = default;

        virtual std::string getName() const = 0;
        virtual std::string getAnchor() const = 0;
        virtual std::string getTime() const = 0;
        virtual std::string getDate() const = 0;
    };

    class Conference : public ConferenceBase {
    public:
        Conference(const std::string& name, const std::string& anchor, const std::string& time, const std::string& date)
            : name(name), anchor(anchor), time(time), date(date) {}

        Conference(const std::string& name, const std::string& anchor)
            : Conference(name, anchor, "N/A", "N/A") {}

        std::string getName() const override { return name; }
        std::string getAnchor() const override { return anchor; }
        std::string getTime() const override { return time; }
        std::string getDate() const override { return date; }

        static Conference createDefaultConference() {
            return Conference("Default Conference", "Default Anchor", "10:00 AM", "2023-01-01");
        }

    private:
        std::string name;
        std::string anchor;
        std::string time;
        std::string date;
    };

    class ConferenceRoom {
    public:
        ConferenceRoom() : lastRoomNumber("C0") {}  // Start with "C0"

        void bookConference(const ConferenceBase* conference) {
            // Increment the room number for each new conference
            lastRoomNumber[1]++;  // Increment the digit after 'C'

            if (lastRoomNumber[1] > '9') {
                // Reset to '1' when reaching '9'
                lastRoomNumber[1] = '1';
                lastRoomNumber[0]++;
            }

            conferences.push_back(std::make_pair(conference, lastRoomNumber));
            std::cout << "Thank you for booking the conference in Room " << lastRoomNumber << "!" << std::endl;

            // Save conference details to CSV file
            saveConferenceToCSV(conference, lastRoomNumber);
        }

        void displayConferences() const {
            if (conferences.empty()) {
                std::cout << "No conferences booked in this room." << std::endl;
            } else {
                std::cout << "Conferences booked in this room:" << std::endl;
                for (const auto& conf : conferences) {
                    std::cout << "Conference Name: " << conf.first->getName() << std::endl;
                    std::cout << "Anchor: " << conf.first->getAnchor() << std::endl;
                    std::cout << "Time: " << conf.first->getTime() << std::endl;
                    std::cout << "Date: " << conf.first->getDate() << std::endl;
                    std::cout << "Room Number: " << conf.second << std::endl;
                    std::cout << "----------------------" << std::endl;
                }
            }

            std::cout << "Slots left: " << 15 - conferences.size() << std::endl;
            std::cout << "Slots booked: " << conferences.size() << std::endl;
        }

        const std::string& getLastBookedRoomNumber() const {
            return lastRoomNumber;
        }

        void displayAllConferencesFromFile() const {
            std::ifstream file("conferences.csv");
            if (!file.is_open()) {
                std::cerr << "Error: Unable to open conferences.csv for reading." << std::endl;
                return;
            }

            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string name, anchor, time, date, roomNumber;
                std::getline(iss, name, ',');
                std::getline(iss, anchor, ',');
                std::getline(iss, time, ',');
                std::getline(iss, date, ',');
                std::getline(iss, roomNumber, ',');

                std::cout << "Conference Name: " << name << std::endl;
                std::cout << "Anchor: " << anchor << std::endl;
                std::cout << "Time: " << time << std::endl;
                std::cout << "Date: " << date << std::endl;
                std::cout << "Room Number: " << roomNumber << std::endl;
                std::cout << "----------------------" << std::endl;
            }

            file.close();
        }

    private:
        std::vector<std::pair<const ConferenceBase*, std::string>> conferences;
        std::string lastRoomNumber;

        bool isRoomBooked(const std::string& roomNumber) const {
            // Check if the room number is already booked
            for (const auto& conf : conferences) {
                if (conf.second == roomNumber) {
                    return true;
                }
            }
            return false;
        }

        void saveConferenceToCSV(const ConferenceBase* conference, const std::string& roomNumber) const {
            std::ofstream file("conferences.csv", std::ios_base::app);  // Open file in append mode

            if (!file.is_open()) {
                std::cerr << "Error: Unable to open conferences.csv for writing." << std::endl;
                return;
            }

            file << conference->getName() << "," << conference->getAnchor() << ","
                 << conference->getTime() << "," << conference->getDate() << ","
                 << roomNumber << "\n";

            file.close();
        }
    };

    namespace Utils {
        inline std::string getCurrentDateTime() {
            return "2023-11-11 12:00 PM";
        }

        auto isConferenceToday = [](const ConferenceBase& conference) {
            // Check if the conference date is today
            time_t now = std::time(0);
            tm* currentDate = localtime(&now);
            std::stringstream ss(conference.getDate());
            int year, month, day;
            char discard;
            ss >> day >> discard >> month >> discard >> year;

            return currentDate->tm_mday == day && currentDate->tm_mon + 1 == month && currentDate->tm_year + 1900 == year;
        };

        void displayConferenceDetails(const ConferenceBase& conference, const std::string& roomNumber) {
            std::cout << "Conference Details:" << std::endl;
            std::cout << "Name: " << conference.getName() << std::endl;
            std::cout << "Anchor: " << conference.getAnchor() << std::endl;
            std::cout << "Time: " << conference.getTime() << std::endl;
            std::cout << "Date: " << conference.getDate() << std::endl;
            std::cout << "Room Number: " << roomNumber << std::endl;
        }
    };

    bool validateAnchorName(const std::string& anchor) {
        return std::regex_match(anchor, std::regex("^[a-zA-Z\\s]+$"));
    }

    bool validateTime(const std::string& time) {
        return std::regex_match(time, std::regex("^(0[1-9]|1[0-2]):[0-5][0-9] (AM|PM)$"));
    }

    bool validateDate(const std::string& date) {
        return std::regex_match(date, std::regex("^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/(19|20)\\d\\d$"));
    }

    bool isPastDate(const std::string& date) {
        // Check if the date is in the past
        time_t now = std::time(0);
        tm* currentDate = localtime(&now);
        std::stringstream ss(date);
        int year, month, day;
        char discard;
        ss >> day >> discard >> month >> discard >> year;

        return (currentDate->tm_year + 1900 > year) ||
               (currentDate->tm_year + 1900 == year && currentDate->tm_mon + 1 > month) ||
               (currentDate->tm_year + 1900 == year && currentDate->tm_mon + 1 == month && currentDate->tm_mday > day);
    }

    bool validateRoomSlot(const std::string& slot) {
        return std::regex_match(slot, std::regex("^C[1-9]|1[0-5]$"));
    }
}

int main() {
    using namespace ConferenceSystem;

    try {
        ConferenceRoom room;

        do {
            std::string name, anchor, date, time;
            std::cout << "Enter conference name: ";
            std::getline(std::cin, name);

            do {
                std::cout << "Enter anchor name: ";
                std::getline(std::cin, anchor);
            } while (!validateAnchorName(anchor));

            do {
                std::cout << "Enter date (DD/MM/YYYY): ";
                std::getline(std::cin, date);
            } while (!validateDate(date) || isPastDate(date));

            do {
                std::cout << "Enter time (HH:MM AM/PM): ";
                std::getline(std::cin, time);
            } while (!validateTime(time));

            Conference userConference(name, anchor, time, date);
            room.bookConference(&userConference);

            room.displayConferences();
            Utils::displayConferenceDetails(userConference, room.getLastBookedRoomNumber());

            if (Utils::isConferenceToday(userConference)) {
                std::cout << "Conference is happening today!" << std::endl;
            } else {
                std::cout << "Conference is not happening today." << std::endl;
            }

            std::cout << "Choose an option:" << std::endl;
            std::cout << "1. Display all conferences" << std::endl;
            std::cout << "2. Book another conference" << std::endl;
            std::cout << "3. Exit" << std::endl;

            int choice;
            std::cout << "Enter your choice (1/2/3): ";
            std::cin >> choice;
            std::cin.ignore();  // Consume the newline character

            if (choice == 1) {
                room.displayAllConferencesFromFile();
            } else if (choice == 3) {
                break;
            }

        } while (true);

        std::cout << "Thank you for using our Conference Room Booking System!" << std::endl;
        std::cout << "Developers: Deeksha, Amruth & Darshan" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
