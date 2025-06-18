// TDS4223 Data Structures and Algorithms Project - Sports Allotment System
// Group: [Insert Group Number]
// AI Disclosure: This code was developed with assistance from Grok (created by xAI). Grok provided suggestions for
// structural design (e.g., inheritance hierarchy, login system), algorithm optimization (e.g., TimSort stack management),
// and error handling strategies. All suggestions were manually reviewed, adapted, and implemented by the group.
// Core logic for Hash Table, TimSort, Hashing, and additional features (e.g., edit functions, report generation) was
// written by the group. The code was tested to ensure compliance with project guidelines.

#include <iostream>    // Standard input/output
#include <fstream>     // File stream for reading/writing
#include <cstring>     // C-string manipulation
#include <cctype>      // Character handling
#include <cstdlib>     // Utilities like atoi
#include <iomanip>     // Output formatting
#include <stdexcept>   // Exception handling
#include <direct.h>    // For _getcwd (working directory)
using namespace std;   // Avoid prefixing std::

// Function to print working directory for debugging
void printWorkingDirectory() {
    char cwd[256];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << "Current working directory: " << cwd << endl;
    } else {
        cout << "Error: Cannot get current working directory!" << endl;
    }
}

// Structure for linked list node in Hash Table
struct Node {
    int id;           // Student ID
    char name[50];    // Student name
    char sport[50];   // Sport name
    char time[50];    // Schedule
    Node* next;       // Next node for collision handling
    Node(int i, const char* n, const char* s, const char* t) : id(i), next(NULL) {
        strncpy(name, n, 49); name[49] = '\0';
        strncpy(sport, s, 49); sport[49] = '\0';
        strncpy(time, t, 49); time[49] = '\0';
    }
};

// Structure for sport details
struct Sport {
    char name[50];    // Sport name
    char time[50];    // Schedule
    int capacity;     // Max participants
    int current_count;// Current enrolled
};

// Structure for athlete data in array (for sorting/searching)
struct Athlete {
    int id;           // Student ID
    char name[50];    // Student name
    char sport[50];   // Sport name
    char time[50];    // Schedule
};

// Stack for TimSort run management
struct Stack {
    static const int MAX_SIZE = 100;
    int runStart[MAX_SIZE];
    int runLength[MAX_SIZE];
    int top;
    Stack() : top(-1) {}
    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == MAX_SIZE - 1; }
    void push(int start, int length) {
        if (!isFull()) {
            runStart[++top] = start;
            runLength[top] = length;
        } else {
            throw runtime_error("Stack overflow in TimSort!");
        }
    }
    void pop(int& start, int& length) {
        if (!isEmpty()) {
            start = runStart[top];
            length = runLength[top--];
        } else {
            throw runtime_error("Stack underflow in TimSort!");
        }
    }
};

// Base class for records
class Record {
protected:
    int id;
public:
    Record(int i) : id(i) {} // Constructor to initialize id
    virtual void display() const = 0;
    virtual ~Record() {}
    int getID() const { return id; }
};

// Derived class for athlete records
class AthleteRecord : public Record {
private:
    char name[50], sport[50], time[50];
public:
    AthleteRecord(int i, const char* n, const char* s, const char* t) : Record(i) {
        strncpy(name, n, 49); name[49] = '\0';
        strncpy(sport, s, 49); sport[49] = '\0';
        strncpy(time, t, 49); time[49] = '\0';
    }
    void display() const {
        cout << "ID: " << id << ", Name: " << name << ", Sport: " << sport << ", Time: " << time << endl;
    }
    const char* getSport() const { return sport; }
    const char* getTime() const { return time; }
    const char* getName() const { return name; }
};

// Base class for users
class User {
protected:
    char username[50];
    char role[20]; // Admin, Customer, Guest
public:
    User() {
        strcpy(username, "unknown");
        strcpy(role, "Guest");
    }
    virtual bool login(const char* user, const char* pass) = 0; // Pure virtual
    virtual void logout() = 0; // Pure virtual
    virtual ~User() {} // Virtual destructor
    const char* getUsername() const { return username; }
    const char* getRole() const { return role; }
};

// Derived class: Admin
class Admin : public User {
public:
    Admin() { strcpy(role, "Admin"); }
    bool login(const char* user, const char* pass) {
        try {
            ifstream inFile("login_credentials.txt");
            if (!inFile) throw runtime_error("Cannot open login_credentials.txt for reading");
            char storedUser[50], storedPass[50], storedRole[20];
            while (inFile >> storedUser >> storedPass >> storedRole) {
                if (strcmp(user, storedUser) == 0 && strcmp(pass, storedPass) == 0 &&
                    strcmp(storedRole, "Admin") == 0) {
                    strncpy(username, user, 49); username[49] = '\0';
                    inFile.close();
                    return true;
                }
            }
            inFile.close();
            return false;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        }
    }
    void logout() {
        cout << "Admin " << username << " logged out.\n";
        strcpy(username, "unknown");
    }
    ~Admin() {}
};

// Derived class: Customer
class Customer : public User {
public:
    Customer() { strcpy(role, "Customer"); }
    bool login(const char* user, const char* pass) {
        try {
            ifstream inFile("login_credentials.txt");
            if (!inFile) throw runtime_error("Cannot open login_credentials.txt for reading");
            char storedUser[50], storedPass[50], storedRole[20];
            while (inFile >> storedUser >> storedPass >> storedRole) {
                if (strcmp(user, storedUser) == 0 && strcmp(pass, storedPass) == 0 &&
                    strcmp(storedRole, "Customer") == 0) {
                    strncpy(username, user, 49); username[49] = '\0';
                    inFile.close();
                    return true;
                }
            }
            inFile.close();
            return false;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        }
    }
    void logout() {
        cout << "Customer " << username << " logged out.\n";
        strcpy(username, "unknown");
    }
    ~Customer() {}
};

