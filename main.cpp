#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace std;

// --- Data Structures ---
struct MenuItem {
    int id;
    string name;
    double price;
};

struct CartItem {
    MenuItem item;
    int quantity;
};

// --- Database Simulation (File Handling) ---
class Database {
public:

static void initializeMenu() {
    ifstream file("menu.txt");
    if (!file.is_open()) {
        ofstream newFile("menu.txt");
        newFile << "1,Cheeseburger,5.99\n";
        newFile << "2,Pizza Margherita,8.50\n";
        newFile << "3,Caesar Salad,4.75\n";
        newFile << "4,French Fries,2.99\n";
        newFile << "5,Soda,1.50\n";
        newFile.close();
    }
}

static vector<MenuItem> loadMenu() {
    vector<MenuItem> menu;
    ifstream file("menu.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, priceStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        menu.push_back({stoi(idStr), name, stod(priceStr)});
    }
    return menu;
}

static bool registerUser(const string& username, const string& password) {
    ifstream inFile("users.txt");
    string u, p;
    while (inFile >> u >> p) {
        if (u == username) return false; 
    }
    inFile.close();

    ofstream outFile("users.txt", ios::app);
    outFile << username << " " << password << "\n";
    return true;
}

static bool loginUser(const string& username, const string& password) {
    ifstream file("users.txt");
    string u, p;
    while (file >> u >> p) {
        if (u == username && p == password) return true;
    }
    return false;
}
// 1. Add these methods inside your Database class
static void saveOrder(const string& username, double total) {
    ofstream file("orders.txt", ios::app);
    file << username << "," << total << "\n";
}

static void viewOrderHistory(const string& username) {
    ifstream file("orders.txt");
    string line, user, total;
    bool found = false;
    cout << "\n--- Order History for " << username << " ---\n";
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, user, ',');
        getline(ss, total, ',');
        if (user == username) {
            cout << "Order Total: $" << total << "\n";
            found = true;
        }
    }
    if (!found) cout << "No previous orders found.\n";
}

// 2. Add this method inside the private section of your FoodApp class
void checkout() {
    if (cart.empty()) {
        cout << "Cart is empty. Cannot checkout.\n";
        return;
    }
    viewCart();
    cout << "Confirm order? (y/n): ";
    char confirm;
    cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        double total = 0;
        for (const auto& c : cart) total += c.item.price * c.quantity;
        Database::saveOrder(currentUser, total);
        cart.clear();
        cout << "Order placed successfully! Thank you.\n";
    } else {
        cout << "Checkout cancelled.\n";
    }
}
};

// --- Application Logic ---
class FoodApp {
private:
    string currentUser;
    vector<MenuItem> menu;
    vector<CartItem> cart;

void displayMenu() {
    cout << "\n--- Menu ---\n";
    for (const auto& item : menu) {
        cout << item.id << ". " << left << setw(20) << item.name 
             << "$" << fixed << setprecision(2) << item.price << "\n";
    }
}

void addToCart() {
    displayMenu();
    int id, qty;
    cout << "Enter Item ID to add: ";
    cin >> id;
    cout << "Enter Quantity: ";
    cin >> qty;

    for (const auto& item : menu) {
        if (item.id == id) {
            cart.push_back({item, qty});
            cout << qty << "x " << item.name << " added to cart.\n";
            return;
        }
    }
    cout << "Invalid Item ID.\n";
}

void viewCart() {
    if (cart.empty()) {
        cout << "\nYour cart is empty.\n";
        return;
    }
    cout << "\n--- Your Cart ---\n";
    double total = 0;
    for (const auto& cItem : cart) {
        double cost = cItem.item.price * cItem.quantity;
        total += cost;
        cout << cItem.quantity << "x " << left << setw(18) << cItem.item.name 
             << "$" << fixed << setprecision(2) << cost << "\n";
    }
    cout << "--------------------------\n";
    cout << "Total: $" << total << "\n";
}
public:
    

};

int main() {
    return 0;
}