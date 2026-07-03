/**
 * Food Delivery Order System
 * Inspired by Foodpanda - Food Delivery Platform
 * 
 * Part of LDCW6123 Group Project
 * 
 * This program simulates a food delivery ordering system
 * with restaurant selection, menu browsing, and order calculation
 * 
 * VERSION 3.0 - CLEAN OUTPUT (NO SPECIAL CHARACTERS)
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
#include <vector>
#include <limits>

using namespace std;

// Function Prototypes
void displayMainMenu();
void displayRestaurants();
void selectRestaurant(int &choice, string &restaurantName);
void displayMenu(int restaurantChoice, vector<string> &items, vector<double> &prices);
void selectItems(vector<string> &selectedItems, vector<double> &selectedPrices, int restaurantChoice);
double calculateDeliveryFee(int distance);
double applyPromo(string promoCode, double subtotal);
void displayOrderSummary(string customerName, string restaurantName, 
                         vector<string> &selectedItems, vector<double> &selectedPrices,
                         double subtotal, double deliveryFee, double discount, double total);
void displayThankYou();
void clearScreen();

// ============ INPUT VALIDATION FUNCTIONS ============

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

int getValidatedIntNoRange(string prompt) {
    int input;
    bool valid = false;
    
    do {
        cout << prompt;
        cin >> input;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Please enter a valid NUMBER!" << endl;
        }
        else {
            valid = true;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
    } while (!valid);
    
    return input;
}

double getValidatedDouble(string prompt, double minVal, double maxVal) {
    double input;
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

string getValidatedString(string prompt, bool allowEmpty = false) {
    string input;
    bool valid = false;
    
    do {
        cout << prompt;
        getline(cin, input);
        
        size_t start = input.find_first_not_of(" \t");
        if (start != string::npos) {
            input = input.substr(start);
        }
        size_t end = input.find_last_not_of(" \t");
        if (end != string::npos) {
            input = input.substr(0, end + 1);
        }
        
        if (!allowEmpty && input.empty()) {
            cout << "[ERROR] Input cannot be empty! Please try again." << endl;
        }
        else {
            valid = true;
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

string getValidatedPromo(string prompt) {
    string input;
    bool valid = false;
    
    do {
        cout << prompt;
        cin >> input;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[ERROR] Invalid input! Please try again." << endl;
        }
        else {
            for (char &c : input) {
                c = toupper(c);
            }
            
            if (input != "NONE" && input != "FOOD10" && input != "FOOD20" && input != "FREEDEL") {
                cout << "[NOTE] '" << input << "' is not a valid promo code." << endl;
                cout << "Valid codes: FOOD10, FOOD20, FREEDEL, or NONE" << endl;
                cout << "Do you want to continue with NONE? (Y/N): ";
                char confirm = getValidatedChar("");
                if (confirm == 'Y') {
                    input = "NONE";
                    valid = true;
                }
            }
            else {
                valid = true;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
    } while (!valid);
    
    return input;
}

// ============ MAIN FUNCTION ============
int main() {
    int restaurantChoice = 0;
    string restaurantName = "";
    vector<string> selectedItems;
    vector<double> selectedPrices;
    int deliveryDistance = 0;
    string promoCode = "";
    double discount = 0.0;
    string customerName = "";
    bool exitProgram = false;
    
    cout << "===========================================" << endl;
    cout << "      FOOD DELIVERY ORDER SYSTEM          " << endl;
    cout << "         (Inspired by Foodpanda)          " << endl;
    cout << "===========================================" << endl << endl;
    
    cout << "Welcome to the Food Delivery Order System!" << endl;
    
    customerName = getValidatedString("Please enter your name: ", false);
    cout << endl << "Hello, " << customerName << "! Let's get started." << endl;
    
    do {
        cout << endl << "===========================================" << endl;
        cout << "          MAIN MENU                       " << endl;
        cout << "===========================================" << endl;
        cout << "1. Place a New Order" << endl;
        cout << "2. Exit System" << endl;
        cout << "===========================================" << endl;
        
        int mainChoice = getValidatedInt("Enter your choice (1-2): ", 1, 2);
        
        if (mainChoice == 2) {
            exitProgram = true;
            break;
        }
        
        bool orderComplete = false;
        
        do {
            selectedItems.clear();
            selectedPrices.clear();
            discount = 0.0;
            
            displayRestaurants();
            selectRestaurant(restaurantChoice, restaurantName);
            
            displayMenu(restaurantChoice, selectedItems, selectedPrices);
            selectItems(selectedItems, selectedPrices, restaurantChoice);
            
            if (selectedItems.empty()) {
                cout << endl << "[ERROR] No items were selected!" << endl;
                char tryAgain = getValidatedChar("Would you like to try again? (Y/N): ");
                if (tryAgain == 'Y') {
                    continue;
                } else {
                    break;
                }
            }
            
            deliveryDistance = getValidatedInt("Enter delivery distance in kilometers (1-20): ", 1, 20);
            promoCode = getValidatedPromo("Enter promo code (or type 'NONE' to skip): ");
            
            double subtotal = 0.0;
            for (double price : selectedPrices) {
                subtotal += price;
            }
            
            double deliveryFee = calculateDeliveryFee(deliveryDistance);
            
            if (promoCode == "FREEDEL") {
                deliveryFee = 0.0;
                cout << endl << "[PROMO] Free delivery applied!" << endl;
                discount = 0.0;
            } else {
                discount = applyPromo(promoCode, subtotal);
            }
            
            double total = subtotal + deliveryFee - discount;
            
            displayOrderSummary(customerName, restaurantName, selectedItems, selectedPrices, 
                               subtotal, deliveryFee, discount, total);
            
            cout << endl << "===========================================" << endl;
            cout << "What would you like to do?" << endl;
            cout << "1. Place another order (different restaurant)" << endl;
            cout << "2. Go back to Main Menu" << endl;
            cout << "3. Exit System" << endl;
            cout << "===========================================" << endl;
            
            int nextAction = getValidatedInt("Enter your choice (1-3): ", 1, 3);
            
            if (nextAction == 1) {
                cout << endl << "Starting a new order..." << endl;
                continue;
            } else if (nextAction == 2) {
                cout << endl << "Returning to main menu..." << endl;
                orderComplete = true;
                break;
            } else if (nextAction == 3) {
                exitProgram = true;
                orderComplete = true;
                break;
            }
            
        } while (!orderComplete && !exitProgram);
        
    } while (!exitProgram);
    
    displayThankYou();
    
    return 0;
}

// ============ FUNCTION IMPLEMENTATIONS ============

void displayRestaurants() {
    cout << endl << "=== AVAILABLE RESTAURANTS ===" << endl;
    cout << "1. McDonald's - Fast Food" << endl;
    cout << "2. KFC - Fried Chicken" << endl;
    cout << "3. Pizza Hut - Pizzas" << endl;
    cout << "4. Starbucks - Coffee & Drinks" << endl;
    cout << "5. Local Noodle House - Asian Cuisine" << endl;
    cout << "6. Subway - Sandwiches" << endl;
    cout << "7. Sushi King - Japanese" << endl;
    cout << "8. Burger King - Burgers" << endl;
    cout << "=============================" << endl;
}

void selectRestaurant(int &choice, string &restaurantName) {
    choice = getValidatedInt("Select a restaurant (1-8): ", 1, 8);
    
    switch (choice) {
        case 1: restaurantName = "McDonald's"; break;
        case 2: restaurantName = "KFC"; break;
        case 3: restaurantName = "Pizza Hut"; break;
        case 4: restaurantName = "Starbucks"; break;
        case 5: restaurantName = "Local Noodle House"; break;
        case 6: restaurantName = "Subway"; break;
        case 7: restaurantName = "Sushi King"; break;
        case 8: restaurantName = "Burger King"; break;
        default: restaurantName = "Unknown"; break;
    }
    
    cout << endl << "You selected: " << restaurantName << "!" << endl;
}

void displayMenu(int restaurantChoice, vector<string> &items, vector<double> &prices) {
    cout << endl << "=== MENU ===" << endl;
    
    items.clear();
    prices.clear();
    
    switch (restaurantChoice) {
        case 1:
            items = {"Big Mac", "McChicken", "Fries", "McFlurry", "Coca-Cola"};
            prices = {18.50, 12.00, 9.00, 8.50, 5.00};
            break;
        case 2:
            items = {"Original Recipe Chicken", "Zinger Burger", "Coleslaw", "Mashed Potatoes", "Pepsi"};
            prices = {22.00, 15.00, 6.00, 7.00, 5.00};
            break;
        case 3:
            items = {"Margherita Pizza", "Pepperoni Pizza", "Hawaiian Pizza", "Garlic Bread", "Soft Drink"};
            prices = {25.00, 28.00, 27.00, 10.00, 5.00};
            break;
        case 4:
            items = {"Caramel Latte", "Cappuccino", "Green Tea", "Croissant", "Muffin"};
            prices = {16.00, 14.00, 12.00, 8.00, 7.00};
            break;
        case 5:
            items = {"Chicken Noodle Soup", "Beef Noodle", "Fried Rice", "Spring Rolls", "Thai Tea"};
            prices = {15.00, 18.00, 13.00, 10.00, 6.00};
            break;
        case 6:
            items = {"Italian BMT", "Turkey Breast", "Veggie Delite", "Cookies", "Soft Drink"};
            prices = {18.00, 16.00, 12.00, 5.00, 5.00};
            break;
        case 7:
            items = {"California Roll", "Sashimi Set", "Tempura", "Miso Soup", "Green Tea"};
            prices = {20.00, 30.00, 18.00, 8.00, 5.00};
            break;
        case 8:
            items = {"Whopper", "Chicken Burger", "Onion Rings", "Ice Cream", "Coca-Cola"};
            prices = {20.00, 14.00, 8.00, 6.00, 5.00};
            break;
        default:
            items = {"No items available"};
            prices = {0.00};
            break;
    }
    
    for (size_t i = 0; i < items.size(); i++) {
        cout << i + 1 << ". " << items[i] << " - RM " << fixed << setprecision(2) << prices[i] << endl;
    }
}

void selectItems(vector<string> &selectedItems, vector<double> &selectedPrices, int restaurantChoice) {
    int itemChoice;
    int quantity;
    char addMore;
    
    cout << endl << "Select items to order (or 0 to finish):" << endl;
    
    do {
        itemChoice = getValidatedInt("Enter item number (0 to finish): ", 0, 5);
        
        if (itemChoice == 0) break;
        
        vector<string> tempItems;
        vector<double> tempPrices;
        displayMenu(restaurantChoice, tempItems, tempPrices);
        
        string itemName = tempItems[itemChoice - 1];
        double itemPrice = tempPrices[itemChoice - 1];
        
        quantity = getValidatedInt("Enter quantity (1-99): ", 1, 99);
        
        for (int i = 0; i < quantity; i++) {
            selectedItems.push_back(itemName);
            selectedPrices.push_back(itemPrice);
        }
        
        cout << "Added " << quantity << "x " << itemName << " to your order." << endl;
        addMore = getValidatedChar("Add more items? (Y/N): ");
        
    } while (addMore == 'Y');
}

double calculateDeliveryFee(int distance) {
    double fee = 0.0;
    
    if (distance <= 3) {
        fee = 3.00;
    } else if (distance <= 5) {
        fee = 5.00;
    } else if (distance <= 8) {
        fee = 8.00;
    } else if (distance <= 12) {
        fee = 12.00;
    } else {
        fee = 15.00;
    }
    
    return fee;
}

double applyPromo(string promoCode, double subtotal) {
    double discount = 0.0;
    
    if (promoCode == "FOOD10") {
        discount = subtotal * 0.10;
        cout << endl << "[PROMO] 10% discount applied! - RM " << fixed << setprecision(2) << discount << endl;
    } else if (promoCode == "FOOD20") {
        discount = subtotal * 0.20;
        cout << endl << "[PROMO] 20% discount applied! - RM " << fixed << setprecision(2) << discount << endl;
    } else if (promoCode == "NONE") {
        cout << endl << "[INFO] No promo code applied." << endl;
        discount = 0.0;
    }
    
    return discount;
}

void displayOrderSummary(string customerName, string restaurantName, 
                         vector<string> &selectedItems, vector<double> &selectedPrices,
                         double subtotal, double deliveryFee, double discount, double total) {
    
    cout << endl << "===========================================" << endl;
    cout << "           ORDER SUMMARY                    " << endl;
    cout << "===========================================" << endl;
    cout << "Customer: " << customerName << endl;
    cout << "Restaurant: " << restaurantName << endl;
    cout << endl << "--- Order Details ---" << endl;
    
    vector<string> uniqueItems;
    vector<int> quantities;
    vector<double> itemPrices;
    
    for (size_t i = 0; i < selectedItems.size(); i++) {
        bool found = false;
        for (size_t j = 0; j < uniqueItems.size(); j++) {
            if (uniqueItems[j] == selectedItems[i]) {
                quantities[j]++;
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueItems.push_back(selectedItems[i]);
            quantities.push_back(1);
            itemPrices.push_back(selectedPrices[i]);
        }
    }
    
    for (size_t i = 0; i < uniqueItems.size(); i++) {
        cout << "   " << quantities[i] << "x " << uniqueItems[i] 
             << " @ RM " << fixed << setprecision(2) << itemPrices[i]
             << " = RM " << fixed << setprecision(2) << (quantities[i] * itemPrices[i]) << endl;
    }
    
    cout << endl << "--- Payment Summary ---" << endl;
    cout << "   Subtotal: RM " << fixed << setprecision(2) << subtotal << endl;
    cout << "   Delivery Fee: RM " << fixed << setprecision(2) << deliveryFee << endl;
    
    if (discount > 0) {
        cout << "   Discount: -RM " << fixed << setprecision(2) << discount << endl;
    }
    
    cout << "-------------------------------------------" << endl;
    cout << "   TOTAL: RM " << fixed << setprecision(2) << total << endl;
    cout << "===========================================" << endl;
}

void displayThankYou() {
    cout << endl << "===========================================" << endl;
    cout << "    Thank you for using Food Delivery System!" << endl;
    cout << "    Your order will be delivered soon!" << endl;
    cout << "    (Inspired by Foodpanda)" << endl;
    cout << "===========================================" << endl;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}