// Derived class: Guest
class Guest : public User {
public:
    Guest() { strcpy(role, "Guest"); }
    bool login(const char* user, const char* pass) {
        strcpy(username, "guest");
        return true; // No credentials needed
    }
    void logout() {
        cout << "Guest logged out.\n";
        strcpy(username, "unknown");
    }
    ~Guest() {}
};

// Class to manage login credentials
class LoginManager {
private:
    static const int MAX_USERS = 50;
    struct Credential {
        char username[50];
        char password[50];
        char role[20];
    };
    Credential* credentials; // Dynamic memory
    int userCount;

    void loadCredentials() {
        try {
            ifstream inFile("login_credentials.txt");
            if (!inFile) {
                // Create file if it doesn't exist
                ofstream outFile("login_credentials.txt");
                if (!outFile) throw runtime_error("Cannot create login_credentials.txt");
                outFile << "admin1 pass1 Admin\n";
                outFile << "customer1 pass2 Customer\n";
                outFile.close();
                cout << "Created login_credentials.txt with default credentials\n";
            }
            userCount = 0;
            while (inFile >> credentials[userCount].username >> credentials[userCount].password >> credentials[userCount].role) {
                if (userCount < MAX_USERS) userCount++;
            }
            inFile.close();
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

public:
    LoginManager() : userCount(0) {
        try {
            credentials = new Credential[MAX_USERS];
            loadCredentials();
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed for credentials!\n";
            throw;
        }
    }
    bool registerUser(const char* user, const char* pass, const char* role) {
        try {
            if (strlen(user) == 0 || strlen(pass) == 0) throw runtime_error("Username or password cannot be empty!");
            if (strcmp(role, "Admin") != 0 && strcmp(role, "Customer") != 0) throw runtime_error("Invalid role!");
            // Check if user already exists
            for (int i = 0; i < userCount; i++) {
                if (strcmp(credentials[i].username, user) == 0) {
                    throw runtime_error("Username already exists!");
                }
            }
            ofstream outFile("login_credentials.txt", ios::app);
            if (!outFile) {
                // Try creating the file if it doesn't exist
                outFile.open("login_credentials.txt", ios::out);
                if (!outFile) throw runtime_error("Cannot open login_credentials.txt for writing");
            }
            outFile << user << " " << pass << " " << role << endl;
            outFile.close();
            loadCredentials();
            cout << "User registered successfully: " << user << endl;
            return true;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        }
    }
    ~LoginManager() {
        delete[] credentials;
    }
};

// Class to manage summary reports
class ReportManager {
private:
    static const int MAX_REPORTS = 100;
    struct Report {
        char filename[100];
        char content[1000];
    };
    Report* reports; // Dynamic memory
    int reportCount;

public:
    ReportManager() : reportCount(0) {
        try {
            reports = new Report[MAX_REPORTS];
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed for reports!\n";
            throw;
        }
    }
    void generateReport(const char* filename, const char* content) {
        try {
            ofstream outFile(filename);
            if (!outFile) throw runtime_error("Cannot open report file for writing");
            outFile << content << endl;
            outFile.close();
            if (reportCount < MAX_REPORTS) {
                strncpy(reports[reportCount].filename, filename, 99); reports[reportCount].filename[99] = '\0';
                strncpy(reports[reportCount].content, content, 999); reports[reportCount].content[999] = '\0';
                reportCount++;
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    void displayReport(const char* filename) {
        try {
            ifstream inFile(filename);
            if (!inFile) throw runtime_error("Cannot open report file");
            cout << "\n===== Contents of " << filename << " =====\n";
            char line[256];
            while (inFile.getline(line, 256)) {
                cout << line << endl;
            }
            inFile.close();
            cout << "====================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    void generateSportStats(const char* filename, int sportCount, const Sport sports[]) {
        try {
            ofstream outFile(filename);
            if (!outFile) throw runtime_error("Cannot open sport_stats.txt");
            outFile << "===== Sport Enrollment Statistics =====\n";
            outFile << "Sport          | Time              | Enrolled/Capacity\n";
            outFile << "---------------|-------------------|-----------------\n";
            for (int i = 0; i < sportCount; i++) {
                outFile << left << setw(14) << sports[i].name << " | "
                        << setw(17) << sports[i].time << " | "
                        << sports[i].current_count << "/" << sports[i].capacity << endl;
            }
            outFile << "======================================\n";
            outFile.close();
            char content[1000];
            snprintf(content, sizeof(content), "Generated sport statistics for %d sports.", sportCount);
            generateReport("sport_stats.txt", content);
            cout << "Statistics saved to sport_stats.txt\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    ~ReportManager() {
        delete[] reports;
    }
};

// Main Hash Table class
class HashTable {
private:
    static const int TABLE_SIZE = 97; // Large prime for fewer collisions
    Node** table; // Dynamic array of node pointers
    static const int SPORT_COUNT = 8;
    Sport sports[SPORT_COUNT];
    bool isLoadingFromFile;
    static const int MAX_ENTRIES = 100;
    Athlete athletes[MAX_ENTRIES];
    int athleteCount;

    // Friend functions
    friend void printTableSize(const HashTable& ht);
    friend void countEntries(const HashTable& ht);
    friend void checkCapacity(const HashTable& ht, const char* sport);
    friend void resetTable(HashTable& ht);

    int hashFunction(int id) const {
        return id % TABLE_SIZE;
    }

    void initSports() {
        strcpy(sports[0].name, "Football"); strcpy(sports[0].time, "Mon 10:00-12:00"); sports[0].capacity = 20; sports[0].current_count = 0;
        strcpy(sports[1].name, "Basketball"); strcpy(sports[1].time, "Tue 14:00-16:00"); sports[1].capacity = 15; sports[1].current_count = 0;
        strcpy(sports[2].name, "Tennis"); strcpy(sports[2].time, "Wed 09:00-11:00"); sports[2].capacity = 10; sports[2].current_count = 0;
        strcpy(sports[3].name, "Swimming"); strcpy(sports[3].time, "Thu 13:00-15:00"); sports[3].capacity = 12; sports[3].current_count = 0;
        strcpy(sports[4].name, "Volleyball"); strcpy(sports[4].time, "Fri 15:00-17:00"); sports[4].capacity = 15; sports[4].current_count = 0;
        strcpy(sports[5].name, "Badminton"); strcpy(sports[5].time, "Mon 14:00-16:00"); sports[5].capacity = 10; sports[5].current_count = 0;
        strcpy(sports[6].name, "Athletics"); strcpy(sports[6].time, "Tue 10:00-12:00"); sports[6].capacity = 20; sports[6].current_count = 0;
        strcpy(sports[7].name, "Cycling"); strcpy(sports[7].time, "Wed 13:00-15:00"); sports[7].capacity = 10; sports[7].current_count = 0;
    }

    bool isValidSport(const char* sport) const {
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) return true;
        }
        return false;
    }

    bool hasTimeConflict(int id, const char* new_time) const {
        int index = hashFunction(id);
        Node* current = table[index];
        while (current != NULL) {
            if (current->id == id && strcmp(current->time, new_time) == 0) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool checkCapacity(const char* sport) {
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) {
                if (sports[i].current_count < sports[i].capacity) {
                    sports[i].current_count++;
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    void reduceCapacity(const char* sport) {
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) {
                if (sports[i].current_count > 0) sports[i].current_count--;
                break;
            }
        }
    }

    bool idExists(int id) const {
        int index = hashFunction(id);
        Node* current = table[index];
        while (current != NULL) {
            if (current->id == id) return true;
            current = current->next;
        }
        return false;
    }

    bool findStudentName(int id, char* name) const {
        int index = hashFunction(id);
        Node* current = table[index];
        bool first = true;
        char firstName[50];
        while (current != NULL) {
            if (current->id == id) {
                if (first) {
                    strcpy(firstName, current->name);
                    strcpy(name, firstName);
                    first = false;
                } else if (strcmp(firstName, current->name) != 0) {
                    cout << "Warning: ID " << id << " has multiple names: '" << firstName
                         << "' and '" << current->name << "'. Using '" << firstName << "'.\n";
                }
            }
            current = current->next;
        }
        return !first;
    }

    bool isValidID(int id) const {
        return id > 0 && id <= 99999;
    }

    void trimWhitespace(char* str) {
        char* start = str;
        char* end;
        while (isspace(*start)) start++;
        end = start + strlen(start) - 1;
        while (end >= start && isspace(*end)) end--;
        *(end + 1) = '\0';
        if (start != str) memmove(str, start, strlen(start) + 1);
    }

    void insertionSort(Athlete arr[], int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            Athlete temp = arr[i];
            int j = i - 1;
            while (j >= left && arr[j].id > temp.id) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = temp;
        }
    }

    void merge(Athlete arr[], int left, int mid, int right) {
        int len1 = mid - left + 1;
        int len2 = right - mid;
        Athlete* leftArr = new Athlete[len1];
        Athlete* rightArr = new Athlete[len2];
        try {
            for (int i = 0; i < len1; i++) leftArr[i] = arr[left + i];
            for (int i = 0; i < len2; i++) rightArr[i] = arr[mid + 1 + i];
            int i = 0, j = 0, k = left;
            while (i < len1 && j < len2) {
                if (leftArr[i].id <= rightArr[j].id) {
                    arr[k++] = leftArr[i++];
                } else {
                    arr[k++] = rightArr[j++];
                }
            }
            while (i < len1) arr[k++] = leftArr[i++];
            while (j < len2) arr[k++] = rightArr[j++];
            delete[] leftArr;
            delete[] rightArr;
        } catch (...) {
            delete[] leftArr;
            delete[] rightArr;
            throw;
        }
    }

    void timSort() {
        const int RUN = 32;
        Stack stack;
        try {
            for (int i = 0; i < athleteCount; i += RUN) {
                int start = i;
                int end = (i + RUN - 1 < athleteCount - 1) ? i + RUN - 1 : athleteCount - 1;
                insertionSort(athletes, start, end);
                stack.push(start, end - start + 1);
            }
            while (!stack.isEmpty()) {
                int start1, len1, start2, len2;
                stack.pop(start1, len1);
                if (stack.isEmpty()) {
                    stack.push(start1, len1);
                    break;
                }
                stack.pop(start2, len2);
                int mid = start2 + len2 - 1;
                int right = start1 + len1 - 1;
                merge(athletes, start2, mid, right);
                stack.push(start2, len2 + len1);
            }
        } catch (const runtime_error& e) {
            cout << "Error in TimSort: " << e.what() << endl;
        }
    }

    void syncAthletesArray() {
        athleteCount = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current != NULL && athleteCount < MAX_ENTRIES) {
                if (strcmp(current->sport, "None") != 0) {
                    athletes[athleteCount].id = current->id;
                    strcpy(athletes[athleteCount].name, current->name);
                    strcpy(athletes[athleteCount].sport, current->sport);
                    strcpy(athletes[athleteCount].time, current->time);
                    athleteCount++;
                }
                current = current->next;
            }
        }
    }

public:
    HashTable() : isLoadingFromFile(false), athleteCount(0) {
        try {
            table = new Node*[TABLE_SIZE]();
            initSports();
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed in HashTable constructor!\n";
            throw;
        }
    }

    int getSportCount() const { return SPORT_COUNT; }

    bool getSportByIndex(int index, char* sport, char* time) const {
        if (index < 1 || index > SPORT_COUNT) return false;
        strcpy(sport, sports[index - 1].name);
        strcpy(time, sports[index - 1].time);
        return true;
    }

    // Overloaded insert functions
    bool insert(int id, const char* name, const char* sport, const char* time) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID! Must be a positive integer.");
            if (!idExists(id)) throw runtime_error("ID not registered! Add student first.");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            if (hasTimeConflict(id, time)) throw runtime_error("Time conflict!");
            if (!checkCapacity(sport)) throw runtime_error("Sport capacity full!");

            Node* newNode = new Node(id, name, sport, time);
            int index = hashFunction(id);
            newNode->next = table[index];
            table[index] = newNode;

            syncAthletesArray();
            if (!isLoadingFromFile) {
                cout << "Successfully inserted: ID=" << id << ", Name=" << newNode->name
                     << ", Sport=" << sport << ", Time=" << time << endl;
            }
            return true;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed!\n";
            return false;
        }
    }

    bool insert(int id, const char* sport) { // Overloaded
        char time[50], name[50] = "";
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) {
                strcpy(time, sports[i].time);
                return insert(id, name, sport, time);
            }
        }
        cout << "Error: Sport not found!\n";
        return false;
    }

