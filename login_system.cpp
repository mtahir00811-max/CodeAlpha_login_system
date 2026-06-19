#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <limits>
using namespace std;
const string DATA_FILE = "users.txt";
// ---------- Helper: simple djb2 hash (turns password into a number string) ----------
string hashPassword(const string &password) {
    unsigned long hash = 5381;
    for (char c : password) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); // hash * 33 + c
    }
    return to_string(hash);
}
// ---------- Input validation ----------
bool isValidUsername(const string &username) {
    if (username.length() < 3 || username.length() > 20) return false;
    for (char c : username) {
        if (!isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    }
    return true;
}
bool isValidPassword(const string &password) {
    if (password.length() < 6) return false;
    bool hasDigit = false, hasAlpha = false;
    for (char c : password) {
        if (isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        if (isalpha(static_cast<unsigned char>(c))) hasAlpha = true;
    }
    return hasDigit && hasAlpha; // must contain at least one letter and one digit
}

// ---------- Check if username already exists in the data file ----------
bool usernameExists(const string &username) {
    ifstream file(DATA_FILE);
    if (!file.is_open()) return false; // file doesn't exist yet -> no users registered

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUser;
        getline(ss, storedUser, ':');
        if (storedUser == username) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}
// ---------- Registration ----------
void registerUser() {
    string username, password, confirmPassword;
    cout << "\n--- Register New Account ---\n";
    cout << "Enter username (3-20 chars, letters/digits/underscore only): ";
    cin >> username;
    if (!isValidUsername(username)) {
        cout << "Error: Invalid username. Use 3-20 alphanumeric characters or underscore.\n";
        return;
    }
    if (usernameExists(username)) {
        cout << "Error: Username \"" << username << "\" is already taken. Please choose another.\n";
        return;
    }
    cout << "Enter password (min 6 chars, at least 1 letter and 1 digit): ";
    cin >> password;
    if (!isValidPassword(password)) {
        cout << "Error: Weak password. It must be at least 6 characters and include both letters and digits.\n";
        return;
    }
    cout << "Confirm password: ";
    cin >> confirmPassword;
    if (password != confirmPassword) {
        cout << "Error: Passwords do not match. Registration cancelled.\n";
        return;
    }
    // Append the new user to the data file
    ofstream file(DATA_FILE, ios::app);
    if (!file.is_open()) {
        cout << "Error: Could not access the data file. Registration failed.\n";
        return;
    }
    file << username << ":" << hashPassword(password) << "\n";
    file.close();
    cout << "Success: User \"" << username << "\" registered successfully!\n";
}
// ---------- Login ----------
void loginUser() {
    string username, password;
    cout << "\n--- Login ---\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "Error: No registered users found. Please register first.\n";
        return;
    }
    string line;
    string hashedInput = hashPassword(password);
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUser, storedHash;
        getline(ss, storedUser, ':');
        getline(ss, storedHash);
        if (storedUser == username) {
            file.close();
            if (storedHash == hashedInput) {
                cout << "Success: Welcome back, " << username << "! Login successful.\n";
            } else {
                cout << "Error: Incorrect password.\n";
            }
            return;
        }
    }
    file.close();
    cout << "Error: Username \"" << username << "\" not found. Please register first.\n";
}
// ---------- Menu-driven main ----------
int main() {
    int choice;
    do {
        cout << "\n========== Login & Registration System ==========\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        if (!(cin >> choice)) {
            cout << "Error: Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                cout << "Goodbye!\n";
                break;
            default:
                cout << "Error: Invalid choice. Please select 1, 2, or 3.\n";
        }
    } while (choice != 3);
    return 0;
}