/**
 * Food Delivery Order System
 * Inspired by Foodpanda - Food Delivery Platform
 * 
 * Part of LDCW6123 Group Project
 * 
 * Combined version with:
 * - User authentication (from teammates)
 * - Restaurant selection & ordering (from you)
 * - Shopping cart & checkout
 * - Order history
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <cctype>

using namespace std;

// ============ DATA STRUCTURES ============

struct MenuItem {
    int id;
    string name;
    double price;
};

struct CartItem {
    MenuItem item;
    int quantity;
};

struct Restaurant {
    int id;
    string name;
    string cuisine;
    vector<MenuItem> menu;
};

// ============ DATABASE CLASS ============

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

    static void saveOrder(const string& username, double total, const vector<CartItem>& currentCart) {
        ofstream file("orders.txt", ios::app);
        file << username << "," << total << ",";
        for (size_t i = 0; i < currentCart.size(); ++i) {
            file << currentCart[i].item.name << ":" << currentCart[i].quantity;
            if (i < currentCart.size() - 1) file << "|";
        }
        file << "\n";
    }

    static void viewOrderHistory(const string& username) {
        ifstream file("orders.txt");
        string line, user, total, itemsStr;
        bool found = false;

        cout << "\n===========================================" << endl;
        cout << "     ORDER HISTORY FOR " << username << endl;
        cout << "===========================================" << endl;

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, user, ',');
            getline(ss, total, ',');
            getline(ss, itemsStr, ',');

            if (user == username) {
                cout << "Total: RM " << total << endl;
                cout << "Items Ordered:" << endl;

                stringstream ssItems(itemsStr);
                string singleItem;
                while (getline(ssItems, singleItem, '|')) {
                    size_t colonPos = singleItem.find(':');
                    if (colonPos != string::npos) {
                        string name = singleItem.substr(0, colonPos);
                        string qty = singleItem.substr(colonPos + 1);
                        cout << "  - " << qty << "x " << name << endl;
                    }
                }
                cout << "-------------------------------------------" << endl;
                found = true;
            }
        }
        if (!found) cout << "No previous orders found." << endl;
    }
};

// ============ INPUT VALIDATION ============

int getValidatedInt(string prompt, int minVal, int maxVal) {
    int input;
    bool valid = false;

    do {
        cout << prompt;
        cin >> input;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Please enter a valid NUMBER between " << minVal << " and " << maxVal << "!" << endl;
        }
        else if (input < minVal || input > maxVal) {
            cout << "[ERROR] Number must be between " << minVal << " and " << maxVal << "!" << endl;
        }
        else {
            valid = true;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (!valid);

    return input;
}

char getValidatedChar(string prompt) {
    char input;
    bool valid = false;

    do {
        cout << prompt;
        cin >> input;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Please enter a SINGLE character!" << endl;
        }
        else {
            input = toupper(input);
            if (input != 'Y' && input != 'N') {
                cout << "[ERROR] Please enter 'Y' for Yes or 'N' for No!" << endl;
            }
            else {
                valid = true;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

    } while (!valid);

    return input;
}

// ============ FOOD DELIVERY SYSTEM (Your Code) ============

class FoodDeliverySystem {
private:
    vector<Restaurant> restaurants;
    vector<CartItem> cart;
    string currentUser;

    void initializeRestaurants() {
        restaurants = {
            {1, "McDonald's", "Fast Food", {
                {1, "Big Mac", 18.50},
                {2, "McChicken", 12.00},
                {3, "Fries", 9.00},
                {4, "McFlurry", 8.50},
                {5, "Coca-Cola", 5.00}
            }},
            {2, "KFC", "Fried Chicken", {
                {1, "Original Recipe Chicken", 22.00},
                {2, "Zinger Burger", 15.00},
                {3, "Coleslaw", 6.00},
                {4, "Mashed Potatoes", 7.00},
                {5, "Pepsi", 5.00}
            }},
            {3, "Pizza Hut", "Pizzas", {
                {1, "Margherita Pizza", 25.00},
                {2, "Pepperoni Pizza", 28.00},
                {3, "Hawaiian Pizza", 27.00},
                {4, "Garlic Bread", 10.00},
                {5, "Soft Drink", 5.00}
            }},
            {4, "Starbucks", "Coffee & Drinks", {
                {1, "Caramel Latte", 16.00},
                {2, "Cappuccino", 14.00},
                {3, "Green Tea", 12.00},
                {4, "Croissant", 8.00},
                {5, "Muffin", 7.00}
            }},
            {5, "Local Noodle House", "Asian Cuisine", {
                {1, "Chicken Noodle Soup", 15.00},
                {2, "Beef Noodle", 18.00},
                {3, "Fried Rice", 13.00},
                {4, "Spring Rolls", 10.00},
                {5, "Thai Tea", 6.00}
            }}
        };
    }

    void displayRestaurants() {
        cout << "\n===========================================" << endl;
        cout << "        AVAILABLE RESTAURANTS              " << endl;
        cout << "===========================================" << endl;
        for (const auto& r : restaurants) {
            cout << r.id << ". " << r.name << " - " << r.cuisine << endl;
        }
        cout << "===========================================" << endl;
    }

    void displayMenu(const Restaurant& restaurant) {
        cout << "\n=== " << restaurant.name << " MENU ===" << endl;
        for (const auto& item : restaurant.menu) {
            cout << item.id << ". " << left << setw(20) << item.name
                 << "RM " << fixed << setprecision(2) << item.price << endl;
        }
        cout << "=============================" << endl;
    }

    void addToCart() {
        displayRestaurants();
        int restChoice = getValidatedInt("Select a restaurant (1-5): ", 1, 5);

        Restaurant& selected = restaurants[restChoice - 1];
        displayMenu(selected);

        int itemChoice = getValidatedInt("Select item (0 to cancel): ", 0, 5);
        if (itemChoice == 0) return;

        int quantity = getValidatedInt("Enter quantity (1-99): ", 1, 99);

        for (const auto& item : selected.menu) {
            if (item.id == itemChoice) {
                cart.push_back({item, quantity});
                cout << "Added " << quantity << "x " << item.name << " to cart!" << endl;
                return;
            }
        }
    }

    void viewCart() {
        if (cart.empty()) {
            cout << "\n[INFO] Your cart is empty." << endl;
            return;
        }

        cout << "\n===========================================" << endl;
        cout << "              YOUR CART                    " << endl;
        cout << "===========================================" << endl;
        double total = 0;
        for (const auto& cItem : cart) {
            double cost = cItem.item.price * cItem.quantity;
            total += cost;
            cout << cItem.quantity << "x " << left << setw(18) << cItem.item.name
                 << "RM " << fixed << setprecision(2) << cost << endl;
        }
        cout << "-------------------------------------------" << endl;
        cout << "TOTAL: RM " << fixed << setprecision(2) << total << endl;
        cout << "===========================================" << endl;
    }

    void checkout() {
        if (cart.empty()) {
            cout << "[ERROR] Cart is empty. Cannot checkout." << endl;
            return;
        }

        viewCart();
        char confirm = getValidatedChar("Confirm order? (Y/N): ");

        if (confirm == 'Y') {
            double total = 0;
            for (const auto& c : cart) total += c.item.price * c.quantity;

            Database::saveOrder(currentUser, total, cart);
            cart.clear();
            cout << "\n[SUCCESS] Order placed successfully! Thank you!" << endl;
        } else {
            cout << "Checkout cancelled." << endl;
        }
    }

    void cartPage() {
        int choice;
        do {
            viewCart();
            cout << "\n--- Cart Menu ---" << endl;
            cout << "1. Checkout" << endl;
            cout << "2. Clear Cart" << endl;
            cout << "3. Go Back" << endl;
            choice = getValidatedInt("Choice: ", 1, 3);

            switch (choice) {
                case 1:
                    checkout();
                    if (cart.empty()) choice = 3;
                    break;
                case 2:
                    cart.clear();
                    cout << "Cart cleared." << endl;
                    break;
                case 3:
                    break;
            }
        } while (choice != 3);
    }

public:
    FoodDeliverySystem(string user) : currentUser(user) {
        initializeRestaurants();
    }

    void showMenu() {
        int choice;
        do {
            cout << "\n===========================================" << endl;
            cout << "        FOOD DELIVERY SYSTEM              " << endl;
            cout << "===========================================" << endl;
            cout << "1. Browse Restaurants & Order" << endl;
            cout << "2. View Cart" << endl;
            cout << "3. View Order History" << endl;
            cout << "4. Logout" << endl;
            cout << "===========================================" << endl;

            choice = getValidatedInt("Choice: ", 1, 4);

            switch (choice) {
                case 1:
                    addToCart();
                    break;
                case 2:
                    cartPage();
                    break;
                case 3:
                    Database::viewOrderHistory(currentUser);
                    break;
                case 4:
                    cout << "Logging out..." << endl;
                    break;
            }
        } while (choice != 4);
    }
};

// ============ MAIN APPLICATION ============

class FoodApp {
private:
    string currentUser;

public:
    void run() {
        int choice;
        string user, pass;

        do {
            cout << "\n===========================================" << endl;
            cout << "          FOOD ORDERING APP                " << endl;
            cout << "         (Inspired by Foodpanda)          " << endl;
            cout << "===========================================" << endl;
            cout << "1. Login" << endl;
            cout << "2. Register" << endl;
            cout << "3. Exit" << endl;
            cout << "===========================================" << endl;

            choice = getValidatedInt("Choice: ", 1, 3);

            switch (choice) {
                case 1:
                    cout << "Username: ";
                    cin >> user;
                    cout << "Password: ";
                    cin >> pass;

                    if (Database::loginUser(user, pass)) {
                        currentUser = user;
                        cout << "\n[SUCCESS] Welcome back, " << currentUser << "!" << endl;
                        FoodDeliverySystem delivery(currentUser);
                        delivery.showMenu();
                    } else {
                        cout << "[ERROR] Invalid credentials." << endl;
                    }
                    break;

                case 2:
                    cout << "New Username: ";
                    cin >> user;
                    cout << "New Password: ";
                    cin >> pass;

                    if (Database::registerUser(user, pass)) {
                        cout << "[SUCCESS] Registration successful! You can now log in." << endl;
                    } else {
                        cout << "[ERROR] Username already exists." << endl;
                    }
                    break;

                case 3:
                    cout << "\nThank you for using Food Ordering App!" << endl;
                    cout << "Your order will be delivered soon! 🚀" << endl;
                    break;
            }
        } while (choice != 3);
    }
};

// ============ MAIN ============

int main() {
    // Initialize database files
    Database::initializeMenu();

    // Run the app
    FoodApp app;
    app.run();

    return 0;
}