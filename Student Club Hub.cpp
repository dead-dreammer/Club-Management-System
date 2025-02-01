#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

const int MAX_CLUBS = 10;

// Node structure for each student in a club
struct StudentNode {
    int studentId;               
    string firstName;            
    string lastName;             
    StudentNode* next; // Pointer to next student in the club list

    StudentNode(int id, const string& first, const string& last)
        : studentId(id), firstName(first), lastName(last), next(nullptr) {}
};

// Club structure representing a club and its members as an adjacency list
struct ClubList {
    int clubId;                 
    string clubName;            
    StudentNode* head; // Head pointer to the linked list of club members
};

// Arrays and maps for managing clubs and students
ClubList clubs[MAX_CLUBS];                    // Array of clubs
unordered_map<string, int> clubNameToId;      // Map of club names to IDs
unordered_map<int, pair<string, string>> students; // Map of student ID to first and last name
unordered_map<int, vector<int>> studentToClubs;    // Map of student ID to club IDs they belong to
int numClubs = 0;                            

// Function to add a new student to the system with no memberships
void addStudent(int studentId, const string& firstName, const string& lastName) {
    students[studentId] = {firstName, lastName}; // Map student ID to name
    studentToClubs[studentId] = {};              // Initialize empty list of club memberships
}

// Function to create a new club with a specific name
void createClub(const string& clubName) {
    if (numClubs < MAX_CLUBS) {                   
        clubs[numClubs].clubId = numClubs;        
        clubs[numClubs].clubName = clubName;      
        clubs[numClubs].head = nullptr;          
        clubNameToId[clubName] = numClubs;  // Map club name to ID
        numClubs++;                              
    } else {
        cout << "Maximum number of clubs reached." << endl; 
    }
}

// Function to add a student to a specified club using club name
void addStudentToClub(const string& clubName, int studentId) {
    if (clubNameToId.find(clubName) != clubNameToId.end()) { 
        int clubId = clubNameToId[clubName];                  
        if (students.find(studentId) != students.end()) {    
            const auto& [firstName, lastName] = students[studentId]; 
            StudentNode* newNode = new StudentNode(studentId, firstName, lastName); 
            newNode->next = clubs[clubId].head;   // Insert new node at the start of club's list
            clubs[clubId].head = newNode;         // Update club head to new node
            studentToClubs[studentId].push_back(clubId); // Track student's membership in club
        } 
        else {
            cout << "Student ID not found in the system." << endl; 
        }
    } 
    else {
        cout << "Club not found." << endl;       
    }
}

// Function to list all students in a specific club using club name
void listStudentsInClub(const string& clubName) {
    if (clubNameToId.find(clubName) != clubNameToId.end()) { 
        int clubId = clubNameToId[clubName];
        cout << "Students in club \"" << clubs[clubId].clubName << "\":" << endl;
        StudentNode* current = clubs[clubId].head;  
        while (current != nullptr) {                
            cout << "ID: " << current->studentId 
                 << ", Name: " << current->firstName << " " << current->lastName << endl;
            current = current->next;
        }
        cout << endl;
    } else {
        cout << "Club not found." << endl;          
    }
}

// Function to list all clubs a specific student is a member of
void listClubsForStudent(int studentId) {
    if (students.find(studentId) != students.end()) {  
        const auto& [firstName, lastName] = students[studentId]; 
        cout << "Student " << studentId << " (" << firstName << " " << lastName << ") is in clubs: " << endl;
        if (studentToClubs[studentId].empty()) { 
            cout << "No clubs." << endl;
        } else {
            for (int clubId : studentToClubs[studentId]) { 
                cout << clubs[clubId].clubName << endl;
            }
            cout << endl;
        }
    } else {
        cout << "Student ID not found." << endl;
    }
}

// Function to sort students by last name using insertion sort
void sortStudentsByLastName(vector<StudentNode>& studentList) {
    int n = studentList.size();
    for (int i = 1; i < n; i++) {
        StudentNode key = studentList[i];
        int j = i - 1;

        // Move elements that are greater than key to one position ahead of their current position
        while (j >= 0 && studentList[j].lastName > key.lastName) {
            studentList[j + 1] = studentList[j];
            j = j - 1;
        }
        studentList[j + 1] = key;
    }
}

// Function to list all students alphabetically by last name
void listStudentsAlphabetically() {
    vector<StudentNode> studentList;

    // Add all students to a vector for sorting
    for (const auto& [id, name] : students) {
        studentList.emplace_back(id, name.first, name.second);  // Add StudentNode to vector
    }

    // Sort students by last name
    sortStudentsByLastName(studentList);

    // Print sorted list of students
    cout << "Students sorted by last name:" << endl;
    for (const auto& student : studentList) {
        cout << student.studentId << ": " << student.firstName << " " << student.lastName << endl;
    }
}