    bool insertStudent(int id, const char* name) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (idExists(id)) throw runtime_error("ID already exists!");
            char trimmedName[50];
            strcpy(trimmedName, name);
            trimWhitespace(trimmedName);
            if (strlen(trimmedName) == 0) throw runtime_error("Name cannot be empty!");

            Node* newNode = new Node(id, trimmedName, "None", "None");
            int index = hashFunction(id);
            newNode->next = table[index];
            table[index] = newNode;

            if (!isLoadingFromFile) {
                cout << "Successfully added student: ID=" << id << ", Name=" << newNode->name << endl;
            }
            return true;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed!\n";
            return false;
        }
    }

    // Overloaded search functions
    void search(int id) const {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            int index = hashFunction(id);
            Node* current = table[index];
            bool found = false;
            while (current != NULL) {
                if (current->id == id && strcmp(current->sport, "None") != 0) {
                    cout << "Found: ID=" << current->id << ", Name=" << current->name
                         << ", Sport=" << current->sport << ", Time=" << current->time << endl;
                    found = true;
                }
                current = current->next;
            }
            if (!found) throw runtime_error("ID not found or has no sports!");
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void search(const char* name) const { // Overloaded
        try {
            bool found = false;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current != NULL) {
                    if (strcmp(current->name, name) == 0 && strcmp(current->sport, "None") != 0) {
                        cout << "Found: ID=" << current->id << ", Name=" << current->name
                             << ", Sport=" << current->sport << ", Time=" << current->time << endl;
                        found = true;
                    }
                    current = current->next;
                }
            }
            if (!found) throw runtime_error("Name not found or has no sports!");
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void display() const {
        cout << "\n============== HASH TABLE CONTENTS ==============\n";
        cout << "Index | ID    | Name            | Sport         | Time\n";
        cout << "------|-------|-----------------|---------------|------------------\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << right << setw(5) << i << " |";
            Node* current = table[i];
            if (current == NULL) {
                cout << setw(7) << "" << " | Empty\n";
                continue;
            }
            bool first = true;
            while (current != NULL) {
                if (strcmp(current->sport, "None") != 0) {
                    if (!first) cout << "      |";
                    cout << right << setw(7) << current->id << " |"
                         << setw(16) << left << current->name << " |"
                         << setw(14) << current->sport << " |"
                         << current->time << endl;
                    first = false;
                }
                current = current->next;
            }
            if (first) cout << setw(7) << "" << " | Empty\n";
        }
        cout << "================================================\n";
    }

    void displaySorted() {
        syncAthletesArray();
        timSort();
        cout << "\n========== SORTED ATHLETE DATA (BY ID) ==========\n";
        cout << "ID    | Name            | Sport         | Time\n";
        cout << "------|-----------------|---------------|------------------\n";
        if (athleteCount == 0) {
            cout << "No athletes with sports enrolled!\n";
        } else {
            for (int i = 0; i < athleteCount; i++) {
                cout << right << setw(5) << athletes[i].id << " | "
                     << setw(16) << left << athletes[i].name << " | "
                     << setw(14) << athletes[i].sport << " | "
                     << athletes[i].time << endl;
            }
        }
        cout << "================================================\n";
    }

    void displaySports() const {
        cout << "\n============= AVAILABLE SPORTS =============\n";
        cout << "No. | Sport         | Time              | Capacity\n";
        cout << "---|---------------|-------------------|---------|\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            cout << right << setw(3) << (i + 1) << " | "
                 << setw(13) << left << sports[i].name << " | "
                 << setw(17) << sports[i].time << " | "
                 << sports[i].current_count << "/" << sports[i].capacity << endl;
        }
        cout << "==========================================\n";
    }

    void displayFileContents(const char* filename) const {
        ReportManager rm;
        rm.displayReport(filename);
    }

    void displaySportParticipants(const char* sport) const {
        try {
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            cout << "\n======== PARTICIPANTS IN " << sport << " ========\n";
            cout << "ID    | Name            | Time\n";
            cout << "------|-----------------|------------------\n";
            bool found = false;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current != NULL) {
                    if (strcmp(current->sport, sport) == 0) {
                        cout << setw(5) << right << current->id << " | "
                             << setw(15) << left << current->name << " | "
                             << current->time << endl;
                        found = true;
                    }
                    current = current->next;
                }
            }
            if (!found) cout << "No participants found for " << sport << endl;
            cout << "==========================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void displaySportParticipantsSorted(const char* sport) {
        try {
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            syncAthletesArray();
            timSort();
            cout << "\n===== PARTICIPANTS IN " << sport << " (SORTED) =====\n";
            cout << "ID    | Name            | Time\n";
            cout << "------|-----------------|------------------\n";
            bool found = false;
            for (int i = 0; i < athleteCount; i++) {
                if (strcmp(athletes[i].sport, sport) == 0) {
                    cout << right << setw(5) << athletes[i].id << " | "
                         << setw(15) << left << athletes[i].name << " | "
                         << athletes[i].time << endl;
                    found = true;
                }
            }
            if (!found) cout << "No participants found for " << sport << endl;
            cout << "==========================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void displayTimeTable(int id) const {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            int index = hashFunction(id);
            Node* current = table[index];
            bool found = false;
            char name[50] = "Unknown";
            if (findStudentName(id, name)) found = true;
            cout << "\n======== TIME TABLE FOR ID=" << id << " (" << name << ") ========\n";
            cout << "Day | Schedule\n";
            cout << "----|--------------------------------------------------\n";
            const char* days[5] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
            for (int i = 0; i < 5; i++) {
                cout << setw(3) << days[i] << " | ";
                bool hasClass = false;
                struct SportEntry {
                    char sport[50];
                    char time[50];
                };
                SportEntry entries[10];
                int entryCount = 0;
                current = table[index];
                while (current != NULL) {
                    if (current->id == id && strstr(current->time, days[i]) != NULL) {
                        strcpy(entries[entryCount].sport, current->sport);
                        strcpy(entries[entryCount].time, current->time);
                        entryCount++;
                    }
                    current = current->next;
                }
                for (int j = 0; j < entryCount - 1; j++) {
                    for (int k = j + 1; k < entryCount; k++) {
                        if (strcmp(entries[j].time, entries[k].time) > 0) {
                            SportEntry temp = entries[j];
                            entries[j] = entries[k];
                            entries[k] = temp;
                        }
                    }
                }
                for (int j = 0; j < entryCount; j++) {
                    if (hasClass) cout << ", ";
                    cout << entries[j].sport << " (" << entries[j].time + 4 << ")";
                    hasClass = true;
                }
                if (!hasClass) cout << "No Classes";
                cout << endl;
            }
            cout << "=======================================================\n";
            if (!found) cout << "Warning: ID " << id << " not found in system!\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void editStudentName(int id, const char* newName) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (strlen(newName) == 0) throw runtime_error("Name cannot be empty!");
            int index = hashFunction(id);
            Node* current = table[index];
            bool found = false;
            while (current != NULL) {
                if (current->id == id) {
                    strncpy(current->name, newName, 49); current->name[49] = '\0';
                    found = true;
                }
                current = current->next;
            }
            if (!found) throw runtime_error("ID not found!");
            syncAthletesArray();
            cout << "Name updated for ID " << id << endl;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void editSportTime(int id, const char* sport, const char* newTime) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            if (hasTimeConflict(id, newTime)) throw runtime_error("Time conflict!");
            int index = hashFunction(id);
            Node* current = table[index];
            while (current != NULL) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    strncpy(current->time, newTime, 49); current->time[49] = '\0';
                    cout << "Time updated for ID " << id << ", Sport: " << sport << endl;
                    syncAthletesArray();
                    return;
                }
                current = current->next;
            }
            throw runtime_error("Sport not found for ID!");
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void readFromFile(const char* filename) {
        try {
            isLoadingFromFile = true;
            ifstream inFile(filename);
            if (!inFile) {
                // Create file with header if it doesn't exist
                ofstream outFile(filename);
                if (!outFile) throw runtime_error("Cannot create file: " + string(filename));
                outFile << "SPORTS_ALLOTMENT_DATA\n";
                outFile << "ID    | Name             | Sport         | Time\n";
                outFile << "------|------------------|---------------|------------------\n";
                outFile.close();
                cout << "Created new file: " << filename << endl;
                isLoadingFromFile = false;
                return;
            }
            char header[100];
            inFile.getline(header, 100);
            if (strcmp(header, "SPORTS_ALLOTMENT_DATA") != 0) {
                inFile.close();
                throw runtime_error("Invalid file format in " + string(filename));
            }
            int entryCount = 0;
            char line[256];
            while (inFile.getline(line, 256)) {
                if (strstr(line, "==========") || strstr(line, "------|") || line[0] == '\0') continue;
                int id;
                char name[50], sport[50], time[50];
                char* token = strtok(line, "|");
                if (!token) continue;
                trimWhitespace(token);
                id = atoi(token);
                if (!isValidID(id)) continue;
                token = strtok(NULL, "|");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(name, token, 49); name[49] = '\0';
                if (strlen(name) == 0) continue;
                token = strtok(NULL, "|");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(sport, token, 49); sport[49] = '\0';
                token = strtok(NULL, "\n");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(time, token, 49); time[49] = '\0';
                if (!idExists(id)) {
                    insertStudent(id, name);
                    entryCount++;
                }
                if (strcmp(sport, "None") == 0) continue;
                if (insert(id, name, sport, time)) entryCount++;
            }
            inFile.close();
            isLoadingFromFile = false;
            cout << "Loaded " << entryCount << " entries from " << filename << endl;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            isLoadingFromFile = false;
        }
    }

    void loadAllFiles() {
        cout << "Loading from input.txt...\n";
        readFromFile("input.txt");
        cout << "Loading from sorted_information.txt...\n";
        readFromFile("sorted_information.txt");
        cout << "Loading from additional_data.txt...\n";
        readFromFile("additional_data.txt");
    }

    void saveToFile(const char* filename) {
        try {
            ofstream outFile(filename);
            if (!outFile) throw runtime_error("Could not open file for writing: " + string(filename));
            outFile << "SPORTS_ALLOTMENT_DATA\n";
            outFile << "ID    | Name             | Sport         | Time\n";
            outFile << "------|------------------|---------------|------------------\n";
            int ids[MAX_ENTRIES];
            int idCount = 0;
            for (int i = 0; i < TABLE_SIZE && idCount < MAX_ENTRIES; i++) {
                Node* current = table[i];
                while (current != NULL && idCount < MAX_ENTRIES) {
                    ids[idCount++] = current->id;
                    current = current->next;
                }
            }
            // Sort IDs to avoid duplicates
            for (int i = 1; i < idCount; i++) {
                int key = ids[i];
                int j = i - 1;
                while (j >= 0 && ids[j] > key) {
                    ids[j + 1] = ids[j];
                    j--;
                }
                ids[j + 1] = key;
            }
            int newSize = 1;
            for (int i = 1; i < idCount; i++) {
                if (ids[i] != ids[newSize - 1]) ids[newSize++] = ids[i];
            }
            for (int i = 0; i < newSize; i++) {
                int id = ids[i];
                Node* current = table[hashFunction(id)];
                while (current != NULL) {
                    if (current->id == id && strcmp(current->sport, "None") != 0) {
                        outFile << right << setw(5) << current->id << " | "
                                << setw(16) << left << current->name << " | "
                                << setw(13) << left << current->sport << " | "
                                << current->time << endl;
                    }
                    current = current->next;
                }
            }
            outFile << "==================================================\n";
            outFile.close();
            cout << "Data saved to '" << filename << "'\n";
            char reportContent[1000];
            snprintf(reportContent, sizeof(reportContent), "Total Athletes: %d\nSports Enrolled: %d", newSize, athleteCount);
            ReportManager rm;
            rm.generateReport("summary_report.txt", reportContent);
            rm.generateSportStats("sport_stats.txt", SPORT_COUNT, sports);
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void removeSport(int id, const char* sport) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            int index = hashFunction(id);
            Node* current = table[index];
            Node* prev = NULL;
            while (current != NULL) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    reduceCapacity(sport);
                    if (prev == NULL) {
                        table[index] = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    syncAthletesArray();
                    cout << "Successfully removed: ID=" << id << ", Sport=" << sport << endl;
                    delete current;
                    return;
                }
                prev = current;
                current = current->next;
            }
            throw runtime_error("ID with sport not found!");
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current != NULL) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }
};

