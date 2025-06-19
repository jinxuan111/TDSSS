#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <errno.h>
#include <string>
#include <direct.h>
#include <set>
using namespace std;

// Forward declaration
int getValidInput(int min, int max);

// Constants
const int MAX_SIZE = 100;
const int TABLE_SIZE = 17;
const int SPORT_COUNT = 8;
const int MAX_ENTRIES = 100;
const int MAX_USERS = 100;
const string BASE_PATH = "C:\\Temp\\TDSSS\\";

// Structures
struct Node {
    int id;
    char name[50];
    char sport[50];
    char time[50];
    Node* next;
};

struct Sport {
    char name[50];
    char time[50];
    int capacity;
    int current_count;
};

struct Athlete {
    int id;
    char name[50];
    char sport[50];
    char time[50];
};

struct UserInfo {
    int id;
    char username[50];
    char password[50];
    char type[20]; // "Admin" or "Customer"
};

// Base class for user management
class User {
protected:
    int id;
    char username[50];
    char password[50];
public:
    User() {
        id = 0;
        strcpy(username, "default");
        strcpy(password, "pass123");
    }
    virtual void login(int userId, const char* pass) = 0;
    virtual ~User() {}
    int getId() const { return id; }
};

// Derived class for Admin
class Admin : public User {
public:
    Admin(int userId, const char* user, const char* pass) {
        id = userId;
        strncpy(username, user, 49);
        username[49] = '\0';
        strncpy(password, pass, 49);
        password[49] = '\0';
    }
    void login(int userId, const char* pass) override {
        if (userId == id && strcmp(pass, password) == 0) {
            cout << "Admin login successful!" << endl;
        } else {
            throw "Invalid admin credentials";
        }
    }
    ~Admin() {}
};

// Derived class for Customer
class Customer : public User {
public:
    Customer(int userId, const char* user, const char* pass) {
        id = userId;
        strncpy(username, user, 49);
        username[49] = '\0';
        strncpy(password, pass, 49);
        password[49] = '\0';
    }
    void login(int userId, const char* pass) override {
        if (userId == id && strcmp(pass, password) == 0) {
            cout << "Customer login successful!" << endl;
        } else {
            throw "Invalid customer credentials";
        }
    }
    ~Customer() {}
};

// Derived class for Guest
class Guest : public User {
public:
    Guest() {
        id = 0;
        strcpy(username, "guest");
        strcpy(password, "none");
    }
    void login(int userId, const char* pass) override {
        cout << "Guest login successful!" << endl;
    }
    ~Guest() {}
};

// Hash Table class
class HashTable {
private:
    Node* table[TABLE_SIZE];
    Sport sports[SPORT_COUNT];
    bool isLoadingFromFile;
    Athlete athletes[MAX_ENTRIES];
    int athleteCount;
    UserInfo users[MAX_USERS];
    int userCount;
    static set<string> loadedFiles; // Track loaded files

