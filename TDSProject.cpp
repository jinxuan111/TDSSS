#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <cerrno>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

// Function to trim whitespace
void trimWhitespace(char* str) {
    char* start = str;
    while (isspace(*start)) start++;
    char* end = start + strlen(start) - 1;
    while (end >= start && isspace(*end)) *end-- = '\0';
    memmove(str, start, strlen(start) + 1);
}

// Function to print working directory
void printWorkingDirectory() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        cout << "Current working directory: " << cwd << endl;
    } else {
        cout << "Error: Cannot get current working directory!" << endl;
    }
}

// Structure for linked list node
struct Node {
    int id;
    char name[50];
    char sport[50];
    char time[50];
    Node* next;
    Node(int i, const char* n, const char* s, const char* t) : id(i), next(NULL) {
        strncpy(name, n, 49); name[49] = '\0';
        strncpy(sport, s, 49); sport[49] = '\0';
        strncpy(time, t, 49); time[49] = '\0';
    }
};

// Structure for sport details
struct Sport {
    char name[50];
    char time[50];
    int capacity;
    int current_count;
};

// Structure for athlete data
struct Athlete {
    int id;
    char name[50];
    char sport[50];
    char time[50];
};

// Stack for TimSort
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
    Record(int i) : id(i) {}
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
    char role[20];
public:
    User() {
        strcpy(username, "unknown");
        strcpy(role, "Guest");
    }
    virtual bool login(const char* user, const char* pass) = 0;
    virtual void logout() = 0;
    virtual ~User() {}
    const char* getUsername() const { return username; }
    const char* getRole() const { return role; }
};

