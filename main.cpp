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
    // Methods will be implemented in Stage 2
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