// Friend function implementations
void printTableSize(const HashTable& ht) {
    cout << "Hash Table Size: " << ht.TABLE_SIZE << endl;
}

void countEntries(const HashTable& ht) {
    int count = 0;
    for (int i = 0; i < ht.TABLE_SIZE; i++) {
        Node* current = ht.table[i];
        while (current != NULL) {
            if (strcmp(current->sport, "None") != 0) count++;
            current = current->next;
        }
    }
    cout << "Total Entries with Sports: " << count << endl;
}

void checkCapacity(const HashTable& ht, const char* sport) {
    for (int i = 0; i < ht.SPORT_COUNT; i++) {
        if (strcmp(sport, ht.sports[i].name) == 0) {
            cout << "Capacity for " << sport << ": " << ht.sports[i].current_count << "/" << ht.sports[i].capacity << endl;
            return;
        }
    }
    cout << "Sport not found!\n";
}

void resetTable(HashTable& ht) {
    for (int i = 0; i < ht.TABLE_SIZE; i++) {
        Node* current = ht.table[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        ht.table[i] = NULL;
    }
    ht.athleteCount = 0;
    for (int i = 0; i < ht.SPORT_COUNT; i++) {
        ht.sports[i].current_count = 0;
    }
    cout << "Hash Table Reset.\n";
}

// Utility functions
int getValidInput(int min, int max) {
    int input;
    while (!(cin >> input) || input < min || input > max) {
        cout << "Invalid input! Enter a number between " << min << " and " << max << ": ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    return input;
}

void getSafeInput(char* buffer, int size) {
    cin.getline(buffer, size);
    char* start = buffer;
    char* end;
    while (isspace(*start)) start++;
    end = start + strlen(start) - 1;
    while (end >= start && isspace(*end)) end--;
    *(end + 1) = '\0';
    if (start != buffer) memmove(buffer, start, strlen(start) + 1);
    if (strlen(buffer) == 0) throw runtime_error("Input cannot be empty!");
}

void waitForUser() {
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

void displayMenu(const char* title, const char* options[], int size) {
    cout << "\n===== " << title << " =====\n";
    for (int i = 0; i < size; i++) {
        cout << i + 1 << ". " << options[i] << endl;
    }
    cout << "0. Exit\n==================\n";
    cout << "Enter your choice: ";
}

int main() {
    cout << "========================================\n";
    cout << "   SPORTS ALLOTMENT SYSTEM\n";
    cout << "========================================\n";

    printWorkingDirectory(); // Debug: Show working directory
    HashTable table;
    LoginManager lm;
    ReportManager rm;
    User* currentUser = new Guest();
    table.loadAllFiles();

    while (true) {
        const char* loginOptions[] = {"Login as Admin", "Login as Customer", "Continue as Guest", "Register New User"};
        displayMenu("LOGIN MENU", loginOptions, 4);
        int loginChoice = getValidInput(0, 4);
        cin.ignore();

        if (loginChoice == 0) break;

        if (loginChoice == 4) {
            cout << "\n--- Register New User ---\n";
            char username[50], password[50], role[20];
            try {
                cout << "Enter Username: ";
                getSafeInput(username, 50);
                cout << "Enter Password: ";
                getSafeInput(password, 50);
                cout << "Enter Role (Admin/Customer): ";
                getSafeInput(role, 20);
                if (strcmp(role, "Admin") != 0 && strcmp(role, "Customer") != 0) {
                    throw runtime_error("Invalid role! Use Admin or Customer.");
                }
                if (lm.registerUser(username, password, role)) {
                    cout << "User registered successfully!\n";
                }
            } catch (const runtime_error& e) {
                cout << "Error: " << e.what() << endl;
            }
            waitForUser();
            continue;
        }

        if (loginChoice == 1) {
            delete currentUser;
            currentUser = new Admin();
            cout << "\n--- Admin Login ---\n";
            char username[50], password[50];
            try {
                cout << "Username: ";
                getSafeInput(username, 50);
                cout << "Password: ";
                getSafeInput(password, 50);
                if (!currentUser->login(username, password)) {
                    cout << "Login failed!\n";
                }
            } catch (const runtime_error& e) {
                cout << "Error: " << e.what() << endl;
            }
            if (strcmp(currentUser->getUsername(), "unknown") == 0) {
                waitForUser();
                continue;
            }
        } else if (loginChoice == 2) {
            delete currentUser;
            currentUser = new Customer();
            cout << "\n--- Customer Login ---\n";
            char username[50], password[50];
            try {
                cout << "Username: ";
                getSafeInput(username, 50);
                cout << "Password: ";
                getSafeInput(password, 50);
                if (!currentUser->login(username, password)) {
                    cout << "Login failed!\n";
                }
            } catch (const runtime_error& e) {
                cout << "Error: " << e.what() << endl;
            }
            if (strcmp(currentUser->getUsername(), "unknown") == 0) {
                waitForUser();
                continue;
            }
        } else {
            delete currentUser;
            currentUser = new Guest();
            currentUser->login("", "");
        }

        cout << "Logged in as " << currentUser->getRole() << ": " << currentUser->getUsername() << endl;

        while (true) {
            const char* mainOptions[] = {"Staff/Admin View", "Customer View", "Display Summary Report", "Logout"};
            displayMenu("MAIN MENU", mainOptions, 4);
            int choice = getValidInput(0, 4);
            cin.ignore();

            if (choice == 0) break;
            if (choice == 4) {
                currentUser->logout();
                break;
            }
            if (choice == 3) {
                rm.displayReport("summary_report.txt");
                waitForUser();
                continue;
            }

            if (choice == 1 && strcmp(currentUser->getRole(), "Guest") == 0) {
                cout << "Access denied! Admin login required.\n";
                waitForUser();
                continue;
            }

            if (choice == 1) {
                while (true) {
                    const char* adminOptions[] = {
                        "Add New Student",
                        "Manage Sport Selection",
                        "Edit Student Name",
                        "Edit Sport Time",
                        "Search Student by ID",
                        "Search Student by Name",
                        "Display All Data",
                        "Display Sport Information",
                        "Display Student Time Table",
                        "Save Data",
                        "Display File Contents"
                    };
                    displayMenu("STAFF/ADMIN VIEW", adminOptions, 11);
                    int subChoice = getValidInput(0, 11);
                    cin.ignore();

                    if (subChoice == 0) break;

                    if (subChoice == 1) {
                        cout << "\n--- Add New Student ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter Student Name: ";
                            char name[50];
                            getSafeInput(name, 50);
                            table.insertStudent(id, name);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 2) {
                        cout << "\n--- Manage Sport Selection ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cout << "\n1. Add Sport\n2. Remove Sport\nEnter action (1-2): ";
                            int action = getValidInput(1, 2);
                            cin.ignore();
                            if (action == 1) {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(1, table.getSportCount());
                                cin.ignore();
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.insert(id, "", sport, time);
                                }
                            } else {
                                cout << "Enter Sport Name to Remove: ";
                                char sportName[50];
                                getSafeInput(sportName, 50);
                                table.removeSport(id, sportName);
                            }
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 3) {
                        cout << "\n--- Edit Student Name ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter New Name: ";
                            char newName[50];
                            getSafeInput(newName, 50);
                            table.editStudentName(id, newName);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 4) {
                        cout << "\n--- Edit Sport Time ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter Sport Name: ";
                            char sport[50];
                            getSafeInput(sport, 50);
                            cout << "Enter New Time (e.g., Mon 10:00-12:00): ";
                            char newTime[50];
                            getSafeInput(newTime, 50);
                            table.editSportTime(id, sport, newTime);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 5) {
                        cout << "\n--- Search Student by ID ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.search(id);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 6) {
                        cout << "\n--- Search Student by Name ---\n";
                        try {
                            cout << "Enter Student Name: ";
                            char name[50];
                            getSafeInput(name, 50);
                            table.search(name);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 7) {
                        cout << "\n--- Display Options ---\n";
                        cout << "1. Display Hash Table Contents\n";
                        cout << "2. Display Sport Participants\n";
                        cout << "Enter choice (1-2): ";
                        int displayChoice = getValidInput(1, 2);
                        if (displayChoice == 1) {
                            table.display();
                        } else {
                            table.displaySports();
                            cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                            int sportIndex = getValidInput(1, table.getSportCount());
                            cin.ignore();
                            char sport[50], time[50];
                            if (table.getSportByIndex(sportIndex, sport, time)) {
                                table.displaySportParticipants(sport);
                            }
                        }
                        waitForUser();
                    } else if (subChoice == 8) {
                        table.displaySports();
                        waitForUser();
                    } else if (subChoice == 9) {
                        cout << "\n--- Student Time Table ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.displayTimeTable(id);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 10) {
                        cout << "Save to (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt? Enter 1-3: ";
                        int saveChoice = getValidInput(1, 3);
                        cin.ignore();
                        const char* filename;
                        if (saveChoice == 1) filename = "input.txt";
                        else if (saveChoice == 2) filename = "sorted_information.txt";
                        else filename = "additional_data.txt";
                        table.saveToFile(filename);
                        waitForUser();
                    } else if (subChoice == 11) {
                        cout << "Display (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt, "
                             << "(4) summary_report.txt, (5) login_credentials.txt? Enter 1-5: ";
                        int fileChoice = getValidInput(1, 5);
                        cin.ignore();
                        const char* filename;
                        if (fileChoice == 1) filename = "input.txt";
                        else if (fileChoice == 2) filename = "sorted_information.txt";
                        else if (fileChoice == 3) filename = "additional_data.txt";
                        else if (fileChoice == 4) filename = "summary_report.txt";
                        else filename = "login_credentials.txt";
                        table.displayFileContents(filename);
                        waitForUser();
                    }
                }
            } else if (choice == 2) {
                while (true) {
                    const char* customerOptions[] = {
                        "Add New Student",
                        "Manage Sport Selection",
                        "Edit Student Name",
                        "Edit Sport Time",
                        "Search Student by Name",
                        "Display Sorted Data",
                        "Display Sport Participants (Sorted)",
                        "Display Student Time Table",
                        "Display Available Sports",
                        "Save Data",
                        "Display File Contents"
                    };
                    displayMenu("CUSTOMER VIEW", customerOptions, 11);
                    int subChoice = getValidInput(0, 11);
                    cin.ignore();

                    if (subChoice == 0) break;

                    if (subChoice == 1) {
                        cout << "\n--- Add New Student ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter Student Name: ";
                            char name[50];
                            getSafeInput(name, 50);
                            table.insertStudent(id, name);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 2) {
                        cout << "\n--- Manage Sport Selection ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cout << "\n1. Add Sport\n2. Remove Sport\nEnter action (1-2): ";
                            int action = getValidInput(1, 2);
                            cin.ignore();
                            if (action == 1) {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(1, table.getSportCount());
                                cin.ignore();
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.insert(id, "", sport, time);
                                }
                            } else {
                                cout << "Enter Sport Name to Remove: ";
                                char sportName[50];
                                getSafeInput(sportName, 50);
                                table.removeSport(id, sportName);
                            }
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 3) {
                        cout << "\n--- Edit Student Name ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter New Name: ";
                            char newName[50];
                            getSafeInput(newName, 50);
                            table.editStudentName(id, newName);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 4) {
                        cout << "\n--- Edit Sport Time ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter Sport Name: ";
                            char sport[50];
                            getSafeInput(sport, 50);
                            cout << "Enter New Time (e.g., Mon 10:00-12:00): ";
                            char newTime[50];
                            getSafeInput(newTime, 50);
                            table.editSportTime(id, sport, newTime);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 5) {
                        cout << "\n--- Search Student by Name ---\n";
                        try {
                            cout << "Enter Student Name: ";
                            char name[50];
                            getSafeInput(name, 50);
                            table.search(name);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 6) {
                        cout << "\n--- Sorted Data ---\n";
                        table.displaySorted();
                        waitForUser();
                    } else if (subChoice == 7) {
                        cout << "\n--- Sport Participants (Sorted) ---\n";
                        table.displaySports();
                        try {
                            cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                            int sportIndex = getValidInput(1, table.getSportCount());
                            cin.ignore();
                            char sport[50], time[50];
                            if (table.getSportByIndex(sportIndex, sport, time)) {
                                table.displaySportParticipantsSorted(sport);
                            }
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 8) {
                        cout << "\n--- Student Time Table ---\n";
                        try {
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.displayTimeTable(id);
                        } catch (const runtime_error& e) {
                            cout << "Error: " << e.what() << endl;
                        }
                        waitForUser();
                    } else if (subChoice == 9) {
                        cout << "\n--- Available Sports ---\n";
                        table.displaySports();
                        waitForUser();
                    } else if (subChoice == 10) {
                        cout << "Save to (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt? Enter 1-3: ";
                        int saveChoice = getValidInput(1, 3);
                        cin.ignore();
                        const char* filename;
                        if (saveChoice == 1) filename = "input.txt";
                        else if (saveChoice == 2) filename = "sorted_information.txt";
                        else filename = "additional_data.txt";
                        table.saveToFile(filename);
                        waitForUser();
                    } else if (subChoice == 11) {
                        cout << "Display (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt, "
                             << "(4) summary_report.txt, (5) login_credentials.txt? Enter 1-5: ";
                        int fileChoice = getValidInput(1, 5);
                        cin.ignore();
                        const char* filename;
                        if (fileChoice == 1) filename = "input.txt";
                        else if (fileChoice == 2) filename = "sorted_information.txt";
                        else if (fileChoice == 3) filename = "additional_data.txt";
                        else if (fileChoice == 4) filename = "summary_report.txt";
                        else filename = "login_credentials.txt";
                        table.displayFileContents(filename);
                        waitForUser();
                    }
                }
            }
        }
    }

    delete currentUser;
    cout << "\nProgram terminated. Thank you for using the Sports Allotment System!\n";
    return 0;
}