// Derived class: Admin
class Admin : public User {
public:
    Admin() { strcpy(role, "Admin"); }
    bool login(const char* user, const char* pass) {
        try {
            ifstream inFile("users.txt");
            if (!inFile) {
                perror("Error opening users.txt for reading");
                throw runtime_error("Cannot open users.txt");
            }
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
};

// Derived class: Customer
class Customer : public User {
public:
    Customer() { strcpy(role, "Customer"); }
    bool login(const char* user, const char* pass) {
        try {
            ifstream inFile("users.txt");
            if (!inFile) {
                perror("Error opening users.txt for reading");
                throw runtime_error("Cannot open users.txt");
            }
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
};

// Derived class: Guest
class Guest : public User {
public:
    Guest() { strcpy(role, "Guest"); }
    bool login(const char* user, const char* pass) {
        strcpy(username, "guest");
        return true;
    }
    void logout() {
        cout << "Guest logged out.\n";
        strcpy(username, "unknown");
    }
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
    Credential* credentials;
    int userCount;

    // Disable copying
    LoginManager(const LoginManager&);
    LoginManager& operator=(const LoginManager&);

    void loadCredentials() {
        try {
            ifstream inFile("users.txt");
            if (!inFile) {
                cout << "users.txt not found, creating new file...\n";
                ofstream outFile("users.txt");
                if (!outFile) {
                    perror("Error creating users.txt");
                    throw runtime_error("Cannot create users.txt: " + string(strerror(errno)));
                }
                outFile << "admin1 pass1 Admin\ncustomer1 pass2 Customer\n";
                outFile.close();
                cout << "Created users.txt with default credentials\n";
            } else {
                userCount = 0;
                while (inFile >> credentials[userCount].username >> credentials[userCount].password >> credentials[userCount].role) {
                    if (userCount < MAX_USERS) userCount++;
                }
                inFile.close();
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

public:
    LoginManager() : userCount(0) {
        credentials = new Credential[MAX_USERS];
        loadCredentials();
    }
    bool registerUser(const char* user, const char* pass, const char* role) {
        try {
            if (strlen(user) == 0 || strlen(pass) == 0) throw runtime_error("Username or password cannot be empty!");
            if (strcmp(role, "Admin") != 0 && strcmp(role, "Customer") != 0) throw runtime_error("Invalid role!");
            for (int i = 0; i < userCount; i++) {
                if (strcmp(credentials[i].username, user) == 0) throw runtime_error("Username already exists!");
            }
            ofstream outFile("users.txt", ios::app);
            if (!outFile) {
                perror("Error opening users.txt for writing");
                throw runtime_error("Cannot open users.txt for writing: " + string(strerror(errno)));
            }
            outFile << user << " " << pass << " " << role << endl;
            outFile.close();
            loadCredentials();
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

// Main HashTable class
class HashTable {
private:
    friend class ReportManager;
    static const int TABLE_SIZE = 100;
    Node** table;
    static const int SPORT_COUNT = 8;
    Sport sports[SPORT_COUNT];
    bool isLoadingFromFile;
    static const int MAX_ENTRIES = 100;
    Athlete athletes[MAX_ENTRIES];
    int athleteCount;

    friend void printTableSize(const HashTable& ht);
    friend void countEntries(const HashTable& ht);
    friend void checkCapacity(const HashTable& ht, const char* sport);
    friend void resetTable(HashTable& ht);

    int hashFunction(int id) const { return id % TABLE_SIZE; }

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

    bool hasTimeConflict(int id, const char* time) const {
        if (strcmp(time, "None") == 0) return true;
        int index = hashFunction(id);
        Node* current = table[index];
        while (current) {
            if (current->id == id && strcmp(current->time, time) == 0 && strcmp(current->sport, "None") != 0)
                return false;
            current = current->next;
        }
        return true;
    }

    bool checkCapacity(const char* sport) {
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) {
                if (sports[i].current_count < sports[i].capacity) {
                    if (!isLoadingFromFile) sports[i].current_count++;
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
        while (current) {
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
        while (current) {
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

    bool isValidID(int id) const { return id > 0 && id <= 99999; }

    // Insertion sort for Athlete array
    void insertionSort(Athlete arr[], int size) {
        for (int i = 1; i < size; i++) {
            Athlete temp = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j].id > temp.id) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = temp;
        }
    }

    // Insertion sort for integer array
    void insertionSort(int arr[], int size) {
        for (int i = 1; i < size; i++) {
            int temp = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > temp) {
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
                if (leftArr[i].id <= rightArr[j].id) arr[k++] = leftArr[i++];
                else arr[k++] = rightArr[j++];
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
        if (athleteCount <= 1) return;
        const int RUN = 32;
        Stack stack;
        try {
            for (int i = 0; i < athleteCount; i += RUN) {
                int start = i;
                int end = min(i + RUN - 1, athleteCount - 1);
                insertionSort(athletes + start, end - start + 1);
                stack.push(start, end - start + 1);
            }
            while (!stack.isEmpty()) {
                int start1, len1;
                stack.pop(start1, len1);
                if (stack.isEmpty()) {
                    stack.push(start1, len1);
                    break;
                }
                int start2, len2;
                stack.pop(start2, len2);
                int mid = start2 + len2 - 1;
                int right = start1 + len1 - 1;
                merge(athletes, start2, mid, right);
                stack.push(start2, len2 + len1);
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void syncAthletesArray() {
        athleteCount = 0;
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current && athleteCount < MAX_ENTRIES) {
                athletes[athleteCount].id = current->id;
                strcpy(athletes[athleteCount].name, current->name);
                strcpy(athletes[athleteCount].sport, current->sport);
                strcpy(athletes[athleteCount].time, current->time);
                athleteCount++;
                current = current->next;
            }
        }
    }

public:
    HashTable() : isLoadingFromFile(false), athleteCount(0) {
        table = new Node*[TABLE_SIZE]();
        initSports();
    }

    int getSportCount() const { return SPORT_COUNT; }

    bool getSportByIndex(int index, char* sport, char* time) const {
        if (index < 1 || index > SPORT_COUNT) return false;
        strcpy(sport, sports[index - 1].name);
        strcpy(time, sports[index - 1].time);
        return true;
    }

    bool insert(int id, const char* name, const char* sport, const char* time) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (!idExists(id)) throw runtime_error("ID not registered!");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");

            // Check for duplicate sport
            int index = hashFunction(id);
            Node* current = table[index];
            while (current) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    if (isLoadingFromFile) return true;
                    throw runtime_error("Student already enrolled in this sport!");
                }
                current = current->next;
            }

            if (!hasTimeConflict(id, time)) throw runtime_error("Time conflict!");
            if (!checkCapacity(sport)) throw runtime_error("Sport capacity full!");

            Node* newNode = new Node(id, name, sport, time);
            newNode->next = table[index];
            table[index] = newNode;

            syncAthletesArray();
            if (!isLoadingFromFile) {
                cout << "Inserted: ID=" << id << ", Name=" << name << ", Sport=" << sport << ", Time=" << time << endl;
            }
            return true;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed!" << endl;
            return false;
        }
    }

    bool insert(int id, const char* sport) {
        char time[50], name[50];
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) {
                strcpy(time, sports[i].time);
                if (!findStudentName(id, name)) strcpy(name, "Unknown");
                return insert(id, name, sport, time);
            }
        }
        cout << "Error: Sport not found!\n";
        return false;
    }

    bool insertStudent(int id, const char* name) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            char trimmedName[50];
            strcpy(trimmedName, name);
            trimWhitespace(trimmedName);
            if (strlen(trimmedName) == 0) throw runtime_error("Name cannot be empty!");

            if (idExists(id)) {
                if (isLoadingFromFile) {
                    int index = hashFunction(id);
                    Node* current = table[index];
                    while (current) {
                        if (current->id == id && strcmp(current->name, trimmedName) != 0) {
                            cout << "Updating name for ID " << id << " from '" << current->name << "' to '" << trimmedName << "'.\n";
                            strncpy(current->name, trimmedName, 49);
                            current->name[49] = '\0';
                        }
                        current = current->next;
                    }
                    return true;
                }
                throw runtime_error("ID already exists!");
            }

            Node* newNode = new Node(id, trimmedName, "None", "None");
            int index = hashFunction(id);
            newNode->next = table[index];
            table[index] = newNode;

            syncAthletesArray();
            if (!isLoadingFromFile) cout << "Added student: ID=" << id << ", Name=" << trimmedName << endl;
            return true;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            return false;
        } catch (const bad_alloc& e) {
            cout << "Error: Memory allocation failed!" << endl;
            return false;
        }
    }

    void search(int id) const {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            int index = hashFunction(id);
            Node* current = table[index];
            bool found = false;
            while (current) {
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

    void search(const char* name) const {
        try {
            bool found = false;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current) {
                    if (strcmp(current->name, name) == 0 && strcmp(current->sport, "None") != 0) {
                        cout << "Found: ID=" << current->id << ", Name=" << name
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
        cout << "\n==== HASH TABLE =========\n";
        cout << "Index | ID    | Name            | Sport         | Time\n";
        cout << "------|-------|-----------------|---------------|------------------\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << right << setw(5) << i << " |";
            Node* current = table[i];
            if (!current) {
                cout << " Empty\n";
                continue;
            }
            bool first = true;
            while (current) {
                if (!first) cout << "      |";
                cout << right << setw(5) << current->id << " | "
                     << left << setw(15) << current->name << " | "
                     << setw(13) << current->sport << " | "
                     << current->time << endl;
                first = false;
                current = current->next;
            }
        }
        cout << "==============================================\n";
    }

    void displaySorted() {
        syncAthletesArray();
        timSort();
        cout << "\n===== SORTED ATHLETE DATA (BY ID) ==========\n";
        cout << "ID    | Name            | Sport         | Time\n";
        cout << "------|-----------------|---------------|------------------\n";
        if (athleteCount == 0) {
            cout << "No athletes listed!\n";
        } else {
            for (int i = 0; i < athleteCount; i++) {
                if (strcmp(athletes[i].sport, "None") != 0) {
                    cout << right << setw(5) << athletes[i].id << " | "
                         << left << setw(15) << athletes[i].name << " | "
                         << setw(13) << athletes[i].sport << " | "
                         << athletes[i].time << endl;
                }
            }
        }
        cout << "==============================================\n";
    }

    void displaySports() const {
        cout << "\n========= AVAILABLE SPORTS =========\n";
        cout << "ID | Sport         | Time            | Enrolled/Capacity\n";
        cout << "---|---------------|-----------------|------------------\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            cout << right << setw(2) << (i + 1) << " | "
                 << left << setw(13) << sports[i].name << " | "
                 << setw(15) << sports[i].time << " | "
                 << sports[i].current_count << "/" << sports[i].capacity << endl;
        }
        cout << "====================================\n";
    }

    void displayFileContents(const char* filename) const {
        try {
            ifstream inFile(filename);
            if (!inFile) throw runtime_error("Cannot open " + string(filename));
            cout << "\n===== Content of " << filename << " =====\n\n";
            string line;
            while (getline(inFile, line)) {
                cout << line << "\n";
            }
            inFile.close();
            cout << "\n============\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << "\n";
        } catch (...) {
            cout << "Unexpected error occurred while displaying file contents.\n";
        }
    }

    void displaySportParticipants(const char* sport) const {
        try {
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            cout << "\n===== PARTICIPANTS IN " << sport << " =====\n";
            cout << "ID    | Name            | Time\n";
            cout << "------|-----------------|------------------\n";
            bool found = false;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current != NULL) {
                    if (strcmp(current->sport, sport) == 0) {
                        cout << right << setw(5) << current->id << " | "
                             << left << setw(15) << current->name << " | "
                             << current->time << "\n";
                        found = true;
                    }
                    current = current->next;
                }
            }
            if (!found) cout << "\nNo participants found for " << sport << "\n";
            cout << "=====================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << "\n";
        } catch (...) {
            cout << "Unexpected error occurred while displaying sport participants.\n";
        }
    }

    void displaySportParticipantsSorted(const char* sport) {
        try {
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            syncAthletesArray();
            timSort();
            cout << "\n===== PARTICIPANTS IN " << sport << " (sorted by ID) =====\n";
            cout << "ID    | Name            | Time\n";
            cout << "------|-----------------|------------------\n";
            bool found = false;
            for (int i = 0; i < athleteCount; i++) {
                if (strcmp(athletes[i].sport, sport) == 0) {
                    cout << right << setw(5) << athletes[i].id << " | "
                         << left << setw(15) << athletes[i].name << " | "
                         << athletes[i].time << endl;
                    found = true;
                }
            }
            if (!found) cout << "\nNo participants found for " << sport << "\n";
            cout << "=====================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while displaying sorted sport participants.\n";
        }
    }

    void displayTimeTable(int id) const {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            char name[50] = "Unknown";
            bool found = findStudentName(id, name);
            cout << "\n==== TIME TABLE FOR ID=" << id << " (" << name << ") =====\n";
            cout << "Day | Schedule\n";
            cout << "----|-------------------------------------\n";
            const char* days[5] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
            for (int i = 0; i < 5; i++) {
                cout << left << setw(3) << days[i] << " | ";
                int index = hashFunction(id);
                Node* current = table[index];
                bool hasClass = false;
                struct SportEntry {
                    char sport[50];
                    char time[50];
                };
                SportEntry entries[10];
                int entryCount = 0;
                while (current != NULL) {
                    if (current->id == id && strstr(current->time, days[i]) && strcmp(current->sport, "None") != 0) {
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
                    cout << entries[j].sport << " (" << (entries[j].time + 4) << ")";
                    hasClass = true;
                }
                if (!hasClass) cout << "No Classes\n";
                cout << endl;
            }
            cout << "=========================================\n";
            if (!found) cout << "Warning: ID " << id << " not found!\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while displaying timetable.\n";
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
                    strncpy(current->name, newName, 49);
                    current->name[49] = '\0';
                    found = true;
                }
                current = current->next;
            }
            if (!found) throw runtime_error("ID not found!");
            syncAthletesArray();
            cout << "Name updated successfully for ID " << id << endl;
        } catch (...) {
            cout << "Error updating student name for ID " << id << endl;
        }
    }

    void editSportTime(int id, const char* sport, const char* newTime) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            if (!hasTimeConflict(id, newTime)) throw runtime_error("Time conflict!");
            int index = hashFunction(id);
            Node* current = table[index];
            bool found = false;
            while (current != NULL) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    strncpy(current->time, newTime, 49);
                    current->time[49] = '\0';
                    found = true;
                    break;
                }
                current = current->next;
            }
            if (!found) throw runtime_error("Sport not found for ID!");
            syncAthletesArray();
            cout << "Time updated successfully for ID " << id << ", Sport: " << sport << endl;
        } catch (...) {
            cout << "Error updating sport time for ID " << id << ", Sport: " << sport << endl;
        }
    }

