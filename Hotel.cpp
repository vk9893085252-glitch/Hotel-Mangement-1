#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <utility>

using namespace std;

// ================= FILE NAMES =================

const string ROOMS_FILE = "rooms.txt";
const string FOODS_FILE = "foods.txt";
const string CUSTOMERS_FILE = "customers.txt";
const string BOOKINGS_FILE = "bookings.txt";
const string STAFF_FILE = "staff.txt";
const string ATTENDANCE_FILE = "attendance.txt";
const string PAYMENTS_FILE = "payments.txt";

// ================= INPUT FUNCTIONS =================

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin >> ws, value);
    return value;
}

int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        int value;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input. Enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(const string& prompt) {
    while (true) {
        cout << prompt;
        double value;

        if (cin >> value && value > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Enter a number greater than zero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string cleanField(string s) {
    replace(s.begin(), s.end(), '|', '/');
    return s;
}

vector<string> split(const string& line, char delim = '|') {
    vector<string> out;
    string item;
    stringstream ss(line);

    while (getline(ss, item, delim)) {
        out.push_back(item);
    }

    return out;
}

// ================= TEMPLATE FUNCTION =================

template <typename T>
void showMessage(const T& message) {
    cout << message << '\n';
}

// ================= ROOM CLASS =================

class Room {
private:
    int number{};
    string type;
    double price{};
    bool booked{};
    string owner;
    string guest;
    int days{};
    vector<int> foodQty;

    static int objectCount;

public:
    Room() : foodQty(100, 0) {
        ++objectCount;
    }

    Room(int n, string t, double p)
        : number(n),
          type(cleanField(std::move(t))),
          price(p),
          foodQty(100, 0) {

        if (n <= 0 || p <= 0 || type.empty()) {
            throw invalid_argument("Invalid room details.");
        }

        ++objectCount;
    }

    Room(const Room& r)
        : number(r.number),
          type(r.type),
          price(r.price),
          booked(r.booked),
          owner(r.owner),
          guest(r.guest),
          days(r.days),
          foodQty(r.foodQty) {

        ++objectCount;
    }

    Room(Room&& r) noexcept
        : number(r.number),
          type(std::move(r.type)),
          price(r.price),
          booked(r.booked),
          owner(std::move(r.owner)),
          guest(std::move(r.guest)),
          days(r.days),
          foodQty(std::move(r.foodQty)) {

        ++objectCount;
    }

    Room& operator=(const Room&) = default;
    Room& operator=(Room&&) noexcept = default;

    ~Room() {
        --objectCount;
    }

    int getNumber() const {
        return number;
    }

    string getType() const {
        return type;
    }

    double getPrice() const {
        return price;
    }

    bool isBooked() const {
        return booked;
    }

    string getOwner() const {
        return owner;
    }

    string getGuest() const {
        return guest;
    }

    int getDays() const {
        return days;
    }

    int getFoodQty(size_t i) const {
        return i < foodQty.size() ? foodQty[i] : 0;
    }

    void setPrice(double p) {
        if (p <= 0) {
            throw invalid_argument("Price must be positive.");
        }

        price = p;
    }

    void book(const string& user,
              const string& guestName,
              int stayDays) {

        if (booked) {
            throw runtime_error("Room is already booked.");
        }

        if (stayDays <= 0) {
            throw invalid_argument(
                "Days must be greater than zero."
            );
        }

        booked = true;
        owner = user;
        guest = guestName;
        days = stayDays;

        fill(foodQty.begin(), foodQty.end(), 0);
    }

    void addFood(size_t i, int quantity) {
        if (!booked) {
            throw runtime_error("Room is not booked.");
        }

        if (i >= foodQty.size() || quantity <= 0) {
            throw invalid_argument("Invalid food quantity.");
        }

        foodQty[i] += quantity;
    }

    void restoreBooking(bool b,
                        string own,
                        string g,
                        int d,
                        const vector<int>& quantities) {

        booked = b;
        owner = std::move(own);
        guest = std::move(g);
        days = d;

        fill(foodQty.begin(), foodQty.end(), 0);

        for (size_t i = 0;
             i < quantities.size() && i < foodQty.size();
             ++i) {

            foodQty[i] = quantities[i];
        }
    }

    void release() {
        booked = false;
        owner.clear();
        guest.clear();
        days = 0;

        fill(foodQty.begin(), foodQty.end(), 0);
    }

    static int getObjectCount() {
        return objectCount;
    }

    // Operator overloading
    friend ostream& operator<<(ostream& os, const Room& r) {
        os << left
           << setw(10) << r.number
           << setw(16) << r.type
           << setw(14) << fixed << setprecision(2) << r.price
           << (r.booked ? "Booked" : "Available");

        return os;
    }
};

int Room::objectCount = 0;

// ================= FOOD ITEM CLASS =================

class FoodItem {
private:
    string name;
    double price{};

public:
    FoodItem() = default;

    FoodItem(string n, double p)
        : name(cleanField(std::move(n))), price(p) {

        if (name.empty() || p <= 0) {
            throw invalid_argument(
                "Food name and positive price are required."
            );
        }
    }

    string getName() const {
        return name;
    }

    double getPrice() const {
        return price;
    }

    void setPrice(double p) {
        if (p <= 0) {
            throw invalid_argument("Price must be positive.");
        }

        price = p;
    }
};

// ================= BASE USER CLASS =================

class User {
protected:
    string username;
    string password;

public:
    User(string u = "", string p = "")
        : username(std::move(u)),
          password(std::move(p)) {}

    virtual ~User() = default;

    string getUsername() const {
        return username;
    }

    bool checkPassword(const string& p) const {
        return password == p;
    }

    string getPasswordForStorage() const {
        return password;
    }

    virtual void dashboard() const = 0;
};

// ================= ADMIN CLASS =================

class Admin : public User {
public:
    Admin() : User("admin", "admin123") {}

    void dashboard() const override {
        cout << "\n========== ADMIN DASHBOARD ==========\n"
             << "1. View all rooms\n"
             << "2. View available rooms\n"
             << "3. View all active bookings\n"
             << "4. Add new room\n"
             << "5. Add new food item\n"
             << "6. Change room/food prices\n"
             << "7. Order food for a room\n"
             << "8. Generate bill and checkout\n"
             << "9. View total revenue\n"
             << "10. Staff management\n"
             << "11. Logout\n";
    }
};

// ================= CUSTOMER CLASS =================

class Customer : public User {
private:
    string aadhaarNumber;

public:
    Customer(string u, string p, string aadhaar = "")
        : User(std::move(u), std::move(p)),
          aadhaarNumber(std::move(aadhaar)) {}

    string getAadhaarNumber() const {
        return aadhaarNumber;
    }

    void dashboard() const override {
        cout << "\n========== CUSTOMER DASHBOARD ==========\n"
             << "1. View all rooms\n"
             << "2. View available rooms\n"
             << "3. Book a room\n"
             << "4. View my bookings\n"
             << "5. Order food\n"
             << "6. View bill and checkout\n"
             << "7. Logout\n";
    }
};

// ================= STAFF CLASS =================

class Staff {
private:
    int id{};
    string name;
    string role;
    double dailyWage{};

public:
    Staff() = default;

    Staff(int i, string n, string r, double w)
        : id(i),
          name(cleanField(std::move(n))),
          role(cleanField(std::move(r))),
          dailyWage(w) {

        if (id <= 0 || name.empty() ||
            role.empty() || w <= 0) {

            throw invalid_argument("Invalid staff details.");
        }
    }

    int getID() const {
        return id;
    }

    string getName() const {
        return name;
    }

    string getRole() const {
        return role;
    }

    double getDailyWage() const {
        return dailyWage;
    }
};

// ================= ATTENDANCE STRUCTURE =================

struct Attendance {
    int staffID;
    string date;
    bool present;
};

// ================= PAYMENT STRUCTURE =================

struct Payment {
    int staffID;
    string period;
    double amount;
    string date;
};

// ================= HOTEL CLASS =================

class Hotel {
private:
    vector<Room> rooms;
    vector<FoodItem> foods;
    vector<Customer> customers;
    vector<Staff> staff;
    vector<Attendance> attendance;
    vector<Payment> payments;

    double roomRevenue = 0;
    double foodRevenue = 0;

    // Function overloading: search by room number
    int findRoom(int number) const {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].getNumber() == number) {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    // Function overloading: search available room by type
    int findRoom(const string& type) const {
        for (size_t i = 0; i < rooms.size(); ++i) {
            if (rooms[i].getType() == type &&
                !rooms[i].isBooked()) {

                return static_cast<int>(i);
            }
        }

        return -1;
    }

    int findStaff(int id) const {
        for (size_t i = 0; i < staff.size(); ++i) {
            if (staff[i].getID() == id) {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    bool userExists(const string& u) const {
        for (const auto& c : customers) {
            if (c.getUsername() == u) {
                return true;
            }
        }

        return false;
    }

    bool paidFor(int id, const string& period) const {
        for (const auto& p : payments) {
            if (p.staffID == id && p.period == period) {
                return true;
            }
        }

        return false;
    }

    int presentDays(int id, const string& period) const {
        int count = 0;

        for (const auto& a : attendance) {
            if (a.staffID == id &&
                a.present &&
                a.date.size() >= 7 &&
                a.date.substr(0, 7) == period) {

                ++count;
            }
        }

        return count;
    }

    int absentDays(int id, const string& period) const {
        int count = 0;

        for (const auto& a : attendance) {
            if (a.staffID == id &&
                !a.present &&
                a.date.size() >= 7 &&
                a.date.substr(0, 7) == period) {

                ++count;
            }
        }

        return count;
    }

    // ================= SAVE DATA =================

    void saveRooms() const {
        ofstream f(ROOMS_FILE);

        if (!f) {
            throw runtime_error("Cannot save rooms.");
        }

        for (const auto& r : rooms) {
            f << r.getNumber() << '|'
              << r.getType() << '|'
              << r.getPrice() << '\n';
        }
    }

    void saveFoods() const {
        ofstream f(FOODS_FILE);

        if (!f) {
            throw runtime_error("Cannot save foods.");
        }

        for (const auto& x : foods) {
            f << x.getName() << '|'
              << x.getPrice() << '\n';
        }
    }

    void saveCustomers() const {
        ofstream f(CUSTOMERS_FILE);

        if (!f) {
            throw runtime_error("Cannot save customers.");
        }

        for (const auto& c : customers) {
            f << c.getUsername() << '|'
              << c.getPasswordForStorage() << '|'
              << c.getAadhaarNumber() << '\n';
        }
    }

    void saveBookings() const {
        ofstream f(BOOKINGS_FILE);

        if (!f) {
            throw runtime_error("Cannot save bookings.");
        }

        for (const auto& r : rooms) {
            if (r.isBooked()) {
                f << r.getNumber() << '|'
                  << r.getOwner() << '|'
                  << r.getGuest() << '|'
                  << r.getDays();

                for (size_t i = 0; i < foods.size(); ++i) {
                    f << '|' << r.getFoodQty(i);
                }

                f << '\n';
            }
        }
    }

    void saveStaff() const {
        ofstream f(STAFF_FILE);

        if (!f) {
            throw runtime_error("Cannot save staff.");
        }

        for (const auto& s : staff) {
            f << s.getID() << '|'
              << s.getName() << '|'
              << s.getRole() << '|'
              << s.getDailyWage() << '\n';
        }
    }

    void saveAttendance() const {
        ofstream f(ATTENDANCE_FILE);

        if (!f) {
            throw runtime_error("Cannot save attendance.");
        }

        for (const auto& a : attendance) {
            f << a.staffID << '|'
              << a.date << '|'
              << a.present << '\n';
        }
    }

    void savePayments() const {
        ofstream f(PAYMENTS_FILE);

        if (!f) {
            throw runtime_error("Cannot save payments.");
        }

        for (const auto& p : payments) {
            f << p.staffID << '|'
              << p.period << '|'
              << p.amount << '|'
              << p.date << '\n';
        }
    }

    // ================= LOAD DATA =================

    void loadData() {
        string line;

        ifstream rf(ROOMS_FILE);

        while (getline(rf, line)) {
            auto v = split(line);

            if (v.size() >= 3) {
                try {
                    int n = stoi(v[0]);
                    double p = stod(v[2]);

                    if (findRoom(n) < 0) {
                        rooms.emplace_back(n, v[1], p);
                    }
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }

        if (rooms.empty()) {
            rooms.emplace_back(101, "Non-AC", 1000);
            rooms.emplace_back(102, "AC", 1500);
            rooms.emplace_back(103, "Non-AC", 1200);
            rooms.emplace_back(104, "AC", 2000);
            rooms.emplace_back(105, "AC", 2500);

            saveRooms();
        }

        ifstream ff(FOODS_FILE);

        while (getline(ff, line)) {
            auto v = split(line);

            if (v.size() >= 2) {
                try {
                    foods.emplace_back(v[0], stod(v[1]));
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }

        if (foods.empty()) {
            foods.emplace_back("Pizza", 200);
            foods.emplace_back("Burger", 100);
            foods.emplace_back("Sandwich", 80);
            foods.emplace_back("Biryani", 180);
            foods.emplace_back("Coffee", 50);

            saveFoods();
        }

        ifstream cf(CUSTOMERS_FILE);

        while (getline(cf, line)) {
            auto v = split(line);

            // Supports older customer records without Aadhaar.
            if (v.size() >= 2 &&
                v[0] != "admin" &&
                !userExists(v[0])) {

                customers.emplace_back(
                    v[0],
                    v[1],
                    v.size() >= 3 ? v[2] : ""
                );
            }
        }

        ifstream sf(STAFF_FILE);

        while (getline(sf, line)) {
            auto v = split(line);

            if (v.size() >= 4) {
                try {
                    staff.emplace_back(
                        stoi(v[0]),
                        v[1],
                        v[2],
                        stod(v[3])
                    );
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }

        ifstream af(ATTENDANCE_FILE);

        while (getline(af, line)) {
            auto v = split(line);

            if (v.size() >= 3) {
                try {
                    attendance.push_back({
                        stoi(v[0]),
                        v[1],
                        stoi(v[2]) != 0
                    });
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }

        ifstream pf(PAYMENTS_FILE);

        while (getline(pf, line)) {
            auto v = split(line);

            if (v.size() >= 4) {
                try {
                    payments.push_back({
                        stoi(v[0]),
                        v[1],
                        stod(v[2]),
                        v[3]
                    });
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }

        ifstream bf(BOOKINGS_FILE);

        while (getline(bf, line)) {
            auto v = split(line);

            if (v.size() >= 4) {
                try {
                    int idx = findRoom(stoi(v[0]));

                    if (idx >= 0) {
                        vector<int> q;

                        for (size_t i = 4; i < v.size(); ++i) {
                            q.push_back(stoi(v[i]));
                        }

                        rooms[idx].restoreBooking(
                            true,
                            v[1],
                            v[2],
                            stoi(v[3]),
                            q
                        );
                    }
                } catch (...) {
                    // Ignore invalid records.
                }
            }
        }
    }

    // ================= ROOM DISPLAY =================

    void showRooms(bool availableOnly) const {
        cout << "\n"
             << left
             << setw(10) << "Room No."
             << setw(16) << "Type"
             << setw(14) << "Price/Day"
             << "Status\n";

        bool found = false;

        for (const auto& r : rooms) {
            if (!availableOnly || !r.isBooked()) {
                cout << r << '\n';
                found = true;
            }
        }

        if (!found) {
            cout << "No matching rooms found.\n";
        }

        cout << "Room objects currently stored: "
             << Room::getObjectCount() << '\n';
    }

    // ================= BOOKING DISPLAY =================

    void showBookings(bool admin, const string& username) const {
        bool found = false;

        cout << "\n========== ACTIVE BOOKINGS ==========\n";

        for (const auto& r : rooms) {
            if (r.isBooked() &&
                (admin || r.getOwner() == username)) {

                found = true;

                cout << "Room: " << r.getNumber()
                     << " | Guest: " << r.getGuest()
                     << " | User: " << r.getOwner()
                     << " | Days: " << r.getDays()
                     << " | Room cost: Rs. "
                     << r.getPrice() * r.getDays()
                     << '\n';
            }
        }

        if (!found) {
            cout << "No matching active bookings.\n";
        }
    }

    // ================= ADD ROOM =================

    void addRoom() {
        int n = readInt("Enter new room number: ");

        if (n <= 0 || findRoom(n) >= 0) {
            throw invalid_argument(
                "Invalid or duplicate room number."
            );
        }

        string type = cleanField(
            readLine("Enter room type (AC/Non-AC/Deluxe etc.): ")
        );

        if (type.empty()) {
            throw invalid_argument("Room type cannot be empty.");
        }

        double price = readDouble("Enter price per day: Rs. ");

        rooms.emplace_back(n, type, price);

        saveRooms();

        cout << "Room added successfully.\n";
    }

    // ================= ADD FOOD =================

    void addFood() {
        string name = cleanField(
            readLine("Enter food item name: ")
        );

        if (name.empty()) {
            throw invalid_argument("Food name cannot be empty.");
        }

        double price = readDouble("Enter food item price: Rs. ");

        foods.emplace_back(name, price);

        saveFoods();

        cout << "Food item added successfully.\n";
    }

    void showFood() const {
        cout << "\n========== FOOD MENU ==========\n";

        for (size_t i = 0; i < foods.size(); ++i) {
            cout << i + 1 << ". "
                 << foods[i].getName()
                 << " - Rs. "
                 << foods[i].getPrice()
                 << '\n';
        }

        if (foods.empty()) {
            cout << "No food items available.\n";
        }
    }

    // ================= CHANGE PRICES =================

    void changePrices() {
        cout << "1. Change room price\n";
        cout << "2. Change food price\n";

        int c = readInt("Enter choice: ");

        if (c == 1) {
            showRooms(false);

            int n = readInt("Enter room number: ");
            int i = findRoom(n);

            if (i < 0) {
                throw runtime_error("Room not found.");
            }

            rooms[i].setPrice(
                readDouble("Enter new daily price: Rs. ")
            );

            saveRooms();

            cout << "Room price updated.\n";
        }
        else if (c == 2) {
            showFood();

            int n = readInt("Enter food item number: ");

            if (n < 1 || n > static_cast<int>(foods.size())) {
                throw runtime_error("Food item not found.");
            }

            foods[n - 1].setPrice(
                readDouble("Enter new food price: Rs. ")
            );

            saveFoods();

            cout << "Food price updated.\n";
        }
        else {
            throw invalid_argument("Invalid choice.");
        }
    }

    // ================= BOOK A ROOM =================

    void bookRoom(const string& user) {
        showRooms(true);

        int n = readInt("Enter room number to book: ");
        int i = findRoom(n);

        if (i < 0) {
            throw runtime_error("Room not found or unavailable.");
        }

        int days = readInt("Enter number of days: ");

        string guest = cleanField(
            readLine("Enter guest name: ")
        );

        rooms[i].book(user, guest, days);

        saveBookings();

        cout << "Room booked successfully. Room cost: Rs. "
             << rooms[i].getPrice() * days
             << '\n';
    }

    // ================= ORDER FOOD =================

    void orderFood(const string& user, bool admin) {
        int n = readInt("Enter room number: ");
        int i = findRoom(n);

        if (i < 0 || !rooms[i].isBooked()) {
            throw runtime_error("Active booking not found.");
        }

        if (!admin && rooms[i].getOwner() != user) {
            throw runtime_error(
                "You can order food only for your own room."
            );
        }

        showFood();

        if (foods.empty()) {
            return;
        }

        int f = readInt("Enter food item number: ");

        if (f < 1 || f > static_cast<int>(foods.size())) {
            throw invalid_argument("Invalid food item.");
        }

        int q = readInt("Enter quantity: ");

        rooms[i].addFood(f - 1, q);

        saveBookings();

        cout << "Food ordered. Cost: Rs. "
             << foods[f - 1].getPrice() * q
             << '\n';
    }

    // ================= CHECKOUT AND BILL =================

    void checkout(const string& user, bool admin) {
        int n = readInt("Enter room number: ");
        int i = findRoom(n);

        if (i < 0 || !rooms[i].isBooked()) {
            throw runtime_error("Active booking not found.");
        }

        Room& r = rooms[i];

        if (!admin && r.getOwner() != user) {
            throw runtime_error(
                "You can checkout only your own room."
            );
        }

        double roomCost = r.getPrice() * r.getDays();
        double foodCost = 0;

        cout << "\n========== HOTEL BILL ==========\n";
        cout << "Guest: " << r.getGuest() << '\n';
        cout << "Room: " << r.getNumber() << '\n';
        cout << "Days: " << r.getDays() << '\n';
        cout << "Room charges: Rs. " << roomCost << '\n';

        for (size_t j = 0; j < foods.size(); ++j) {
            if (r.getFoodQty(j) > 0) {
                double cost =
                    foods[j].getPrice() * r.getFoodQty(j);

                foodCost += cost;

                cout << foods[j].getName()
                     << " x " << r.getFoodQty(j)
                     << " = Rs. " << cost << '\n';
            }
        }

        cout << "Food charges: Rs. " << foodCost << '\n';
        cout << "TOTAL BILL: Rs. "
             << roomCost + foodCost << '\n';

        int yes = readInt(
            "Confirm checkout? (1=Yes, 0=No): "
        );

        if (yes != 1) {
            cout << "Checkout cancelled.\n";
            return;
        }

        roomRevenue += roomCost;
        foodRevenue += foodCost;

        r.release();

        saveBookings();

        cout << "Checkout completed successfully.\n";
    }

    // ================= REVENUE REPORT =================

    void revenueReport() const {
        cout << "\n========== REVENUE REPORT ==========\n";
        cout << "Room revenue this run: Rs. "
             << roomRevenue << '\n';
        cout << "Food revenue this run: Rs. "
             << foodRevenue << '\n';
        cout << "Total revenue this run: Rs. "
             << roomRevenue + foodRevenue << '\n';
        cout << "Note: revenue totals reset when the program restarts.\n";
    }

    // ================= STAFF MANAGEMENT =================

    void staffMenu() {
        while (true) {
            cout << "\n========== STAFF MANAGEMENT ==========\n"
                 << "1. Add new staff\n"
                 << "2. View all staff\n"
                 << "3. Total workers\n"
                 << "4. Mark attendance\n"
                 << "5. View attendance for a month\n"
                 << "6. Calculate salary\n"
                 << "7. Pay salary\n"
                 << "8. View payment status\n"
                 << "9. Return\n";

            int c = readInt("Enter choice: ");

            try {
                if (c == 1) {
                    string name = cleanField(
                        readLine("Enter staff name: ")
                    );

                    string role = cleanField(
                        readLine("Enter job role: ")
                    );

                    double wage = readDouble(
                        "Enter daily wage: Rs. "
                    );

                    int id = 101;

                    for (const auto& s : staff) {
                        if (s.getID() >= id) {
                            id = s.getID() + 1;
                        }
                    }

                    staff.emplace_back(id, name, role, wage);

                    saveStaff();

                    cout << "Staff added successfully. Staff ID: "
                         << id << '\n';
                }
                else if (c == 2) {
                    cout << left
                         << setw(10) << "ID"
                         << setw(24) << "Name"
                         << setw(20) << "Role"
                         << "Daily Wage\n";

                    for (const auto& s : staff) {
                        cout << left
                             << setw(10) << s.getID()
                             << setw(24) << s.getName()
                             << setw(20) << s.getRole()
                             << s.getDailyWage()
                             << '\n';
                    }

                    if (staff.empty()) {
                        cout << "No staff registered.\n";
                    }
                }
                else if (c == 3) {
                    cout << "Total workers registered: "
                         << staff.size() << '\n';
                }
                else if (c == 4) {
                    int id = readInt("Enter staff ID: ");
                    int i = findStaff(id);

                    if (i < 0) {
                        throw runtime_error(
                            "Staff member not found."
                        );
                    }

                    string date = readLine(
                        "Enter date (YYYY-MM-DD): "
                    );

                    if (date.size() != 10 ||
                        date[4] != '-' ||
                        date[7] != '-') {

                        throw invalid_argument(
                            "Use date format YYYY-MM-DD."
                        );
                    }

                    for (const auto& a : attendance) {
                        if (a.staffID == id && a.date == date) {
                            throw runtime_error(
                                "Attendance already recorded for this date."
                            );
                        }
                    }

                    string status = readLine(
                        "Present or absent? (P/A): "
                    );

                    if (status != "P" && status != "p" &&
                        status != "A" && status != "a") {

                        throw invalid_argument("Enter P or A.");
                    }

                    bool present =
                        status == "P" || status == "p";

                    attendance.push_back({id, date, present});

                    saveAttendance();

                    cout << "Attendance recorded for "
                         << date << ".\n";
                }
                else if (c == 5) {
                    string period = readLine(
                        "Enter month (YYYY-MM): "
                    );

                    if (period.size() != 7 ||
                        period[4] != '-') {

                        throw invalid_argument(
                            "Use month format YYYY-MM."
                        );
                    }

                    cout << left
                         << setw(10) << "ID"
                         << setw(24) << "Name"
                         << setw(12) << "Present"
                         << "Absent\n";

                    for (const auto& s : staff) {
                        cout << left
                             << setw(10) << s.getID()
                             << setw(24) << s.getName()
                             << setw(12)
                             << presentDays(s.getID(), period)
                             << absentDays(s.getID(), period)
                             << '\n';
                    }
                }
                else if (c == 6) {
                    int id = readInt("Enter staff ID: ");
                    int i = findStaff(id);

                    if (i < 0) {
                        throw runtime_error(
                            "Staff member not found."
                        );
                    }

                    string period = readLine(
                        "Enter salary month (YYYY-MM): "
                    );

                    if (period.size() != 7 ||
                        period[4] != '-') {

                        throw invalid_argument(
                            "Use month format YYYY-MM."
                        );
                    }

                    int present = presentDays(id, period);
                    int absent = absentDays(id, period);

                    double salary =
                        present * staff[i].getDailyWage();

                    cout << "Name: " << staff[i].getName() << '\n';
                    cout << "Month: " << period << '\n';
                    cout << "Present days: " << present << '\n';
                    cout << "Absent days: " << absent << '\n';
                    cout << "Salary: Rs. " << salary << '\n';
                    cout << "Payment status: "
                         << (paidFor(id, period)
                                 ? "Paid"
                                 : "Not Paid")
                         << '\n';
                }
                else if (c == 7) {
                    int id = readInt("Enter staff ID: ");
                    int i = findStaff(id);

                    if (i < 0) {
                        throw runtime_error(
                            "Staff member not found."
                        );
                    }

                    string period = readLine(
                        "Enter salary month (YYYY-MM): "
                    );

                    if (period.size() != 7 ||
                        period[4] != '-') {

                        throw invalid_argument(
                            "Use month format YYYY-MM."
                        );
                    }

                    if (paidFor(id, period)) {
                        throw runtime_error(
                            "Salary already recorded as paid for this month."
                        );
                    }

                    int present = presentDays(id, period);

                    double amount =
                        present * staff[i].getDailyWage();

                    if (present == 0) {
                        throw runtime_error(
                            "No present days recorded for this month."
                        );
                    }

                    cout << "Staff: " << staff[i].getName()
                         << " | Month: " << period
                         << " | Amount: Rs. " << amount
                         << '\n';

                    string confirm = readLine(
                        "Confirm payment? (Y/N): "
                    );

                    if (confirm == "Y" || confirm == "y") {
                        string date = readLine(
                            "Enter payment date (YYYY-MM-DD): "
                        );

                        if (date.size() != 10 ||
                            date[4] != '-' ||
                            date[7] != '-') {

                            throw invalid_argument(
                                "Use date format YYYY-MM-DD."
                            );
                        }

                        payments.push_back({
                            id,
                            period,
                            amount,
                            date
                        });

                        savePayments();

                        cout << "Payment recorded successfully.\n";
                    }
                    else {
                        cout << "Payment cancelled.\n";
                    }
                }
                else if (c == 8) {
                    string period = readLine(
                        "Enter month (YYYY-MM): "
                    );

                    if (period.size() != 7 ||
                        period[4] != '-') {

                        throw invalid_argument(
                            "Use month format YYYY-MM."
                        );
                    }

                    for (const auto& s : staff) {
                        int present =
                            presentDays(s.getID(), period);

                        double due =
                            present * s.getDailyWage();

                        cout << "ID: " << s.getID()
                             << " | " << s.getName()
                             << " | Salary: Rs. " << due
                             << " | Status: "
                             << (paidFor(s.getID(), period)
                                     ? "Paid"
                                     : "Not Paid")
                             << '\n';
                    }
                }
                else if (c == 9) {
                    return;
                }
                else {
                    cout << "Invalid choice.\n";
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }

    // ================= ADMIN MENU =================

    void adminLoop() {
        Admin admin;

        while (true) {
            admin.dashboard();

            int c = readInt("Enter choice: ");

            try {
                switch (c) {
                    case 1:
                        showRooms(false);
                        break;

                    case 2:
                        showRooms(true);
                        break;

                    case 3:
                        showBookings(true, "");
                        break;

                    case 4:
                        addRoom();
                        break;

                    case 5:
                        addFood();
                        break;

                    case 6:
                        changePrices();
                        break;

                    case 7:
                        orderFood("", true);
                        break;

                    case 8:
                        checkout("", true);
                        break;

                    case 9:
                        revenueReport();
                        break;

                    case 10:
                        staffMenu();
                        break;

                    case 11:
                        return;

                    default:
                        cout << "Invalid choice.\n";
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }

    // ================= CUSTOMER MENU =================

    void customerLoop(Customer& c) {
        while (true) {
            c.dashboard();

            int choice = readInt("Enter choice: ");

            try {
                switch (choice) {
                    case 1:
                        showRooms(false);
                        break;

                    case 2:
                        showRooms(true);
                        break;

                    case 3:
                        bookRoom(c.getUsername());
                        break;

                    case 4:
                        showBookings(false, c.getUsername());
                        break;

                    case 5:
                        orderFood(c.getUsername(), false);
                        break;

                    case 6:
                        checkout(c.getUsername(), false);
                        break;

                    case 7:
                        return;

                    default:
                        cout << "Invalid choice.\n";
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }

public:
    Hotel() {
        loadData();
    }

    // ================= CUSTOMER REGISTRATION =================

    void registerCustomer() {
        string u = readLine("Enter username: ");

        if (u == "admin" || userExists(u)) {
            throw runtime_error(
                "Username reserved or already exists."
            );
        }

        string aadhaar = readLine(
            "Enter Aadhaar number (12 digits): "
        );

        bool validAadhaar =
            aadhaar.size() == 12 &&
            all_of(
                aadhaar.begin(),
                aadhaar.end(),
                [](unsigned char ch) {
                    return ch >= '0' && ch <= '9';
                }
            );

        if (!validAadhaar) {
            throw invalid_argument(
                "Aadhaar number must contain exactly 12 digits."
            );
        }

        string p = readLine(
            "Enter password (minimum 6 characters): "
        );

        if (p.size() < 6) {
            throw invalid_argument(
                "Password must contain at least 6 characters."
            );
        }

        customers.emplace_back(u, p, aadhaar);

        saveCustomers();

        cout << "Registration successful.\n";
    }

    // ================= CUSTOMER LOGIN =================

    void customerLogin() {
        string u = readLine("Enter username: ");
        string p = readLine("Enter password: ");

        for (auto& c : customers) {
            if (c.getUsername() == u &&
                c.checkPassword(p)) {

                cout << "Login successful.\n";
                customerLoop(c);
                return;
            }
        }

        cout << "Invalid username or password.\n";
    }

    // ================= ADMIN LOGIN =================

    void adminLogin() {
        Admin a;

        string u = readLine("Enter admin username: ");
        string p = readLine("Enter admin password: ");

        if (u == a.getUsername() &&
            a.checkPassword(p)) {

            cout << "Admin login successful.\n";
            adminLoop();
        }
        else {
            cout << "Invalid admin credentials.\n";
        }
    }
};

// ================= MAIN FUNCTION =================

int main() {
    try {
        Hotel hotel;

        while (true) {
            cout << "\n====================================\n"
                 << "       HOTEL MANAGEMENT SYSTEM\n"
                 << "====================================\n"
                 << "1. Customer registration\n"
                 << "2. Customer login\n"
                 << "3. Admin login\n"
                 << "4. Exit\n";

            int c = readInt("Enter choice: ");

            try {
                if (c == 1) {
                    hotel.registerCustomer();
                }
                else if (c == 2) {
                    hotel.customerLogin();
                }
                else if (c == 3) {
                    hotel.adminLogin();
                }
                else if (c == 4) {
                    cout << "Thank you for using Hotel Management System!\n";
                    break;
                }
                else {
                    cout << "Invalid choice.\n";
                }
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }
    catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}