    int hashFunction(int key) {
        return key % TABLE_SIZE;
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

    bool isValidSport(const char* sport) {
        for (int i = 0; i < SPORT_COUNT; i++) {
            if (strcmp(sport, sports[i].name) == 0) return true;
        }
        return false;
    }

    bool hasTimeConflict(int id, const char* new_time) {
        int index = hashFunction(id);
        Node* current = table[index];
        string new_day(new_time, 3);
        while (current != NULL) {
            if (current->id == id && strcmp(current->sport, "None") != 0) {
                string current_day(current->time, 3);
                if (new_day == current_day && strcmp(current->time, new_time) == 0) {
                    return true;
                }
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

    bool idExists(int id) {
        int index = hashFunction(id);
        Node* current = table[index];
        while (current != NULL) {
            if (current->id == id) return true;
            current = current->next;
        }
        return false;
    }

    bool findStudentName(int id, char* name) {
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
                         << "' and '" << current->name << "'. Using '" << firstName << "'." << endl;
                }
            }
            current = current->next;
        }
        return !first;
    }

    bool isValidID(int id) {
        return id > 0;
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
    }

    void mergeSort(Athlete arr[], int left, int right) {
        if (right - left <= 32) {
            insertionSort(arr, left, right);
            return;
        }
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    int binarySearch(int targetId) {
        if (athleteCount == 0) return -1;
        int left = 0, right = athleteCount - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (athletes[mid].id == targetId) return mid;
            if (athletes[mid].id < targetId) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    void syncAthletesArray() {
        athleteCount = 0;
        for (int i = 0; i < SPORT_COUNT; i++) {
            sports[i].current_count = 0; // Reset sport counts
        }
        for (int i = 0; i < TABLE_SIZE; i++) {
            Node* current = table[i];
            while (current != NULL && athleteCount < MAX_ENTRIES) {
                athletes[athleteCount].id = current->id;
                strcpy(athletes[athleteCount].name, current->name);
                strcpy(athletes[athleteCount].sport, current->sport);
                strcpy(athletes[athleteCount].time, current->time);
                if (strcmp(current->sport, "None") != 0) {
                    for (int j = 0; j < SPORT_COUNT; j++) {
                        if (strcmp(current->sport, sports[j].name) == 0) {
                            sports[j].current_count++;
                            break;
                        }
                    }
                    athleteCount++;
                } else {
                    athleteCount++;
                }
                current = current->next;
            }
        }
        if (athleteCount > 1) mergeSort(athletes, 0, athleteCount - 1);
    }

    bool idExistsUser(int id) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].id == id) return true;
        }
        return false;
    }

    bool usernameExists(const char* username) {
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0) return true;
        }
        return false;
    }

    int findUserIndexById(int id) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].id == id) return i;
        }
        return -1;
    }

    bool createFileIfNotExists(const string& filename) {
        string dir = BASE_PATH;
        if (_mkdir(dir.c_str()) != 0 && errno != EEXIST) {
            cout << "Error: Unable to create directory '" << dir << "': " << strerror(errno) << endl;
            return false;
        }
        ofstream outFile(filename.c_str(), ios::app);
        if (!outFile) {
            cout << "Error: Unable to create file '" << filename << "': " << strerror(errno) << endl;
            return false;
        }
        outFile.close();
        return true;
    }

    bool sportExistsForId(int id, const char* sport, const char* time) {
        int index = hashFunction(id);
        Node* current = table[index];
        while (current != NULL) {
            if (current->id == id && strcmp(current->sport, sport) == 0 && strcmp(current->time, time) == 0) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void saveUsersToFile(const string& filename) {
        try {
            string fullPath = BASE_PATH + filename;
            if (!createFileIfNotExists(fullPath)) {
                throw string("Failed to create file: ") + filename;
            }
            ofstream outFile(fullPath.c_str());
            if (!outFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
            outFile << "========== USER DATA ==========\n";
            outFile << "ID    | Username | Password | Type\n";
            outFile << "------|----------|----------|-------\n";
            for (int i = 0; i < userCount; i++) {
                outFile << left << setw(5) << users[i].id << " | "
                        << setw(8) << users[i].username << " | "
                        << setw(8) << users[i].password << " | "
                        << users[i].type << endl;
            }
            outFile << "===============================\n";
            outFile.close();
            cout << "User data saved to '" << filename << "'" << endl;
        } catch (const string& error) {
            cout << "Error: " << error << endl;
        }
    }

    friend void exportSummaryReport(HashTable& ht, const string& filename);
    friend void importData(HashTable& ht, const string& filename);
    friend void generateStatistics(HashTable& ht, const string& filename);
    friend void backupData(HashTable& ht, const string& filename);

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = NULL;
        initSports();
        isLoadingFromFile = false;
        athleteCount = 0;
        userCount = 0;
        users[0].id = 1;
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "admin123");
        strcpy(users[0].type, "Admin");
        users[1].id = 2;
        strcpy(users[1].username, "customer");
        strcpy(users[1].password, "cust123");
        strcpy(users[1].type, "Customer");
        userCount = 2;
        string fullPath = BASE_PATH + "user_data.txt";
        if (createFileIfNotExists(fullPath)) {
            saveUsersToFile("user_data.txt");
        }
    }

    int getSportCount() const { return SPORT_COUNT; }

    bool getSportByIndex(int index, char* sport, char* time) {
        if (index < 1 || index > SPORT_COUNT) return false;
        strcpy(sport, sports[index - 1].name);
        strcpy(time, sports[index - 1].time);
        return true;
    }

    void registerUser() {
        try {
            if (userCount >= MAX_USERS) throw "Maximum user limit reached";
            int id;
            char username[50], password[50], type[20];
            cout << "\n--- Register New User ---\n";
            cout << "Enter ID: ";
            id = getValidInput(1, 99999);
            cin.ignore();
            if (idExistsUser(id)) throw "ID already exists";
            cout << "Enter username: ";
            cin.getline(username, 50);
            trimWhitespace(username);
            if (strlen(username) == 0) throw "Empty username";
            cout << "Enter password: ";
            cin.getline(password, 50);
            trimWhitespace(password);
            if (strlen(password) == 0) throw "Empty password";
            cout << "Enter user type (1 for Admin, 2 for Customer): ";
            int typeChoice = getValidInput(1, 2);
            cin.ignore();
            strcpy(type, typeChoice == 1 ? "Admin" : "Customer");
            if (typeChoice == 2 && !idExists(id)) {
                insertStudent(id, username, password);
            }
            users[userCount].id = id;
            strncpy(users[userCount].username, username, 49);
            users[userCount].username[49] = '\0';
            strncpy(users[userCount].password, password, 49);
            users[userCount].password[49] = '\0';
            strcpy(users[userCount].type, type);
            userCount++;
            saveUsersToFile("user_data.txt");
            cout << "User ID " << id << " ('" << username << "') registered successfully as " << type << "!" << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void loadUsersFromFile(const string& filename) {
        try {
            string fullPath = BASE_PATH + filename;
            if (!createFileIfNotExists(fullPath)) {
                throw string("Failed to create file: ") + filename;
            }
            ifstream inFile(fullPath.c_str());
            if (!inFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
            char line[256];
            bool firstLine = true;
            while (inFile.getline(line, 256)) {
                if (firstLine || strstr(line, "==========") || strstr(line, "------|") || line[0] == '\0') {
                    firstLine = false;
                    continue;
                }
                int id;
                char username[50], password[50], type[20];
                char* token = strtok(line, "|");
                if (!token) continue;
                trimWhitespace(token);
                id = atoi(token);
                token = strtok(NULL, "|");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(username, token, 49);
                username[49] = '\0';
                token = strtok(NULL, "|");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(password, token, 49);
                password[49] = '\0';
                token = strtok(NULL, "\n");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(type, token, 19);
                type[19] = '\0';
                if (userCount < MAX_USERS && !idExistsUser(id)) {
                    users[userCount].id = id;
                    strncpy(users[userCount].username, username, 49);
                    users[userCount].username[49] = '\0';
                    strncpy(users[userCount].password, password, 49);
                    users[userCount].password[49] = '\0';
                    strcpy(users[userCount].type, type);
                    userCount++;
                }
            }
            inFile.close();
            cout << "Loaded " << userCount << " users from '" << filename << "'" << endl;
            saveUsersToFile("user_data.txt"); // Ensure file is updated
        } catch (const string& error) {
            cout << "Error: " << error << endl;
        }
    }

    User* authenticateUser(int id, const char* password, const char* expectedType) {
        for (int i = 0; i < userCount; i++) {
            if (users[i].id == id && strcmp(users[i].password, password) == 0 &&
                strcmp(users[i].type, expectedType) == 0) {
                if (strcmp(expectedType, "Admin") == 0) {
                    return new Admin(id, users[i].username, password);
                } else if (strcmp(expectedType, "Customer") == 0) {
                    return new Customer(id, users[i].username, password);
                }
            }
        }
        return NULL;
    }

    bool insert(int id, const char* name, const char* sport, const char* time) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
            if (!idExists(id)) throw "ID not registered";
            if (!isValidSport(sport)) throw "Invalid sport";
            if (hasTimeConflict(id, time)) throw "Time conflict";
            if (!checkCapacity(sport)) throw "Sport capacity full";

            Node* newNode = new Node;
            newNode->id = id;
            char existingName[50];
            if (findStudentName(id, existingName)) {
                strcpy(newNode->name, existingName);
            } else {
                strcpy(newNode->name, name);
            }
            strcpy(newNode->sport, sport);
            strcpy(newNode->time, time);
            newNode->next = NULL;

            int index = hashFunction(id);
            newNode->next = table[index];
            table[index] = newNode;

            syncAthletesArray();
            saveAllFiles();
            cout << "Successfully inserted: ID=" << id << ", Name=" << newNode->name
                 << ", Sport=" << sport << ", Time=" << time << endl;
            return true;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
            return false;
        }
    }

    bool insertStudent(int id, const char* name, const char* password) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
            if (idExists(id)) throw "ID already exists";
            if (strlen(name) == 0) throw "Empty name";
            if (strlen(password) == 0) throw "Empty password";

            int index = hashFunction(id);
            Node* newNode = new Node;
            newNode->id = id;
            strncpy(newNode->name, name, 49);
            newNode->name[49] = '\0';
            strcpy(newNode->sport, "None");
            strcpy(newNode->time, "None");
            newNode->next = NULL;

            newNode->next = table[index];
            table[index] = newNode;

            if (userCount >= MAX_USERS) throw "Maximum user limit reached";
            users[userCount].id = id;
            strncpy(users[userCount].username, name, 49);
            users[userCount].username[49] = '\0';
            strncpy(users[userCount].password, password, 49);
            users[userCount].password[49] = '\0';
            strcpy(users[userCount].type, "Customer");
            userCount++;

            syncAthletesArray();
            saveAllFiles();
            saveUsersToFile("user_data.txt");
            cout << "Successfully added student: ID=" << id << ", Name=" << name << endl;
            return true;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
            return false;
        }
    }

    void search(int id) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
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
            if (!found) cout << "ID " << id << " not found or has no sport allotments!" << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void search(const char* name) {
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
        if (!found) cout << "Name " << name << " not found!" << endl;
    }

    void searchBinary(int id) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
            syncAthletesArray();
            int index = binarySearch(id);
            if (index == -1) {
                cout << "ID " << id << " not found!" << endl;
            } else {
                cout << "Found: ID=" << athletes[index].id << ", Name=" << athletes[index].name
                     << ", Sport=" << athletes[index].sport << ", Time=" << athletes[index].time << endl;
            }
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void display() {
        cout << "\n============== HASH TABLE CONTENTS ==============\n";
        cout << "Index | ID    | Name            | Sport         | Time\n";
        cout << "------|-------|-----------------|---------------|------------------\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            cout << right << setw(5) << i << " |";
            Node* current = table[i];
            if (current == NULL) {
                cout << setw(7) << "" << " |" << " Empty\n";
                continue;
            }
            bool hasSport = false;
            Node* temp = current;
            while (temp != NULL) {
                if (strcmp(temp->sport, "None") != 0) {
                    hasSport = true;
                    break;
                }
                temp = temp->next;
            }
            if (!hasSport) {
                cout << setw(7) << "" << " |" << " Empty\n";
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
        }
        cout << "==================================================\n";
    }

    void displaySorted() {
        syncAthletesArray();
        cout << "\n========== SORTED ATHLETE DATA (BY ID - Merge Sort) ==========\n";
        cout << "ID    | Name            | Sport         | Time\n";
        cout << "------|-----------------|---------------|------------------\n";
        for (int i = 0; i < athleteCount; i++) {
            cout << right << setw(5) << athletes[i].id << " | "
                 << setw(16) << left << athletes[i].name << " | "
                 << setw(14) << athletes[i].sport << " | "
                 << athletes[i].time << endl;
        }
        cout << "==================================================\n";
    }

    void displaySports() {
        cout << "\n============= AVAILABLE SPORTS =============\n";
        cout << "No. | Sport         | Time              | Capacity\n";
        cout << "----|---------------|-------------------|---------|\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            cout << right << setw(3) << (i + 1) << " | "
                 << setw(13) << left << sports[i].name << " | "
                 << setw(17) << sports[i].time << " | "
                 << sports[i].current_count << "/" << sports[i].capacity << endl;
        }
        cout << "==========================================\n";
    }

    void displayFileContents(const string& filename) {
        try {
            string fullPath = BASE_PATH + filename;
            ifstream inFile(fullPath.c_str());
            if (!inFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
            cout << "\n===== Contents of " << filename << " =====" << endl;
            string line;
            while (getline(inFile, line)) cout << line << endl;
            inFile.close();
            cout << "====================================\n";
        } catch (const string& error) {
            cout << "Error: " << error << endl;
        }
    }

    void displaySportParticipants(const char* sport) {
        try {
            if (!isValidSport(sport)) throw "Invalid sport";
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
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void displaySportParticipantsSorted(const char* sport) {
        try {
            if (!isValidSport(sport)) throw "Invalid sport";
            syncAthletesArray();
            cout << "\n===== PARTICIPANTS IN " << sport << " (SORTED - Merge Sort) =====\n";
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
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void displayTimeTable(int id) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
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
            if (!found) cout << "Warning: ID " << id << " not found in system!" << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void readFromFile(const string& filename) {
        try {
            if (loadedFiles.find(filename) != loadedFiles.end()) {
                cout << "Skipping already loaded file: " << filename << endl;
                return;
            }
            loadedFiles.insert(filename);
            isLoadingFromFile = true;
            string fullPath = BASE_PATH + filename;
            if (!createFileIfNotExists(fullPath)) {
                throw string("Failed to create file: ") + filename;
            }
            ifstream inFile(fullPath.c_str());
            if (!inFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
            char line[256];
            bool firstLine = true;
            while (inFile.getline(line, 256)) {
                if (firstLine || strstr(line, "==========") || strstr(line, "------|") || line[0] == '\0') {
                    firstLine = false;
                    continue;
                }
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
                strncpy(name, token, 49);
                name[49] = '\0';
                token = strtok(NULL, "|");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(sport, token, 49);
                sport[49] = '\0';
                token = strtok(NULL, "\n");
                if (!token) continue;
                trimWhitespace(token);
                strncpy(time, token, 49);
                time[49] = '\0';
                cout << "Loading: ID=" << id << ", Name=" << name << ", Sport=" << sport << ", Time=" << time << endl;
                if (!idExists(id)) insertStudent(id, name, "default123");
                if (strcmp(sport, "None") != 0 && !sportExistsForId(id, sport, time)) {
                    insert(id, name, sport, time);
                }
            }
            inFile.close();
            isLoadingFromFile = false;
            syncAthletesArray();
            saveAllFiles();
            cout << "Successfully loaded '" << filename << "'" << endl;
        } catch (const string& error) {
            cout << "Error: " << error << endl;
            isLoadingFromFile = false;
        }
    }

    void loadAllFiles() {
        try {
            loadedFiles.clear(); // Reset loaded files
            cout << "Loading from input.txt...\n";
            readFromFile("input.txt");
            cout << "Loading from sorted_information.txt...\n";
            readFromFile("sorted_information.txt");
            cout << "Loading from user_data.txt...\n";
            loadUsersFromFile("user_data.txt");
            cout << "Loading from statistics.txt...\n";
            readFromFile("statistics.txt");
            cout << "Loading from backup.txt...\n";
            readFromFile("backup.txt");
            syncAthletesArray();
            saveAllFiles();
        } catch (const string& error) {
            cout << "Error: " << error << endl;
        }
    }

    void saveToFile(const string& filename) {
        try {
            string fullPath = BASE_PATH + filename;
            if (!createFileIfNotExists(fullPath)) {
                throw string("Failed to create file: ") + filename;
            }
            ofstream outFile(fullPath.c_str());
            if (!outFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
            outFile << "========== SPORTS ALLOTMENT SYSTEM DATA ==========\n";
            outFile << "ID    | Name             | Sport         | Time\n";
            outFile << "------|------------------|---------------|------------------\n";
            int ids[MAX_ENTRIES];
            int idCount = 0;
            for (int i = 0; i < TABLE_SIZE; i++) {
                Node* current = table[i];
                while (current != NULL) {
                    bool exists = false;
                    for (int j = 0; j < idCount; j++) {
                        if (ids[j] == current->id) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists && idCount < MAX_ENTRIES) ids[idCount++] = current->id;
                    current = current->next;
                }
            }
            for (int i = 0; i < idCount - 1; i++) {
                for (int j = i + 1; j < idCount; j++) {
                    if (ids[i] > ids[j]) {
                        int temp = ids[i];
                        ids[i] = ids[j];
                        ids[j] = temp;
                    }
                }
            }
            for (int i = 0; i < idCount; i++) {
                int id = ids[i];
                bool hasNonNoneSport = false;
                for (int j = 0; j < TABLE_SIZE; j++) {
                    Node* current = table[j];
                    while (current != NULL) {
                        if (current->id == id && strcmp(current->sport, "None") != 0) {
                            hasNonNoneSport = true;
                            outFile << right << setw(5) << current->id << " | "
                                    << setw(16) << left << current->name << " | "
                                    << setw(13) << left << current->sport << " | "
                                    << current->time << endl;
                        }
                        current = current->next;
                    }
                }
                if (!hasNonNoneSport) {
                    for (int j = 0; j < TABLE_SIZE; j++) {
                        Node* current = table[j];
                        while (current != NULL) {
                            if (current->id == id && strcmp(current->sport, "None") == 0) {
                                outFile << right << setw(5) << current->id << " | "
                                        << setw(16) << left << current->name << " | "
                                        << setw(13) << left << current->sport << " | "
                                        << current->time << endl;
                                break;
                            }
                            current = current->next;
                        }
                    }
                }
            }
            outFile << "==================================================\n";
            outFile.close();
            cout << "Data saved to '" << filename << "'" << endl;
        } catch (const string& error) {
            cout << "Error: " << error << endl;
        }
    }

    void saveAllFiles() {
        syncAthletesArray();
        saveToFile("input.txt");
        saveToFile("sorted_information.txt");
        saveToFile("backup.txt");
        generateStatistics(*this, "statistics.txt");
        exportSummaryReport(*this, "summary_report.txt");
        saveUsersToFile("user_data.txt");
    }

    void removeSport(int id, const char* sport) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
            if (!isValidSport(sport)) throw "Invalid sport";
            int index = hashFunction(id);
            Node* current = table[index];
            Node* prev = NULL;
            while (current != NULL) {
                if (current->id == id && strcmp(current->sport, sport) == 0) {
                    reduceCapacity(sport);
                    if (prev == NULL) table[index] = current->next;
                    else prev->next = current->next;
                    syncAthletesArray();
                    saveAllFiles();
                    cout << "Successfully removed: ID=" << id << ", Name=" << current->name
                         << ", Sport=" << sport << endl;
                    delete current;
                    return;
                }
                prev = current;
                current = current->next;
            }
            cout << "ID " << id << " with sport " << sport << " not found!" << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void editStudentRecord(int id, const char* newName) {
        try {
            if (!isValidID(id)) throw "Invalid ID";
            if (!idExists(id)) throw "ID not found";
            if (strlen(newName) == 0) throw "Empty name";
            int index = hashFunction(id);
            Node* current = table[index];
            while (current != NULL) {
                if (current->id == id) {
                    strncpy(current->name, newName, 49);
                    current->name[49] = '\0';
                }
                current = current->next;
            }
            syncAthletesArray();
            saveAllFiles();
            cout << "Successfully updated name for ID=" << id << " to " << newName << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void editStudentRecord(int oldId, int newId) {
        try {
            if (!isValidID(oldId)) throw "Invalid old ID";
            if (!isValidID(newId)) throw "Invalid new ID";
            if (!idExists(oldId)) throw "Old ID not found";
            if (idExists(newId)) throw "New ID already exists";

            int oldIndex = hashFunction(oldId);
            int newIndex = hashFunction(newId);
            Node* current = table[oldIndex];
            Node* prev = NULL;
            Node* newNodeHead = NULL;

            while (current != NULL) {
                if (current->id == oldId) {
                    Node* newNode = new Node;
                    newNode->id = newId;
                    strcpy(newNode->name, current->name);
                    strcpy(newNode->sport, current->sport);
                    strcpy(newNode->time, current->time);
                    newNode->next = newNodeHead;
                    newNodeHead = newNode;

                    Node* temp = current;
                    if (prev == NULL) table[oldIndex] = current->next;
                    else prev->next = current->next;
                    current = current->next;
                    delete temp;
                    continue;
                }
                prev = current;
                current = current->next;
            }

            if (newNodeHead != NULL) {
                newNodeHead->next = table[newIndex];
                table[newIndex] = newNodeHead;
            }

            syncAthletesArray();
            saveAllFiles();
            cout << "Successfully updated ID from " << oldId << " to " << newId << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void editUserRecord(int id, const char* newUsername, const char* newPassword) {
        try {
            int userIndex = findUserIndexById(id);
            if (userIndex == -1) throw "User ID not found";
            if (newUsername && strlen(newUsername) > 0) {
                strncpy(users[userIndex].username, newUsername, 49);
                users[userIndex].username[49] = '\0';
            }
            if (newPassword && strlen(newPassword) > 0) {
                strncpy(users[userIndex].password, newPassword, 49);
                users[userIndex].password[49] = '\0';
            }
            saveUsersToFile("user_data.txt");
            cout << "Successfully updated user record for ID=" << id << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
        }
    }

    void editUserRecord(int oldId, int newId) {
        try {
            if (!isValidID(oldId)) throw "Invalid old ID";
            if (!isValidID(newId)) throw "Invalid new ID";
            int userIndex = findUserIndexById(oldId);
            if (userIndex == -1) throw "Old user ID not found";
            if (idExistsUser(newId)) throw "New user ID already exists";
            users[userIndex].id = newId;
            saveUsersToFile("user_data.txt");
            cout << "Successfully updated user ID from " << oldId << " to " << newId << endl;
        } catch (const char* error) {
            cout << "Error: " << error << endl;
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
            table[i] = NULL;
        }
    }
};

set<string> HashTable::loadedFiles;

void exportSummaryReport(HashTable& ht, const string& filename) {
    try {
        string fullPath = BASE_PATH + filename;
        if (!ht.createFileIfNotExists(fullPath)) {
            throw string("Failed to create file: ") + filename;
        }
        ofstream outFile(fullPath.c_str());
        if (!outFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
        outFile << "========== SPORTS PARTICIPATION SUMMARY ==========\n";
        outFile << "Sport         | Participants | Capacity\n";
        outFile << "--------------|--------------|---------|\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            outFile << left << setw(13) << ht.sports[i].name << " | "
                    << setw(12) << ht.sports[i].current_count << " | "
                    << ht.sports[i].capacity << endl;
        }
        outFile << "==================================================\n";
        outFile.close();
        cout << "Summary report saved to '" << filename << "'" << endl;
    } catch (const string& error) {
        cout << "Error: " << error << endl;
    }
}

void importData(HashTable& ht, const string& filename) {
    ht.readFromFile(filename);
}

void generateStatistics(HashTable& ht, const string& filename) {
    try {
        string fullPath = BASE_PATH + filename;
        if (!ht.createFileIfNotExists(fullPath)) {
            throw string("Failed to create file: ") + filename;
        }
        ofstream outFile(fullPath.c_str());
        if (!outFile) throw string("Unable to open file: ") + filename + ": " + strerror(errno);
        outFile << "========== SPORTS STATISTICS ==========\n";
        outFile << "Total Athletes: " << ht.athleteCount << "\n";
        outFile << "Sport Distribution:\n";
        for (int i = 0; i < SPORT_COUNT; i++) {
            outFile << ht.sports[i].name << ": " << ht.sports[i].current_count << " participants\n";
        }
        outFile << "=======================================\n";
        outFile.close();
        cout << "Statistics saved to '" << filename << "'" << endl;
    } catch (const string& error) {
        cout << "Error: " << error << endl;
    }
}

void backupData(HashTable& ht, const string& filename) {
    ht.saveToFile(filename);
}

int getValidInput(int min, int max) {
    int input;
    while (true) {
        try {
            if (cin >> input && input >= min && input <= max) return input;
            throw "Invalid input";
        } catch (const char* error) {
            cout << "Error: Please enter a number between " << min << " and " << max << ": ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }
}

void waitForUser() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

int main() {
    cout << "========================================\n";
    cout << "   SPORTS ALLOTMENT SYSTEM\n";
    cout << "========================================\n";

    HashTable table;
    Guest guest;
    table.loadAllFiles();

    while (true) {
        cout << "\n========================================\n";
        cout << "         MAIN MENU\n";
        cout << "========================================\n";
        cout << "1. Admin Login\n";
        cout << "2. Customer Login\n";
        cout << "3. Guest View\n";
        cout << "4. Register New User\n";
        cout << "0. Exit Program\n";
        cout << "========================================\n";
        cout << "Enter your choice (0-4): ";

        int choice = getValidInput(0, 4);
        cin.ignore();

        if (choice == 0) break;

        try {
            if (choice == 1 || choice == 2) {
                cout << "\nEnter ID: ";
                int id = getValidInput(1, 99999);
                cin.ignore();
                cout << "Enter password: ";
                char pass[50];
                cin.getline(pass, 50);
                User* loggedInUser = table.authenticateUser(id, pass, choice == 1 ? "Admin" : "Customer");
                if (!loggedInUser) throw "Invalid credentials";

                if (choice == 1) {
                    while (true) {
                        cout << "\n========================================\n";
                        cout << "    ADMIN - HASH TABLE OPERATIONS\n";
                        cout << "========================================\n";
                        cout << "1. Add New Student\n";
                        cout << "2. Manage Sport Selection\n";
                        cout << "3. Search Student by ID\n";
                        cout << "4. Search Student by Name\n";
                        cout << "5. Display All Data\n";
                        cout << "6. Display Sport Information\n";
                        cout << "7. Display Student Time Table\n";
                        cout << "8. Save Data\n";
                        cout << "9. Edit Record\n";
                        cout << "10. Generate Summary Report\n";
                        cout << "0. Back to Main Menu\n";
                        cout << "========================================\n";
                        cout << "Enter your choice (0-10): ";

                        int subChoice = getValidInput(0, 10);
                        cin.ignore();

                        if (subChoice == 0) break;

                        if (subChoice == 1) {
                            cout << "\n--- Add New Student ---\n";
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cin.ignore();
                            cout << "Enter Student Name: ";
                            char name[50];
                            cin.getline(name, 50);
                            cout << "Enter Student Password: ";
                            char password[50];
                            cin.getline(password, 50);
                            table.insertStudent(id, name, password);
                        } else if (subChoice == 2) {
                            cout << "\n--- Manage Sport Selection ---\n";
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            cout << "\n1. Add Sport\n2. Remove Sport\nEnter action (1-2): ";
                            int action = getValidInput(1, 2);
                            cin.ignore();
                            if (action == 1) {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number (0 to exit, 1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(0, table.getSportCount());
                                cin.ignore();
                                if (sportIndex == 0) {
                                    cout << "Sport selection cancelled.\n";
                                    continue;
                                }
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.insert(id, "", sport, time);
                                }
                            } else {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number to remove (0 to exit, 1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(0, table.getSportCount());
                                cin.ignore();
                                if (sportIndex == 0) {
                                    cout << "Sport removal cancelled.\n";
                                    continue;
                                }
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.removeSport(id, sport);
                                }
                            }
                        } else if (subChoice == 3) {
                            cout << "\n--- Search Student by ID ---\n";
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.search(id);
                        } else if (subChoice == 4) {
                            cout << "\n--- Search Student by Name ---\n";
                            cout << "Enter Student Name: ";
                            char name[50];
                            cin.getline(name, 50);
                            table.search(name);
                        } else if (subChoice == 5) {
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
                        } else if (subChoice == 6) {
                            table.displaySports();
                        } else if (subChoice == 7) {
                            cout << "\n--- Student Time Table ---\n";
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.displayTimeTable(id);
                        } else if (subChoice == 8) {
                            cout << "Save to (1) input.txt, (2) sorted_information.txt, (3) backup.txt? Enter 1-3: ";
                            int saveChoice = getValidInput(1, 3);
                            cin.ignore();
                            const char* filename;
                            if (saveChoice == 1) filename = "input.txt";
                            else if (saveChoice == 2) filename = "sorted_information.txt";
                            else filename = "backup.txt";
                            table.saveToFile(filename);
                        } else if (subChoice == 9) {
                            cout << "\n--- Edit Record ---\n";
                            cout << "Enter Target User ID: ";
                            int targetId = getValidInput(1, 99999);
                            cout << "\n0. Exit\n1. Edit Name\n2. Edit ID\n3. Edit Password\nEnter action (0-3): ";
                            int editChoice = getValidInput(0, 3);
                            cin.ignore();
                            if (editChoice == 0) {
                                cout << "Edit record cancelled.\n";
                                continue;
                            }
                            if (editChoice == 1) {
                                cout << "Enter New Name: ";
                                char newName[50];
                                cin.getline(newName, 50);
                                table.editUserRecord(targetId, newName, NULL);
                                table.editStudentRecord(targetId, newName);
                            } else if (editChoice == 2) {
                                cout << "Enter New ID: ";
                                int newId = getValidInput(1, 99999);
                                cin.ignore();
                                table.editUserRecord(targetId, newId);
                                table.editStudentRecord(targetId, newId);
                            } else {
                                cout << "Enter New Password: ";
                                char newPassword[50];
                                cin.getline(newPassword, 50);
                                table.editUserRecord(targetId, NULL, newPassword);
                            }
                        } else if (subChoice == 10) {
                            exportSummaryReport(table, "summary_report.txt");
                        }
                        waitForUser();
                    }
                } else {
                    Customer* customer = dynamic_cast<Customer*>(loggedInUser);
                    int customerId = customer->getId();
                    while (true) {
                        cout << "\n========================================\n";
                        cout << "   CUSTOMER - SORTING & SEARCHING\n";
                        cout << "========================================\n";
                        cout << "1. Manage Sport Selection\n";
                        cout << "2. Search Student by ID (Binary Search)\n";
                        cout << "3. Search Student by Name\n";
                        cout << "4. Display Sorted Data\n";
                        cout << "5. Display Sport Participants (Sorted)\n";
                        cout << "6. Display My Time Table\n";
                        cout << "7. Display Available Sports\n";
                        cout << "8. Save Data\n";
                        cout << "9. Edit My Record\n";
                        cout << "0. Back to Main Menu\n";
                        cout << "========================================\n";
                        cout << "Enter your choice (0-9): ";

                        int subChoice = getValidInput(0, 9);
                        cin.ignore();

                        if (subChoice == 0) break;

                        if (subChoice == 1) {
                            cout << "\n--- Manage Sport Selection ---\n";
                            cout << "\n1. Add Sport\n2. Remove Sport\nEnter action (1-2): ";
                            int action = getValidInput(1, 2);
                            cin.ignore();
                            if (action == 1) {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number (0 to exit, 1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(0, table.getSportCount());
                                cin.ignore();
                                if (sportIndex == 0) {
                                    cout << "Sport selection cancelled.\n";
                                    continue;
                                }
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.insert(customerId, "", sport, time);
                                }
                            } else {
                                cout << "\n--- Available Sports ---\n";
                                table.displaySports();
                                cout << "\nEnter sport number to remove (0 to exit, 1-" << table.getSportCount() << "): ";
                                int sportIndex = getValidInput(0, table.getSportCount());
                                cin.ignore();
                                if (sportIndex == 0) {
                                    cout << "Sport removal cancelled.\n";
                                    continue;
                                }
                                char sport[50], time[50];
                                if (table.getSportByIndex(sportIndex, sport, time)) {
                                    table.removeSport(customerId, sport);
                                }
                            }
                        } else if (subChoice == 2) {
                            cout << "\n--- Binary Search by ID ---\n";
                            cout << "Enter Student ID: ";
                            int id = getValidInput(1, 99999);
                            table.searchBinary(id);
                        } else if (subChoice == 3) {
                            cout << "\n--- Search Student by Name ---\n";
                            cout << "Enter Student Name: ";
                            char name[50];
                            cin.getline(name, 50);
                            table.search(name);
                        } else if (subChoice == 4) {
                            cout << "\n--- Sorted Data (Merge Sort) ---\n";
                            table.displaySorted();
                        } else if (subChoice == 5) {
                            cout << "\n--- Sport Participants (Sorted) ---\n";
                            table.displaySports();
                            cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                            int sportIndex = getValidInput(1, table.getSportCount());
                            cin.ignore();
                            char sport[50], time[50];
                            if (table.getSportByIndex(sportIndex, sport, time)) {
                                table.displaySportParticipantsSorted(sport);
                            }
                        } else if (subChoice == 6) {
                            cout << "\n--- My Time Table ---\n";
                            table.displayTimeTable(customerId);
                        } else if (subChoice == 7) {
                            cout << "\n--- Available Sports ---\n";
                            table.displaySports();
                        } else if (subChoice == 8) {
                            cout << "Save to (1) input.txt, (2) sorted_information.txt, (3) backup.txt? Enter 1-3: ";
                            int saveChoice = getValidInput(1, 3);
                            cin.ignore();
                            const char* filename;
                            if (saveChoice == 1) filename = "input.txt";
                            else if (saveChoice == 2) filename = "sorted_information.txt";
                            else filename = "backup.txt";
                            table.saveToFile(filename);
                        } else if (subChoice == 9) {
                            cout << "\n--- Edit My Record ---\n";
                            cout << "0. Exit\n1. Edit Name\n2. Edit ID\n3. Edit Password\nEnter action (0-3): ";
                            int editChoice = getValidInput(0, 3);
                            cin.ignore();
                            if (editChoice == 0) {
                                cout << "Edit record cancelled.\n";
                                continue;
                            }
                            if (editChoice == 1) {
                                cout << "Enter New Name: ";
                                char newName[50];
                                cin.getline(newName, 50);
                                table.editUserRecord(customerId, newName, NULL);
                                table.editStudentRecord(customerId, newName);
                            } else if (editChoice == 2) {
                                cout << "Enter New ID: ";
                                int newId = getValidInput(1, 99999);
                                cin.ignore();
                                table.editUserRecord(customerId, newId);
                                table.editStudentRecord(customerId, newId);
                            } else {
                                cout << "Enter New Password: ";
                                char newPassword[50];
                                cin.getline(newPassword, 50);
                                table.editUserRecord(customerId, NULL, newPassword);
                            }
                        }
                        waitForUser();
                    }
                }
                delete loggedInUser;
            } else if (choice == 3) {
                guest.login(0, "none");
                while (true) {
                    cout << "\n========================================\n";
                    cout << "   GUEST - VIEW ONLY\n";
                    cout << "========================================\n";
                    cout << "1. Display Available Sports\n";
                    cout << "2. Display Sport Participants\n";
                    cout << "3. Display File Contents\n";
                    cout << "0. Back to Main Menu\n";
                    cout << "========================================\n";
                    cout << "Enter your choice (0-3): ";

                    int subChoice = getValidInput(0, 3);
                    cin.ignore();

                    if (subChoice == 0) break;

                    if (subChoice == 1) {
                        table.displaySports();
                    } else if (subChoice == 2) {
                        table.displaySports();
                        cout << "\nEnter sport number (1-" << table.getSportCount() << "): ";
                        int sportIndex = getValidInput(1, table.getSportCount());
                        cin.ignore();
                        char sport[50], time[50];
                        if (table.getSportByIndex(sportIndex, sport, time)) {
                            table.displaySportParticipants(sport);
                        }
                    } else if (subChoice == 3) {
                        cout << "Choose file: (1) input.txt, (2) sorted_information.txt, (3) summary_report.txt, (4) statistics.txt, (5) backup.txt: ";
                        int fileChoice = getValidInput(1, 5);
                        cin.ignore();
                        const char* filename;
                        if (fileChoice == 1) filename = "input.txt";
                        else if (fileChoice == 2) filename = "sorted_information.txt";
                        else if (fileChoice == 3) filename = "summary_report.txt";
                        else if (fileChoice == 4) filename = "statistics.txt";
                        else filename = "backup.txt";
                        table.displayFileContents(filename);
                    }
                    waitForUser();
                }
            } else if (choice == 4) {
                table.registerUser();
                waitForUser();
            }
        } catch (const char* error) {
            cout << "Error: " << error << endl;
            waitForUser();
        }
    }

    cout << "\nProgram terminated. Thank you for using the Sports Allotment System!\n";
    return 0;
}