    void readFromFile(const char* filename) {
        try {
            isLoadingFromFile = true;
            ifstream inFile(filename);
            if (!inFile) {
                ofstream outFile(filename);
                if (!outFile) {
                    perror("Error creating file");
                    throw runtime_error("Cannot create file: " + string(filename));
                }
                outFile << "SPORTS_ALLOTMENT_DATA\nID    | Name             | Sport         | Time\n------|------------------|---------------|------------------\n";
                outFile.close();
                cout << "Created new file: " << filename << endl;
                isLoadingFromFile = false;
                return;
            }
            char header[100];
            inFile.getline(header, 100);
            if (strcmp(header, "SPORTS_ALLOTMENT_DATA") != 0) throw runtime_error("Invalid header in " + string(filename));
            int entryCount = 0, lineNum = 1;
            char line[256];
            while (inFile.getline(line, 256)) {
                ++lineNum;
                if (strstr(line, "==========") || strstr(line, "------|") || line[0] == '\0') continue;
                int id;
                char name[50], sport[50], time[50];
                char* token = strtok(line, "|");
                if (!token) { cout << "Warning: Skipping invalid line " << lineNum << " in " << filename << endl; continue; }
                trimWhitespace(token);
                id = atoi(token);
                if (!isValidID(id)) { cout << "Warning: Invalid ID at line " << lineNum << endl; continue; }
                token = strtok(NULL, "|");
                if (!token) { cout << "Warning: Missing Name at line " << lineNum << endl; continue; }
                trimWhitespace(token);
                strncpy(name, token, 49); name[49] = '\0';
                if (strlen(name) == 0) { cout << "Warning: Empty Name at line " << lineNum << endl; continue; }
                token = strtok(NULL, "|");
                if (!token) { cout << "Warning: Missing Sport at line " << lineNum << endl; continue; }
                trimWhitespace(token);
                strncpy(sport, token, 49); sport[49] = '\0';
                token = strtok(NULL, "\n");
                if (!token) { cout << "Warning: Missing Time at line " << lineNum << endl; continue; }
                trimWhitespace(token);
                strncpy(time, token, 49); time[49] = '\0';
                if (!idExists(id)) {
                    insertStudent(id, name);
                    ++entryCount;
                }
                if (strcmp(sport, "None") == 0) continue;
                if (insert(id, name, sport, time)) ++entryCount;
            }
            inFile.close();
            isLoadingFromFile = false;
            cout << "Loaded " << entryCount << " entries from " << filename << endl;
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
            isLoadingFromFile = false;
        } catch (...) {
            cout << "Unexpected error occurred while reading from file.\n";
            isLoadingFromFile = false;
        }
    }

