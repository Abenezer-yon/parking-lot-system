#ifndef PARKING_LOT_H
#define PARKING_LOT_H

#include <string>
#include <unordered_map>

struct Vehicle {
    std::string licensePlate;
    std::string entryTime;
    std::string exitTime;
    int slot;
    Vehicle* next;
};

struct QueueNode {
    int slot;
    QueueNode* next;
};

class ParkingLot {
private:
    int capacity;
    QueueNode* front;
    QueueNode* rear;
    std::unordered_map<int, Vehicle*> slotToVehicle;
    Vehicle* recordsHead;

    // Private methods
    void enqueue(int slot);
    int dequeue();
    bool isQueueEmpty() const;
    Vehicle* mergeSort(Vehicle* head);
    Vehicle* merge(Vehicle* left, Vehicle* right);
    float calculateHours(const std::string& entry, const std::string& exit);
    void saveToFile(Vehicle* vehicle);
    void loadFromFile();
    
public:
    ParkingLot(int cap);
    ~ParkingLot();
    
    void parkVehicle(const std::string& licensePlate);
    void removeVehicle(int slot);
    void displayRecords();
    void displayParkingLot();
    bool isFull() const;
    bool isEmpty() const;
    int getCapacity() const;
    int getAvailableSlots() const;
};

#endif