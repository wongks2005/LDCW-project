#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdlib>

using namespace std;

// --- Helper Function ---
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
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

    // Updated to save items and quantities alongside the total price
    static void saveOrder(const string& username, double total, const vector<CartItem>& currentCart) {
        ofstream file("orders.txt", ios::app);
        file << username << "," << total << ",";
        for (size_t i = 0; i < currentCart.size(); ++i) {
            file << currentCart[i].item.name << ":" << currentCart[i].quantity;
            if (i < currentCart.size() - 1) file << "|";
        }
        file << "\n";
    }

    // Updated to parse and display individual items from the saved order string
    static void viewOrderHistory(const string& username) {
        ifstream file("orders.txt");
        string line, user, total, itemsStr;
        bool found = false;
        
        clearScreen();
        cout << "\n--- Order History for " << username << " ---\n";
        
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, user, ',');
            getline(ss, total, ',');
            getline(ss, itemsStr, ','); 

            if (user == username) {
                cout << "Order Total: $" << total << "\n";
                cout << "Items Ordered:\n";
                
                stringstream ssItems(itemsStr);
                string singleItem;
                while (getline(ssItems, singleItem, '|')) {
                    size_t colonPos = singleItem.find(':');
                    if (colonPos != string::npos) {
                        string name = singleItem.substr(0, colonPos);
                        string qty = singleItem.substr(colonPos + 1);
                        cout << "  - " << qty << "x " << name << "\n";
                    }
                }
                cout << "--------------------------\n";
                found = true;
            }
        }
        if (!found) cout << "No previous orders found.\n";
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
        cout << "\n--- Menu ---\n";
        for (const auto& item : menu) {
            cout << item.id << ". " << left << setw(20) << item.name 
                 << "$" << fixed << setprecision(2) << item.price << "\n";
        }
    }

    void addToCart() {
        displayMenu();
        int id, qty;
        cout << "\nEnter Item ID to add (or 0 to cancel): ";
        cin >> id;
        if (id == 0) return;
        
        cout << "Enter Quantity: ";
        cin >> qty;

        for (const auto& item : menu) {
            if (item.id == id) {
                cart.push_back({item, qty});
                cout << qty << "x " << item.name << " added to cart.\n";
                pauseScreen();
                return;
            }
        }
        cout << "Invalid Item ID.\n";
        pauseScreen();
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

    void checkout() {
        if (cart.empty()) {
            cout << "Cart is empty. Cannot checkout.\n";
            pauseScreen();
            return;
        }
        
        cout << "\nConfirm order? (y/n): ";
        char confirm;
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            double total = 0;
            for (const auto& c : cart) total += c.item.price * c.quantity;
            
            // Pass the cart to save individual items in history
            Database::saveOrder(currentUser, total, cart);
            cart.clear();
            cout << "Order placed successfully! Thank you.\n";
        } else {
            cout << "Checkout cancelled.\n";
        }
        pauseScreen();
    }

    // New dedicated Cart Page feature
    void cartPage() {
        int choice;
        do {
            clearScreen();
            viewCart();
            cout << "\n--- Cart Menu ---\n";
            cout << "1. Checkout\n";
            cout << "2. Clear Cart\n";
            cout << "3. Go Back to Main Menu\n";
            cout << "Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: 
                    checkout(); 
                    if (cart.empty()) choice = 3; // Exit cart page if checkout was successful
                    break;
                case 2: 
                    cart.clear(); 
                    cout << "Cart cleared.\n"; 
                    pauseScreen();
                    break;
                case 3: 
                    break;
                default: 
                    cout << "Invalid choice.\n"; 
                    pauseScreen();
            }
        } while (choice != 3);
    }

    void userMenu() {
        int choice;
        do {
            clearScreen();
            cout << "\n--- Welcome, " << currentUser << " ---\n";
            cout << "1. View Menu & Add to Cart\n";
            cout << "2. Open Cart Page\n";
            cout << "3. View Order History\n";
            cout << "4. Logout\n";
            cout << "Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addToCart(); break;
                case 2: cartPage(); break;
                case 3: Database::viewOrderHistory(currentUser); break;
                case 4: cout << "Logging out...\n"; currentUser = ""; pauseScreen(); break;
                default: cout << "Invalid choice.\n"; pauseScreen();
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
            cout << "\n=== Food Ordering App ===\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. Exit\n";
            cout << "Choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    cout << "Username: "; cin >> user;
                    cout << "Password: "; cin >> pass;
                    if (Database::loginUser(user, pass)) {
                        currentUser = user;
                        userMenu();
                    } else {
                        cout << "Invalid credentials.\n";
                        pauseScreen();
                    }
                    break;
                case 2:
                    cout << "New Username: "; cin >> user;
                    cout << "New Password: "; cin >> pass;
                    if (Database::registerUser(user, pass)) {
                        cout << "Registration successful! You can now log in.\n";
                    } else {
                        cout << "Username already exists.\n";
                    }
                    pauseScreen();
                    break;
                case 3:
                    cout << "Exiting app. Goodbye!\n";
                    break;
                default:
                    cout << "Invalid choice.\n";
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