    void loadAllFiles() {
        cout << "Loading input.txt...\n";
        readFromFile("input.txt");
        cout << "Loading sorted_information.txt...\n";
        readFromFile("sorted_information.txt");
        cout << "Loading additional_data.txt...\n";
        readFromFile("additional_data.txt");
        cout << "Sport capacities after loading:\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            cout << sports[i].name << ": " << sports[i].current_count << "/" << sports[i].capacity << endl;
        }
    }

    void saveToFile(const char* filename) {
        try {
            ofstream outFile(filename);
            if (!outFile) {
                perror("Error opening file for writing");
                throw runtime_error("Could not open " + string(filename));
            }
            outFile << "SPORTS_ALLOTMENT_DATA\n";
            outFile << "ID    | Name             | Sport         | Time\n";
            outFile << "------|------------------|---------------|------------------\n";
            int ids[MAX_ENTRIES];
            int idCount = 0;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current && idCount < MAX_ENTRIES) {
                    ids[idCount++] = current->id;
                    current = current->next;
                }
            }
            insertionSort(ids, idCount);
            int uniqueIdCount = 1;
            for (int i = 1; i < idCount; i++) {
                if (ids[i] != ids[uniqueIdCount - 1]) ids[uniqueIdCount++] = ids[i];
            }
            for (int i = 0; i < uniqueIdCount; i++) {
                int id = ids[i];
                Node* current = table[hashFunction(id)];
                while (current) {
                    if (current->id == id) {
                        outFile << left << setw(5) << current->id << " | "
                                << setw(16) << current->name << " | "
                                << setw(15) << current->sport << " | "
                                << current->time << endl;
                    }
                    current = current->next;
                }
            }
            outFile.close();
            cout << "Data saved to " << filename << "\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while saving to file.\n";
        }
    }

    void removeSport(int id, const char* sport) {
        try {
            if (!isValidID(id)) throw runtime_error("Invalid ID!");
            if (!isValidSport(sport)) throw runtime_error("Invalid sport!");
            int index = hashFunction(id);
            Node* current = table[index];
            Node* prev = NULL;
            while (current) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    reduceCapacity(sport);
                    if (prev) prev->next = current->next;
                    else table[index] = current->next;
                    delete current;
                    syncAthletesArray();
                    cout << "Removed: ID=" << id << ", Sport=" << sport << endl;
                    return;
                }
                prev = current;
                current = current->next;
            }
            throw runtime_error("Sport not found for ID!");
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while removing sport.\n";
        }
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }
};

