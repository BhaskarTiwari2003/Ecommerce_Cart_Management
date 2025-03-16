#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;

// Product structure
struct Product {
    int id;
    string name;
    double price;
};

// Shopping cart class
class ShoppingCart {
private:
    map<int, int> cart; // product ID -> quantity
    string cartFile = "cart.txt";  // File to store cart data
    string ordersFile = "orders.txt"; // File to store past orders

public:
    // Load cart from file
    void loadCart() {
        ifstream file(cartFile);
        if (!file) return;
        cart.clear();
        int productId, quantity;
        while (file >> productId >> quantity) {
            cart[productId] = quantity;
        }
        file.close();
    }

    // Save cart to file
    void saveCart() {
        ofstream file(cartFile);
        for (const auto& item : cart) {
            file << item.first << " " << item.second << endl;
        }
        file.close();
    }

    // Save order history
    void saveOrderHistory(const vector<Product>& products) {
        ofstream file(ordersFile, ios::app);
        if (file.is_open()) {
            file << "Order:\n";
            for (const auto& item : cart) {
                auto it = find_if(products.begin(), products.end(), [item](const Product& p) { return p.id == item.first; });
                if (it != products.end()) {
                    file << "Product: " << it->name << " | Price: $" << it->price << " | Quantity: " << item.second << "\n";
                }
            }
            file << "------------------------\n";
            file.close();
        }
    }

    // View order history
    void viewOrderHistory() {
        ifstream file(ordersFile);
        if (!file) {
            cout << "No past purchases found.\n";
            return;
        }
        string line;
        cout << "\nPrevious Purchases:\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    // Add multiple products to cart
    void addProducts() {
        int productId, quantity;
        while (true) {
            cout << "Enter product ID to add (0 to stop): ";
            cin >> productId;
            if (productId == 0) break;
            cout << "Enter quantity: ";
            cin >> quantity;
            cart[productId] += quantity;
            cout << "Added " << quantity << " of product " << productId << " to cart.\n";
        }
        saveCart();
    }

    // View cart with product names
    void viewCart(const vector<Product>& products) {
        if (cart.empty()) {
            cout << "Your cart is empty.\n";
            return;
        }
        cout << "Your Cart:\n";
        double totalCost = 0;
        for (const auto& item : cart) {
            int productId = item.first;
            auto it = find_if(products.begin(), products.end(), 
                              [productId](const Product& p) { return p.id == productId; });
            if (it != products.end()) {
                double cost = it->price * item.second;
                totalCost += cost;
                cout << "Product: " << it->name << " | Quantity: " << item.second << " | Price: $" << cost << "\n";
            } else {
                cout << "Product ID: " << productId << " | Quantity: " << item.second << " | (Product not found)\n";
            }
        }
        cout << "Total Cost: $" << totalCost << "\n";
    }



    void removeProduct() {
        int productId;
        cout << "Enter product ID to remove: ";
        cin >> productId;

        auto it = cart.find(productId);
        if (it != cart.end()) {
            int quantity;
            cout << "Enter quantity to remove (current quantity: " << it->second << "): ";
            cin >> quantity;
            if (quantity >= it->second) {
                cart.erase(it);
                cout << "Product removed from cart.\n";
            } else {
                it->second -= quantity;
                cout << quantity << " of product " << productId << " removed from cart.\n";
            }
            saveCart();
        } else {
            cout << "Product not found in cart.\n";
        }
    }

    void emptyCart() {
        cart.clear();
        saveCart();
        cout << "Your cart has been emptied.\n";
    }

    // Checkout process
    void checkout(const vector<Product>& products) {
        if (cart.empty()) {
            cout << "Your cart is empty. Nothing to checkout.\n";
            return;
        }
        cout << "Proceeding to checkout...\n";
        cout << "Thank you for shopping with us!\n";
        saveOrderHistory(products);
        cart.clear();
        saveCart();
    }
};

// Product Manager (For Admin)
class ProductManager {
private:
    string productFile = "products.txt";

public:
    vector<Product> loadProducts() {
        vector<Product> products;
        ifstream file(productFile);
        if (!file) return products;

        int id;
        string name;
        double price;
        while (file >> id >> ws && getline(file, name, ',') && file >> price) {
            products.push_back({id, name, price});
        }
        file.close();
        return products;
    }

    void saveProduct(const vector<Product>& products) {
        ofstream file(productFile, ios::app);
        for (const auto& product : products) {
            file << product.id << " " << product.name << "," << product.price << endl;
        }
        file.close();
    }

    void addProducts() {
        vector<Product> products = loadProducts();
        int newId = products.empty() ? 1 : products.back().id + 1;
        vector<Product> newProducts;
        while (true) {
            string name;
            double price;
            cout << "Enter product name (or type 'done' to stop): ";
            cin.ignore();
            getline(cin, name);
            if (name == "done") break;
            cout << "Enter product price: ";
            cin >> price;
            newProducts.push_back({newId++, name, price});
        }
        saveProduct(newProducts);
        cout << "Products added successfully!\n";
    }

    void viewProducts() {
        vector<Product> products = loadProducts();
        cout << "\nAvailable Products:\n";
        for (const auto& product : products) {
            cout << product.id << ". " << product.name << " - $" << product.price << "\n";
        }
    }
};

// Main function
int main() {
    ProductManager productManager;
    ShoppingCart cart;
    cart.loadCart();
    vector<Product> products = productManager.loadProducts();

    int userType;
    cout << "1. User Login\n2. Admin Login\n3. Exit\nEnter choice: ";
    cin >> userType;

    if (userType == 2) {
        string password;
        cout << "Enter Admin Password: ";
        cin >> password;

        if (password == "admin123") {
            int adminChoice;
            while (true) {
                cout << "\nAdmin Panel:\n1. View Products\n2. Add Products\n3. Exit\nEnter choice: ";
                cin >> adminChoice;
                if (adminChoice == 1) {
                    productManager.viewProducts();
                } else if (adminChoice == 2) {
                    productManager.addProducts();
                } else if (adminChoice == 3) {
                    break;
                }
            }
        }
    } else if (userType == 1) {
        int choice;
        while (true) {
            cout << "\n1. View Products\n2. Add to Cart\n3. View Cart\n4. Remove from Cart\n5. Empty Cart\n6. Checkout\n7. View Orders\n8. Exit\nEnter choice: ";
            cin >> choice;
            if (choice == 1) productManager.viewProducts();
            else if (choice == 2) cart.addProducts();
            else if (choice == 3) cart.viewCart(products);
            else if (choice == 4) cart.removeProduct();
            else if (choice == 5) cart.emptyCart();
            else if (choice == 6) cart.checkout(products);
            else if (choice == 7) cart.viewOrderHistory();
            else if (choice == 8) break;
        }
    }
    return 0;
}