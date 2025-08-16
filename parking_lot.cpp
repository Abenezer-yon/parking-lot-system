#include "parking_lot.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>  // Add this line

// Queue implementation
void ParkingLot::enqueue(int slot) {
    QueueNode* newNode = new QueueNode{slot, nullptr};
    if (rear == nullptr) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

int ParkingLot::dequeue() {
    if (isQueueEmpty()) {
        throw std::runtime_error("Queue underflow");
    }
    QueueNode* temp = front;
    int slot = temp->slot;
    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }
    delete temp;
    return slot;
}

bool ParkingLot::isQueueEmpty() const {
    return front == nullptr;
}

// Constructor
ParkingLot::ParkingLot(int cap) 
    : capacity(cap), front(nullptr), rear(nullptr), recordsHead(nullptr) {
    if (cap <= 0) {
        throw std::invalid_argument("Capacity must be positive");
    }
    for (int i = 1; i <= cap; i++) {
        enqueue(i);
    }
    loadFromFile();
}

// Destructor
ParkingLot::~ParkingLot() {
    // Clear queue
    while (!isQueueEmpty()) {
        dequeue();
    }
    // Clear records
    Vehicle* current = recordsHead;
    while (current != nullptr) {
        Vehicle* next = current->next;
        delete current;
        current = next;
    }
}

// Parking operations
void ParkingLot::parkVehicle(const std::string& licensePlate) {
    if (licensePlate.empty()) {
        throw std::invalid_argument("License plate cannot be empty");
    }
    if (isFull()) {
        throw std::runtime_error("Parking lot is full");
    }

    time_t now = time(0);
    tm* ltm = localtime(&now);
    std::stringstream entryTime;
    entryTime << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
              << std::setw(2) << std::setfill('0') << ltm->tm_min;

    int slot = dequeue();
    Vehicle* newVehicle = new Vehicle{licensePlate, entryTime.str(), "", slot, recordsHead};
    slotToVehicle[slot] = newVehicle;
    recordsHead = newVehicle;
    saveToFile(newVehicle);
    std::cout << "Vehicle parked at Slot " << slot << std::endl;
}

void ParkingLot::removeVehicle(int slot) {
    if (slot < 1 || slot > capacity) {
        throw std::out_of_range("Invalid slot number");
    }
    auto it = slotToVehicle.find(slot);
    if (it == slotToVehicle.end()) {
        throw std::runtime_error("Slot is already empty");
    }

    time_t now = time(0);
    tm* ltm = localtime(&now);
    std::stringstream exitTime;
    exitTime << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
             << std::setw(2) << std::setfill('0') << ltm->tm_min;

    Vehicle* vehicle = it->second;
    vehicle->exitTime = exitTime.str();
    enqueue(slot);
    slotToVehicle.erase(it);

    float hours = calculateHours(vehicle->entryTime, vehicle->exitTime);
    float fee = hours * 2.0f;

    std::cout << "\nReceipt:\n"
              << "License: " << vehicle->licensePlate << "\n"
              << "Slot: " << slot << "\n"
              << "Entry: " << vehicle->entryTime << "\n"
              << "Exit: " << vehicle->exitTime << "\n"
              << "Duration: " << hours << " hours\n"
              << "Fee: " << fee << "Birr" << "\n";

    saveToFile(vehicle);
}

// File operations
void ParkingLot::loadFromFile() {
    std::ifstream file("parking_records.txt");
    if (!file.is_open()) return;

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() == 4) {
            try {
                Vehicle* v = new Vehicle{tokens[0], tokens[1], tokens[2], std::stoi(tokens[3]), nullptr};
                if (v->exitTime.empty()) {
                    slotToVehicle[v->slot] = v;
                    // Rebuild queue
                    while (!isQueueEmpty()) dequeue();
                    for (int i = 1; i <= capacity; i++) {
                        if (slotToVehicle.find(i) == slotToVehicle.end()) {
                            enqueue(i);
                        }
                    }
                }
                v->next = recordsHead;
                recordsHead = v;
            } catch (...) {
                continue;
            }
        }
    }
    file.close();
}

void ParkingLot::saveToFile(Vehicle* vehicle) {
    std::ofstream file("parking_records.txt", std::ios::app);
    if (file.is_open()) {
        file << vehicle->licensePlate << ","
             << vehicle->entryTime << ","
             << vehicle->exitTime << ","
             << vehicle->slot << "\n";
        file.close();
    }
}

// Time calculation
float ParkingLot::calculateHours(const std::string& entry, const std::string& exit) {
    if (entry.empty() || exit.empty()) return 0.0f;

    int h1, m1, h2, m2;
    char colon;
    std::istringstream ss1(entry), ss2(exit);
    ss1 >> h1 >> colon >> m1;
    ss2 >> h2 >> colon >> m2;

    float hours = h2 - h1 + (m2 - m1)/60.0f;
    return (hours >= 0) ? hours : hours + 24.0f;
}

// Display methods
void ParkingLot::displayParkingLot() {
    std::cout << "\nCurrent Parking Status (" 
              << slotToVehicle.size() << "/" << capacity << " occupied):\n";
    std::cout << "-------------------------------------------------\n";
    
    const int slotsPerRow = 5;
    for (int i = 1; i <= capacity; i++) {
        if (i % slotsPerRow == 1) std::cout << "| ";
        std::cout << std::setw(2) << i << ":";
        std::cout << (slotToVehicle.find(i) != slotToVehicle.end() ? "P" : "F");
        std::cout << " | ";
        if (i % slotsPerRow == 0) std::cout << "\n-------------------------------------------------\n";
    }
}

void ParkingLot::displayRecords() {
    Vehicle* sorted = mergeSort(recordsHead);
    std::cout << "\nParking Records:\n";
    std::cout << "=================================================\n";
    
    for (Vehicle* current = sorted; current; current = current->next) {
        std::cout << "| " << std::setw(10) << current->licensePlate
                  << " | Slot:" << std::setw(3) << current->slot
                  << " | Entry:" << std::setw(6) << current->entryTime;
        
        if (!current->exitTime.empty()) {
            std::cout << " | Exit:" << std::setw(6) << current->exitTime;
            float hours = calculateHours(current->entryTime, current->exitTime);
            std::cout << " | Fee: " << std::fixed << std::setprecision(2) << hours * 2.0f << " Birr";
        }
        std::cout << " |\n";
    }
    std::cout << "=================================================\n";
}

// Merge sort
Vehicle* ParkingLot::mergeSort(Vehicle* head) {
    if (!head || !head->next) return head;
    
    Vehicle* slow = head;
    Vehicle* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Vehicle* mid = slow->next;
    slow->next = nullptr;
    
    return merge(mergeSort(head), mergeSort(mid));
}

Vehicle* ParkingLot::merge(Vehicle* left, Vehicle* right) {
    Vehicle dummy;
    Vehicle* tail = &dummy;
    
    while (left && right) {
        if (left->entryTime <= right->entryTime) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}

// Status methods
bool ParkingLot::isFull() const { 
    return isQueueEmpty(); 
}

bool ParkingLot::isEmpty() const { 
    return slotToVehicle.empty(); 
}

int ParkingLot::getCapacity() const { 
    return capacity; 
}

int ParkingLot::getAvailableSlots() const {
    int count = 0;
    QueueNode* current = front;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}