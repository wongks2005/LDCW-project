/**
 * Food Delivery Order System
 * Inspired by Foodpanda - Food Delivery Platform
 * This code was made for LDCW6123 Group Project
 * 
 * This is the finalized version with several features:
 * - Persistent Flat-File Database Initialization: Automatically sets up a local relational CSV file populated with sample restaurants and structured menu items if it does not already exist[cite: 7, 8].
 * - Dynamic Restaurant Data Parsing: Loads restaurant menus line-by-line using string streams to assemble complex runtime data vectors dynamically[cite: 17, 18, 22].
 * - User Access Control: Manages credential authentication through dedicated file-backed registration and matching login verification routines[cite: 24, 26, 28].
 * - Transaction Log and History Tracker: Formats and stores transactional order logs onto external storage, allowing users to recall their individual order histories at any time[cite: 31, 34, 37].
 * - Fail-Safe Input Validation: Intercepts input stream failures and evaluates logical boundary limits for numeric choices and character choices to eliminate terminal crashes[cite: 46, 48, 52, 55].
 * - Interactive Menu Browsing and Shopping Cart: Empowers users to browse various cuisines, customize distinct item quantities, compute item totals, and clear or checkout items smoothly[cite: 63, 71, 78, 80, 95].
 * - Structured Console Interface: Organizes user navigation via layered menus that dynamically refresh using dedicated ANSI screen-clearing utilities[cite: 4, 109, 112].
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

// ============ INTERFACE FLOW HELPERS ============

// Clears the console screen using standard ANSI escape codes
void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

// Pauses the screen until the user presses Enter
void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

// ============ DATABASE CLASS ============

class Database {
public:
    // Generates a structured relational csv flat-file database
    static void initializeMenu() {
        ifstream file("menu.txt");
        if (!file.is_open()) {
            ofstream newFile("menu.txt");
            // Format: RestaurantID,RestaurantName,Cuisine,ItemID,ItemName,Price
            newFile << "1,McDonald's,Fast Food,1,Big Mac,18.50\n";
            newFile << "1,McDonald's,Fast Food,2,McChicken,12.00\n";
            newFile << "1,McDonald's,Fast Food,3,Fries,9.00\n";
            newFile << "1,McDonald's,Fast Food,4,McFlurry,8.50\n";
            newFile << "1,McDonald's,Fast Food,5,Coca-Cola,5.00\n";
            newFile << "2,KFC,Fried Chicken,1,Original Recipe Chicken,22.00\n";
            newFile << "2,KFC,Fried Chicken,2,Zinger Burger,15.00\n";
            newFile << "2,KFC,Fried Chicken,3,Coleslaw,6.00\n";
            newFile << "2,KFC,Fried Chicken,4,Mashed Potatoes,7.00\n";
            newFile << "2,KFC,Fried Chicken,5,Pepsi,5.00\n";
            newFile << "3,Pizza Hut,Pizzas,1,Margherita Pizza,25.00\n";
            newFile << "3,Pizza Hut,Pizzas,2,Pepperoni Pizza,28.00\n";
            newFile << "3,Pizza Hut,Pizzas,3,Hawaiian Pizza,27.00\n";
            newFile << "3,Pizza Hut,Pizzas,4,Garlic Bread,10.00\n";
            newFile << "3,Pizza Hut,Pizzas,5,Soft Drink,5.00\n";
            newFile << "4,Starbucks,Coffee & Drinks,1,Caramel Latte,16.00\n";
            newFile << "4,Starbucks,Coffee & Drinks,2,Cappuccino,14.00\n";
            newFile << "4,Starbucks,Coffee & Drinks,3,Green Tea,12.00\n";
            newFile << "4,Starbucks,Coffee & Drinks,4,Croissant,8.00\n";
            newFile << "4,Starbucks,Coffee & Drinks,5,Muffin,7.00\n";
            newFile << "5,Local Noodle House,Asian Cuisine,1,Chicken Noodle Soup,15.00\n";
            newFile << "5,Local Noodle House,Asian Cuisine,2,Beef Noodle,18.00\n";
            newFile << "5,Local Noodle House,Asian Cuisine,3,Fried Rice,13.00\n";
            newFile << "5,Local Noodle House,Asian Cuisine,4,Spring Rolls,10.00\n";
            newFile << "5,Local Noodle House,Asian Cuisine,5,Thai Tea,6.00\n";
            newFile.close();
        }
    }

    // Dynamically builds the objects by parsing the database text file line-by-line
    static vector<Restaurant> loadRestaurants() {
        vector<Restaurant> lists;
        ifstream file("menu.txt");
        string line;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string rIdStr, rName, cuisine, mIdStr, mName, priceStr;

            getline(ss, rIdStr, ',');
            getline(ss, rName, ',');
            getline(ss, cuisine, ',');
            getline(ss, mIdStr, ',');
            getline(ss, mName, ',');
            getline(ss, priceStr, ',');

            int rId = stoi(rIdStr);
            int mId = stoi(mIdStr);
            double price = stod(priceStr);

            bool found = false;
            for (auto& r : lists) {
                if (r.id == rId) {
                    r.menu.push_back({mId, mName, price});
                    found = true;
                    break;
                }
            }

            if (!found) {
                Restaurant newRest = {rId, rName, cuisine, {{mId, mName, price}}};
                lists.push_back(newRest);
            }
        }
        return lists;
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

        clearScreen();
        cout << "===========================================" << endl;
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
        pauseScreen();
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

// ============ FOOD DELIVERY SYSTEM ============

class FoodDeliverySystem {
private:
    vector<Restaurant> restaurants;
    vector<CartItem> cart;
    string currentUser;

    void initializeRestaurants() {
        restaurants = Database::loadRestaurants();
    }

    void displayRestaurants() {
        clearScreen();
        cout << "===========================================" << endl;
        cout << "        AVAILABLE RESTAURANTS              " << endl;
        cout << "===========================================" << endl;
        for (const auto& r : restaurants) {
            cout << r.id << ". " << r.name << " - " << r.cuisine << endl;
        }
        cout << "===========================================" << endl;
    }

    void displayMenu(const Restaurant& restaurant) {
        clearScreen();
        cout << "=== " << restaurant.name << " MENU ===" << endl;
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
                cout << "\nAdded " << quantity << "x " << item.name << " to cart!" << endl;
                pauseScreen();
                return;
            }
        }
    }

    void viewCart() {
        clearScreen();
        if (cart.empty()) {
            cout << "[INFO] Your cart is empty." << endl;
            return;
        }

        cout << "===========================================" << endl;
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
        viewCart();
        if (cart.empty()) {
            pauseScreen();
            return;
        }

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
        pauseScreen();
    }

public:
    FoodDeliverySystem(string user) : currentUser(user) {
        initializeRestaurants();
    }

void cartPage() {
        viewCart();
        if (cart.empty()) {
            pauseScreen();
            return;
        }

        cout << "\n--- Cart Menu ---" << endl;
        cout << "1. Checkout" << endl;
        cout << "2. Clear Cart" << endl;
        cout << "3. Go Back" << endl;
        int choice = getValidatedInt("Choice: ", 1, 3);

        switch (choice) {
            case 1:
                checkout();
                break;
            case 2:
                cart.clear();
                cout << "Cart cleared." << endl;
                pauseScreen();
                break;
            case 3:
                break;
        }
    }

    void showMenu() {
        int choice;
        do {
            clearScreen();
            cout << "===========================================" << endl;
            cout << "        FOOD DELIVERY SYSTEM               " << endl;
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
                    pauseScreen();
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
            clearScreen();
            cout << "===========================================" << endl;
            cout << "          FOOD ORDERING APP                " << endl;
            cout << "===========================================" << endl;
            cout << "1. Login" << endl;
            cout << "2. Register" << endl;
            cout << "3. Exit" << endl;
            cout << "===========================================" << endl;

            choice = getValidatedInt("Choice: ", 1, 3);

            switch (choice) {
                case 1:
                    clearScreen();
                    cout << "=== USER LOGIN ===" << endl;
                    cout << "Username: "; cin >> user;
                    cout << "Password: "; cin >> pass;

                    if (Database::loginUser(user, pass)) {
                        currentUser = user;
                        cout << "\n[SUCCESS] Welcome back, " << currentUser << "!" << endl;
                        FoodDeliverySystem delivery(currentUser);
                        delivery.showMenu();
                    } else {
                        cout << "\n[ERROR] Invalid credentials." << endl;
                        pauseScreen();
                    }
                    break;

                case 2:
                    clearScreen();
                    cout << "=== USER REGISTRATION ===" << endl;
                    cout << "New Username: "; cin >> user;
                    cout << "New Password: "; cin >> pass;

                    if (Database::registerUser(user, pass)) {
                        cout << "\n[SUCCESS] Registration successful! You can now log in." << endl;
                    } else {
                        cout << "\n[ERROR] Username already exists." << endl;
                    }
                    pauseScreen();
                    break;

                case 3:
                    clearScreen();
                    cout << "Thank you for using Food Ordering App!" << endl;
                    break;
            }
        } while (choice != 3);
    }
};

// ============ MAIN ============

int main() {
    Database::initializeMenu();
    FoodApp app;
    app.run();

    return 0;
}