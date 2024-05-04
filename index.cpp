#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

class Car {
private:
    string brand;
    string model;
    string color;
    int year;
    double pricePerDay;
    bool available;

public:
    Car(const string& brand, const string& model, const string& color, int year, double pricePerDay)
        : brand(brand), model(model), color(color), year(year), pricePerDay(pricePerDay), available(true) {}

    string getBrand() const { return brand; }
    string getModel() const { return model; }
    string getColor() const { return color; }
    int getYear() const { return year; }
    double getPricePerDay() const { return pricePerDay; }
    bool isAvailable() const { return available; }

    void rent() { available = false; }
    void returnCar() { available = true; }
};

class Customer {
private:
    string name;
    string licenseNumber;
    vector<int> rentedCarIndices; 
    time_t rentStartDate;

public:
    Customer(const string& name, const string& licenseNumber) 
        : name(name), licenseNumber(licenseNumber) {}

    string getName() const { return name; }
    string getLicenseNumber() const { return licenseNumber; }
    vector<int>& getRentedCarIndices() { return rentedCarIndices; }
    time_t getRentStartDate() const { return rentStartDate; }
    void  setRentedCarIndices(int carindex) { rentedCarIndices.push_back(carindex);}

    void rentCar(int carIndex, time_t rentStartDate) { 
        rentedCarIndices.push_back(carIndex); 
        this->rentStartDate = rentStartDate;
    }

    void returnCar(int carIndex) { 
        auto it = find(rentedCarIndices.begin(), rentedCarIndices.end(), carIndex); // Use find from std namespace
        if (it != rentedCarIndices.end()) {
            rentedCarIndices.erase(it);
        }
    }
};

class CarRentalSystem {
private:
    vector<Car> cars;
    vector<Customer> customers;

public:
    void addCar(const Car& car) { cars.push_back(car); }
    const Car& getCar(int index) const {
        if (index >= 0 && index < cars.size()) {
            return cars[index];
        } else {
            throw out_of_range("Invalid car index");
        }
    }
    void rentCar(Customer& customer, int carIndex) {
        if (carIndex >= 0 && carIndex < cars.size() && cars[carIndex].isAvailable()) {
            cars[carIndex].rent();
            customers.push_back(customer);
            //customer.setRentedCarIndices(carIndex);
            customer.rentCar(carIndex, time(nullptr)); // Pass current time as rent start date
            cout << "Car rented successfully to " << customer.getName() << endl;
            // Show all rented cars to the customer
            cout << "Cars rented by " << customer.getName() << ":" << endl;
            vector<int>& rentedCarIndices = customer.getRentedCarIndices();
            for (const int& rentedCarIndex : rentedCarIndices) {
                const Car& rentedCar = cars[rentedCarIndex];
                cout << "Brand: " << rentedCar.getBrand() << ", Model: " << rentedCar.getModel() << ", Color: " << rentedCar.getColor() << ", Year: " << rentedCar.getYear() << ", Price/Day: " << rentedCar.getPricePerDay() << endl;
            }
        } else {
            cout << "Car is not available for rent." << endl;
        }
    }

    void returnCar(Customer& customer, int carIndex) {
        vector<int>& rentedCarIndices = customer.getRentedCarIndices();
        if (carIndex >= 0 && carIndex < rentedCarIndices.size()) {
            int rentedCarIndex = rentedCarIndices[carIndex];
            time_t returnTime = time(nullptr);
            double rentAmount = calculateRentAmount(customer.getRentStartDate(), returnTime, cars[rentedCarIndex].getPricePerDay());
            cout << "Rent amount for the returned car: $" << rentAmount << endl;
            cars[rentedCarIndex].returnCar();
            rentedCarIndices.erase(rentedCarIndices.begin() + carIndex);
            cout << "Car returned successfully by " << customer.getName() << endl;
        } else {
            cout << "Invalid car index." << endl;
        }
    }
    void displayAvailableCars() const {
        cout << setw(10) << "Brand" << setw(10) << "Model" << setw(10) << "Color" << setw(6) << "Year" << setw(10) << "Price/Day" << setw(10) << "Available" << endl;
        for (const Car& car : cars) {
            cout << setw(10) << car.getBrand() << setw(10) << car.getModel() << setw(10) << car.getColor() << setw(6) << car.getYear() << setw(10) << car.getPricePerDay() << setw(10) << (car.isAvailable() ? "Yes" : "No") << endl;
        }
    }
     double calculateRentAmount(time_t rentStartDate, time_t returnTime, double pricePerDay) {
        double rentAmount = 0.0;
        double secondsPerDay = 24 * 60 * 60; // Seconds in a day
        double rentalDuration = difftime(returnTime, rentStartDate) / secondsPerDay; // Convert duration to days
        rentAmount = rentalDuration * pricePerDay;
        return rentAmount;
    }
};

int main() {
    CarRentalSystem carRentalSystem;

    // Add cars to the inventory
    carRentalSystem.addCar(Car("Toyota", "Corolla", "Black", 2020, 50.0));
    carRentalSystem.addCar(Car("Honda", "Civic", "Blue", 2019, 60.0));
    carRentalSystem.addCar(Car("Ford", "Mustang", "Red", 2018, 80.0));

    char repeatChoice;
    
        string name, licenseNumber;
        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Enter your license number: ";
        getline(cin, licenseNumber);

        Customer customer(name, licenseNumber);
        do {
        char choice;
        cout << "Would you like to rent a car? (y/n): ";
        cin >> choice;
        cin.ignore(); // Clear the input buffer
        if (choice == 'y' || choice == 'Y') {
            cout << "Available Cars:" << endl;
            carRentalSystem.displayAvailableCars();
            int carIndex;
            cout << "Enter the index of the car you want to rent: ";
            cin >> carIndex;
            carRentalSystem.rentCar(customer, carIndex);
        } else if (choice == 'n' || choice == 'N') {
            cout << "Would you like to return a rented car? (y/n): ";
            char returnChoice;
            cin >> returnChoice;
            if (returnChoice == 'y' || returnChoice == 'Y') {
                const vector<int>& rentedCar = customer.getRentedCarIndices();
                if (rentedCar.empty()) {
                    cout << "You have no cars rented at the moment." << endl;
                } else {
                    cout << "Cars rented by you:" << endl;
                    for (size_t i = 0; i < rentedCar.size(); ++i) {
                        cout << i + 1 << ". ";
                        const Car& rentedCa = carRentalSystem.getCar(rentedCar[i]);
                        cout << "Brand: " << rentedCa.getBrand() << ", Model: " << rentedCa.getModel() << ", Color: " << rentedCa.getColor() << ", Year: " << rentedCa.getYear() << ", Price/Day: " << rentedCa.getPricePerDay() << endl;
                    }
                    cout << "Enter the index of the car you want to return: ";
                    int returnIndex;
                    cin >> returnIndex;
                    carRentalSystem.returnCar(customer, returnIndex - 1);
                }
            }
             }else {
                cout << "Thank you for using our car rental service!" << endl;
            }
        

        cout << "Do you want to repeat the process? (y/n): ";
        cin >> repeatChoice;
        cin.ignore(); // Clear the input buffer

    } while (repeatChoice == 'y' || repeatChoice == 'Y');

    return 0;
}


