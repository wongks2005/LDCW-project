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
    // Add these methods inside your Database class

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
};

// --- Application Logic ---
class FoodApp {
private:
    string currentUser;
    vector<MenuItem> menu;
    vector<CartItem> cart;

public:
    // Methods will be implemented in Stages 3-5
};

int main() {
    return 0;
}