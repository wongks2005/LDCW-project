/**
 * Food Delivery Order System (Prototype Version)
 * * This initial baseline version establishes basic file-handling and core application logic:
 * - Single-Menu Architecture: Supports a single flat list of items mapped directly from a basic menu file.
 * - Flat Data Structures: Employs a straightforward, non-relational vector storage scheme for menu tracking.
 * - Simple Input Extraction: Utilizes stringstream lines for numeric evaluation without active looping error correction.
 * - Cyclic Cart Environment: Relies on a traditional do-while control loop within the cart view submenu.
 * - Standard Global Currency: Configured for basic dollar ($) formatting.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace std;

// --- Helper Functions ---


void clearScreen() {
    cout << "\033[2J\033[1;1H";
}


void pauseScreen() {
    cout << endl << "Press Enter to continue...";
    string dummy;
    getline(cin, dummy);
}


int getIntegerInput() {
    string input;
    getline(cin, input);
    stringstream ss(input);
    int value;
    if (ss >> value) {
        return value;
    }
    return -1; 
}

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
        if (username.empty() || password.empty()) return false;
        
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
        file << endl;
    }

    static void viewOrderHistory(const string& username) {
        ifstream file("orders.txt");
        string line, user, total, itemsStr;
        bool found = false;
        
        clearScreen();
        cout << endl << "--- Order History for " << username << " ---" << endl;
        
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, user, ',');
            getline(ss, total, ',');
            getline(ss, itemsStr, ','); 

            if (user == username) {
                cout << "Order Total: $" << total << endl;
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
                cout << "--------------------------" << endl;
                found = true;
            }
        }
        if (!found) cout << "No previous orders found." << endl;
        pauseScreen();
    }
};

// --- Application Logic ---

class FoodApp {
private:
    string currentUser;
    vector<MenuItem> menu;
    vector<CartItem> cart;

    void displayMenu() {
        clearScreen();
        cout << endl << "--- Menu ---" << endl;
        for (const auto& item : menu) {
            cout << item.id << ". " << left << setw(20) << item.name 
                 << "$" << fixed << setprecision(2) << item.price << endl;
        }
    }

    void addToCart() {
        displayMenu();
        cout << endl << "Enter Item ID to add (or 0 to cancel): ";
        int id = getIntegerInput();
        
        if (id == 0) return;
        if (id == -1) {
            cout << "Invalid input. Operational ID must be a number." << endl;
            pauseScreen();
            return;
        }
        
        cout << "Enter Quantity: ";
        int qty = getIntegerInput();

        if (qty <= 0) {
            cout << "Invalid quantity entered." << endl;
            pauseScreen();
            return;
        }

        for (const auto& item : menu) {
            if (item.id == id) {
                cart.push_back({item, qty});
                cout << qty << "x " << item.name << " added to cart." << endl;
                pauseScreen();
                return;
            }
        }
        cout << "Invalid Item ID." << endl;
        pauseScreen();
    }

    void viewCart() {
        if (cart.empty()) {
            cout << endl << "Your cart is empty." << endl;
            return;
        }
        cout << endl << "--- Your Cart ---" << endl;
        double total = 0;
        for (const auto& cItem : cart) {
            double cost = cItem.item.price * cItem.quantity;
            total += cost;
            cout << cItem.quantity << "x " << left << setw(18) << cItem.item.name 
                 << "$" << fixed << setprecision(2) << cost << endl;
        }
        cout << "--------------------------" << endl;
        cout << "Total: $" << total << endl;
    }

    void checkout() {
        if (cart.empty()) {
            cout << "Cart is empty. Cannot checkout." << endl;
            pauseScreen();
            return;
        }
        
        cout << endl << "Confirm order? (y/n): ";
        string confirm;
        getline(cin, confirm);
        
        if (confirm == "y" || confirm == "Y") {
            double total = 0;
            for (const auto& c : cart) total += c.item.price * c.quantity;
            
            Database::saveOrder(currentUser, total, cart);
            cart.clear();
            cout << "Order placed successfully! Thank you." << endl;
        } else {
            cout << "Checkout cancelled." << endl;
        }
        pauseScreen();
    }

    void cartPage() {
        int choice;
        do {
            clearScreen();
            viewCart();
            cout << endl << "--- Cart Menu ---" << endl;
            cout << "1. Checkout" << endl;
            cout << "2. Clear Cart" << endl;
            cout << "3. Go Back to Main Menu" << endl;
            cout << "Choice: ";
            
            choice = getIntegerInput();

            switch (choice) {
                case 1: 
                    checkout(); 
                    if (cart.empty()) choice = 3; 
                    break;
                case 2: 
                    cart.clear(); 
                    cout << "Cart cleared." << endl; 
                    pauseScreen();
                    break;
                case 3: 
                    break;
                default: 
                    cout << "Invalid choice." << endl; 
                    pauseScreen();
            }
        } while (choice != 3);
    }

    void userMenu() {
        int choice;
        do {
            clearScreen();
            cout << endl << "--- Welcome, " << currentUser << " ---" << endl;
            cout << "1. View Menu & Add to Cart" << endl;
            cout << "2. Open Cart Page" << endl;
            cout << "3. View Order History" << endl;
            cout << "4. Logout" << endl;
            cout << "Choice: ";
            
            choice = getIntegerInput();

            switch (choice) {
                case 1: addToCart(); break;
                case 2: cartPage(); break;
                case 3: Database::viewOrderHistory(currentUser); break;
                case 4: cout << "Logging out..." << endl; currentUser = ""; pauseScreen(); break;
                default: cout << "Invalid choice." << endl; pauseScreen();
            }
        } while (choice != 4);
    }

public:
    FoodApp() {
        Database::initializeMenu();
        menu = Database::loadMenu();
    }

    void run() {
        int choice;
        string user, pass;
        do {
            clearScreen();
            cout << endl << "=== Food Ordering App ===" << endl;
            cout << "1. Login" << endl;
            cout << "2. Register" << endl;
            cout << "3. Exit" << endl;
            cout << "Choice: ";
            
            choice = getIntegerInput();

            switch (choice) {
                case 1:
                    cout << "Username: "; getline(cin, user);
                    cout << "Password: "; getline(cin, pass);
                    
                    if (Database::loginUser(user, pass)) {
                        currentUser = user;
                        userMenu();
                    } else {
                        cout << "Invalid credentials." << endl;
                        pauseScreen();
                    }
                    break;
                case 2:
                    cout << "New Username: "; getline(cin, user);
                    cout << "New Password: "; getline(cin, pass);
                    
                    if (Database::registerUser(user, pass)) {
                        cout << "Registration successful! You can now log in." << endl;
                    } else {
                        cout << "Username invalid or already exists." << endl;
                    }
                    pauseScreen();
                    break;
                case 3:
                    cout << "Exiting app. Goodbye!" << endl;
                    break;
                default:
                    cout << "Invalid choice." << endl;
                    pauseScreen();
            }
        } while (choice != 3);
    }
};

int main() {
    FoodApp app;
    app.run();
    return 0;
}