// Class to manage reports
class ReportManager {
private:
    static const int MAX_REPORTS = 100;
    struct Report {
        char filename[100];
        char content[1000];
    };
    Report* reports;
    int reportCount;
    HashTable& hashTable;

    // Disable copying
    ReportManager(const ReportManager&);
    ReportManager& operator=(const ReportManager&);

    int getTotalAthletes() const {
        int count = 0;
        bool used[10000] = {false};
        for (int i = 0; i < hashTable.TABLE_SIZE; i++) {
            Node* current = hashTable.table[i];
            while (current) {
                if (!used[current->id]) {
                    used[current->id] = true;
                    ++count;
                }
                current = current->next;
            }
        }
        return count;
    }

    int getSportsEnrolled() const {
        int count = 0;
        for (int i = 0; i < hashTable.SPORT_COUNT; i++) {
            if (hashTable.sports[i].current_count > 0) ++count;
        }
        return count;
    }

public:
    ReportManager(HashTable& ht) : hashTable(ht), reportCount(0) {
        reports = new Report[MAX_REPORTS];
    }
    void generateReport(const char* filename) {
        try {
            ofstream outFile(filename);
            if (!outFile) {
                perror("Error opening report file");
                throw runtime_error("Cannot open " + string(filename));
            }
            int totalAthletes = getTotalAthletes();
            int sportsEnrolled = getSportsEnrolled();
            outFile << "Total Athletes Enrolled: " << totalAthletes << "\nSports Enrolled: " << sportsEnrolled << endl;
            outFile.close();
            if (reportCount < MAX_REPORTS) {
                strncpy(reports[reportCount].filename, filename, 99);
                snprintf(reports[reportCount].content, 999, "Total athletes: %d\nSports Enrolled: %d", totalAthletes, sportsEnrolled);
                ++reportCount;
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while generating report.\n";
        }
    }

    void generateSportStats() {
        try {
            ofstream outFile("sport_stats.txt");
            if (!outFile) {
                perror("Error opening sport_stats.txt");
                throw runtime_error("Cannot open sport_stats.txt");
            }
            outFile << "===== Sport Enrollment Statistics =====\n";
            outFile << "Sport          | Time              | Enrolled/Capacity\n";
            outFile << "---------------|-------------------|------------------\n";
            for (int i = 0; i < hashTable.SPORT_COUNT; i++) {
                outFile << left << setw(14) << hashTable.sports[i].name << " | "
                        << setw(17) << hashTable.sports[i].time << " | "
                        << hashTable.sports[i].current_count << "/" << hashTable.sports[i].capacity << endl;
            }
            outFile.close();
            if (reportCount < MAX_REPORTS) {
                strncpy(reports[reportCount].filename, "sport_stats.txt", 99);
                strncpy(reports[reportCount].content, "Generated sport statistics.", 999);
                ++reportCount;
            }
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while generating sport stats.\n";
        }
    }

    void displayReport(const char* filename) const {
        try {
            ifstream inFile(filename);
            if (!inFile) {
                perror("Error opening report file for reading");
                throw runtime_error("Cannot open " + string(filename));
            }
            cout << "\n===== Contents of " << filename << " =====\n";
            string line;
            while (getline(inFile, line)) {
                cout << line << endl;
            }
            inFile.close();
            cout << "====================================\n";
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (...) {
            cout << "Unexpected error occurred while displaying report.\n";
        }
    }

    ~ReportManager() {
        delete[] reports;
    }
};

// Friend functions
void printTableSize(const HashTable& ht) {
    cout << "Hash Table Size: " << ht.TABLE_SIZE << endl;
}

void countEntries(const HashTable& ht) {
    int count = 0;
    for (int i = 0; i < ht.TABLE_SIZE; i++) {
        Node* current = ht.table[i];
        while (current) {
            if (strcmp(current->sport, "None") != 0) ++count;
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
        while (current) {
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
    trimWhitespace(buffer);
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

    printWorkingDirectory();
    HashTable table;
    LoginManager lm;
    ReportManager rm(table);
    User* currentUser = new Guest();
    table.loadAllFiles();

    while (true) {
        const char* loginOptions[] = {"Login as Admin", "Login as Customer", "Continue as Guest", "Register New User"};
        displayMenu("LOGIN MENU", loginOptions, 4);
        int loginChoice = getValidInput(0, 4);
        cin.ignore();

        if (loginChoice == 0) break;

        if (loginChoice == 4) {
            cout << "\n=== Register New User ===\n";
            char username[50], password[50], role[20];
            try {
                cout << "Enter Username: ";
                getSafeInput(username, 50);
                cout << "Enter Password: ";
                getSafeInput(password, 50);
                cout << "Enter Role (Admin/Customer): ";
                getSafeInput(role, 20);
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
                cout << "Enter Username: ";
                getSafeInput(username, 50);
                cout << "Enter Password: ";
                getSafeInput(password, 50);
                if (!currentUser->login(username, password)) {
                    cout << "Login failed!\n";
                    delete currentUser;
                    currentUser = new Guest();
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
                cout << "Enter Username: ";
                getSafeInput(username, 50);
                cout << "Enter Password: ";
                getSafeInput(password, 50);
                if (!currentUser->login(username, password)) {
                    cout << "Login failed!\n";
                    delete currentUser;
                    currentUser = new Guest();
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
                rm.displayReport("sport_stats.txt");
                waitForUser();
                continue;
            }

            if (choice == 1 && strcmp(currentUser->getRole(), "Admin") != 0) {
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
                                    table.insert(id, sport);
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
                        cout << "1. Display Hash Table Contents\n2. Display Sport Participants\n";
                        cout << "Enter choice (1-2): ";
                        int displayChoice = getValidInput(1, 2);
                        cin.ignore();
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
                        rm.generateReport("summary_report.txt");
                        rm.generateSportStats();
                        waitForUser();
                    } else if (subChoice == 11) {
                        cout << "Display (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt, "
                             << "(4) summary_report.txt, (5) sport_stats.txt? Enter 1-5: ";
                        int fileChoice = getValidInput(1, 5);
                        cin.ignore();
                        const char* filename;
                        if (fileChoice == 1) filename = "input.txt";
                        else if (fileChoice == 2) filename = "sorted_information.txt";
                        else if (fileChoice == 3) filename = "additional_data.txt";
                        else if (fileChoice == 4) filename = "summary_report.txt";
                        else filename = "sport_stats.txt";
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
                                    table.insert(id, sport);
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
                        rm.generateReport("summary_report.txt");
                        rm.generateSportStats();
                        waitForUser();
                    } else if (subChoice == 11) {
                        cout << "Display (1) input.txt, (2) sorted_information.txt, (3) additional_data.txt, "
                             << "(4) summary_report.txt, (5) sport_stats.txt? Enter 1-5: ";
                        int fileChoice = getValidInput(1, 5);
                        cin.ignore();
                        const char* filename;
                        if (fileChoice == 1) filename = "input.txt";
                        else if (fileChoice == 2) filename = "sorted_information.txt";
                        else if (fileChoice == 3) filename = "additional_data.txt";
                        else if (fileChoice == 4) filename = "summary_report.txt";
                        else filename = "sport_stats.txt";
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