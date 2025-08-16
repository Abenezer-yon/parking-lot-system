#include "parking_lot.h"
#include <iostream>

void displayMenu() {
    std::cout << "\nParking Lot System\n"
              << "1. Park Vehicle\n"
              << "2. Remove Vehicle\n"
              << "3. View Records\n"
              << "4. View Parking Status\n"
              << "5. Exit\n"
              << "Choice: ";
}

int main() {
    int capacity;
    std::cout << "Enter parking capacity: ";
    std::cin >> capacity;
    
    ParkingLot lot(capacity);
    int choice;
    
    do {
        displayMenu();
        std::cin >> choice;
        
        try {
            switch (choice) {
                case 1: {
                    std::string plate;
                    std::cout << "Enter license plate: ";
                    std::cin >> plate;
                    lot.parkVehicle(plate);
                    break;
                }
                case 2: {
                    int slot;
                    std::cout << "Enter slot number: ";
                    std::cin >> slot;
                    lot.removeVehicle(slot);
                    break;
                }
                case 3:
                    lot.displayRecords();
                    break;
                case 4:
                    lot.displayParkingLot();
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } while (choice != 5);
    
    return 0;
}