// Save the current state of the system to a file
void saveSystemState(const string& filename) {
    ofstream file(filename);

    // Save students
    file << "[STUDENTS]\n";
    for (const auto& [id, name] : students) {
        file << id << " " << name.first << " " << name.second << "\n";
    }

    // Save clubs
    file << "[CLUBS]\n";
    for (int i = 0; i < numClubs; ++i) {
        file << clubs[i].clubId << " " << clubs[i].clubName << "\n";
    }

    // Save memberships
    file << "[MEMBERSHIPS]\n";
    for (int i = 0; i < numClubs; ++i) {
        file << clubs[i].clubId << ": ";
        StudentNode* current = clubs[i].head;
        while (current) {
            file << current->studentId << " ";
            current = current->next;
        }
        file << "-1\n";  // End of the list for this club (changed to -1)
    }

    file.close();
}

// Load the state of the system from a file
void loadSystemState(const string& filename) {
    ifstream file(filename);
    string line;
    string section;

    // Clear current data
    students.clear();
    clubNameToId.clear();
    studentToClubs.clear();
    numClubs = 0;

    while (getline(file, line)) {
        if (line == "[STUDENTS]") {
            section = "STUDENTS";
            cout << "Loading Students:\n";
        } else if (line == "[CLUBS]") {
            section = "CLUBS";
            cout << "Loading Clubs:\n";
        } else if (line == "[MEMBERSHIPS]") {
            section = "MEMBERSHIPS";
            cout << "Loading Memberships:\n";
        } else if (section == "STUDENTS") {
            int id;
            string firstName, lastName;
            istringstream iss(line);
            iss >> id >> firstName >> lastName;
            students[id] = {firstName, lastName};
            // Output student data
            cout << "Student ID: " << id << ", First Name: " << firstName << ", Last Name: " << lastName << "\n";
        } else if (section == "CLUBS") {
            int clubId;
            string clubName;
            istringstream iss(line);
            iss >> clubId >> clubName;
            clubs[numClubs].clubId = clubId;
            clubs[numClubs].clubName = clubName;
            clubs[numClubs].head = nullptr;
            clubNameToId[clubName] = clubId;
            numClubs++;
            // Output club data
            cout << "Club ID: " << clubId << ", Club Name: " << clubName << "\n";
        } else if (section == "MEMBERSHIPS") {
            int clubId;
            char colon;
            istringstream iss(line);
            iss >> clubId >> colon;
            int studentId;
            StudentNode* prev = nullptr;
            cout << "Club ID " << clubId << " Memberships:\n";
            while (iss >> studentId) {
                if (studentId == -1) break; // End of list
                const auto& [firstName, lastName] = students[studentId];
                StudentNode* newNode = new StudentNode(studentId, firstName, lastName);
                if (prev == nullptr) {
                    clubs[clubId].head = newNode;
                } else {
                    prev->next = newNode;
                }
                prev = newNode;
                studentToClubs[studentId].push_back(clubId);
                // Output membership data
                cout << "  Student ID: " << studentId << ", Name: " << firstName << " " << lastName << "\n";
            }
        }
    }

    file.close();
    cout << "Data loading complete.\n";
    cout << endl;
}

// Main function providing a menu for user interaction
int main() {
    int option, studentId;
    string firstName, lastName, clubName, fileName;
    bool menu = true;

    while (menu) {
        cout << "Welcome to ClubHub!" << endl;
        cout << "Choose an option: " << endl;
        cout << "1. Add a new student" << endl;
        cout << "2. Create a new club" << endl;
        cout << "3. Add a student to a club" << endl;
        cout << "4. List the clubs of a student" << endl;
        cout << "5. List all the students in a club" << endl;
        cout << "6. Save the current state of the system"<< endl;
        cout << "7. Load the data from a file to the system" << endl;
        cout << "8. List students in alphabetical order" << endl;
        cout << "9. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> option;
        cin.ignore();

        switch (option) {
            case 1: // Add a new student
                cout << "Enter the student's ID: ";
                cin >> studentId;
                cout << "Enter the student's first name: ";
                cin >> firstName;
                cout << "Enter the student's last name: ";
                cin >> lastName;
                addStudent(studentId, firstName, lastName);
                break;

            case 2: // Create a new club
                cout << "Enter the new club's name: ";
                cin >> clubName;
                createClub(clubName);
                break;

            case 3: // Add a student to a club
                cout << "Enter club name: ";
                cin >> clubName;
                cout << "Enter the student's ID: ";
                cin >> studentId;
                addStudentToClub(clubName, studentId);
                break;

            case 4: // List clubs for a student
                cout << "Enter the student's ID: ";
                cin >> studentId;
                listClubsForStudent(studentId);
                break;

            case 5: // List students in a club
                cout << "Enter club name: ";
                cin >> clubName;
                listStudentsInClub(clubName);
                break;

            case 6:
                cout << "Enter the filename to save the data on: ";
                cin >> fileName;
                saveSystemState(fileName);
                break;

            case 7:
                cout << "Enter the name of a file to load data from: ";
                cin >> fileName;
                loadSystemState(fileName);
                break;

            case 8:
                listStudentsAlphabetically();
                break;

            case 9: // Exit
                menu = false;
                break;

            default:
                cout << "Invalid option. Try again." << endl;
                break;
        }
    }
    return 0;
}
