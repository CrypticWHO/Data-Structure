#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// ============================================================
// Struct: Resident
// Stores all data fields for a single resident from the CSV.
// monthlyEmission is computed as dailyDist * factor * days.
// ============================================================
struct Resident {
    string id;
    int age;
    string mode;
    double dailyDist;
    double factor;
    int days;
    double monthlyEmission;
    string cityName;
};

// ============================================================
// Class: NodeType
// A single node in the singly linked list.
// ============================================================
class NodeType {
public:
    Resident info;
    NodeType* link;
};

// ============================================================
// Class: CityList
// Singly linked list storing all residents from all datasets.
// ============================================================
class CityList {
public:
    NodeType* head;
    NodeType* tail;
    int size;

    CityList() { head = NULL; tail = NULL; size = 0; }

    // --------------------------------------------------------
    // Destructor: free all nodes
    // --------------------------------------------------------
    ~CityList() {
        NodeType* curr = head;
        while (curr != NULL) {
            NodeType* next = curr->link;
            delete curr;
            curr = next;
        }
    }

    // --------------------------------------------------------
    // insert: creates a new node and appends to the tail
    // --------------------------------------------------------
    void insert(string id, int a, string m, double d, double f, int dy, string city) {
        NodeType* newNode = new NodeType;
        newNode->info.id             = id;
        newNode->info.age            = a;
        newNode->info.mode           = m;
        newNode->info.dailyDist      = d;
        newNode->info.factor         = f;
        newNode->info.days           = dy;
        newNode->info.monthlyEmission = d * f * dy;
        newNode->info.cityName       = city;
        newNode->link = NULL;

        if (head == NULL) {
            head = tail = newNode;
        } else {
            tail->link = newNode;
            tail = newNode;
        }
        size++;
    }

    // --------------------------------------------------------
    // loadCSV: reads a CSV file and inserts each row as a node
    // --------------------------------------------------------
    void loadCSV(string filename, string city) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << filename << endl;
            return;
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            if (line.empty()) continue;

            // Remove carriage return if present (Windows line endings)
            if (!line.empty() && line.back() == '\r')
                line.pop_back();

            stringstream ss(line);
            string id, ageS, mode, distS, factS, daysS;
            getline(ss, id,    ',');
            getline(ss, ageS,  ',');
            getline(ss, mode,  ',');
            getline(ss, distS, ',');
            getline(ss, factS, ',');
            getline(ss, daysS, ',');

            if (!id.empty())
                insert(id, stoi(ageS), mode, stod(distS), stod(factS), stoi(daysS), city);
        }
        file.close();
    }

    // --------------------------------------------------------
    // Task 4b: For each age group print per-mode breakdown:
    //   Count, Total Emission, Avg per Resident,
    //   Total & Average for group, Most Preferred Mode
    // Traverses linked list for each age group.
    // --------------------------------------------------------
    void printTask4b() {
        string labels[5] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
        string names[5]  = {
            "Children & Teenagers",
            "University Students / Young Adults",
            "Working Adults (Early Career)",
            "Working Adults (Late Career)",
            "Senior Citizens / Retirees"
        };
        int ageLow[5]  = {6,  18, 26, 46, 61};
        int ageHigh[5] = {17, 25, 45, 60, 100};
        string allModes[6] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};

        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 4B: AGE GROUP ANALYSIS - CARBON EMISSIONS" << endl;
        cout << string(65, '=') << endl;

        for (int g = 0; g < 5; g++) {
            double modeEmissions[6] = {0,0,0,0,0,0};
            int    modeCounts[6]    = {0,0,0,0,0,0};
            int    groupCount       = 0;
            double groupTotal       = 0.0;

            // Traverse linked list
            NodeType* curr = head;
            while (curr != NULL) {
                int a = curr->info.age;
                if (a >= ageLow[g] && a <= ageHigh[g]) {
                    groupCount++;
                    groupTotal += curr->info.monthlyEmission;
                    for (int m = 0; m < 6; m++) {
                        if (curr->info.mode == allModes[m]) {
                            modeEmissions[m] += curr->info.monthlyEmission;
                            modeCounts[m]++;
                            break;
                        }
                    }
                }
                curr = curr->link;
            }

            int prefIdx = 0;
            for (int m = 1; m < 6; m++)
                if (modeCounts[m] > modeCounts[prefIdx]) prefIdx = m;

            double avgPerResident = (groupCount > 0) ? groupTotal / groupCount : 0.0;

            cout << "\nAge Group: " << labels[g] << " (" << names[g] << ")" << endl;
            cout << string(65, '-') << endl;

            if (groupCount == 0) {
                cout << "  No residents found in dataset." << endl;
                cout << string(65, '-') << endl;
                continue;
            }

            cout << left
                 << setw(15) << "Mode"
                 << setw(8)  << "Count"
                 << setw(28) << "Total Emission (kg CO2)"
                 << "Avg per Resident" << endl;
            cout << string(65, '-') << endl;

            for (int m = 0; m < 6; m++) {
                if (modeCounts[m] == 0) continue;
                double avgMode = modeEmissions[m] / modeCounts[m];
                cout << left
                     << setw(15) << allModes[m]
                     << setw(8)  << modeCounts[m]
                     << setw(28) << fixed << setprecision(2) << modeEmissions[m]
                     << avgMode << " kg" << endl;
            }

            cout << string(65, '-') << endl;
            cout << "Total Emission for Age Group : " << fixed << setprecision(2) << groupTotal << " kg CO2" << endl;
            cout << "Average Emission per Resident: " << avgPerResident << " kg CO2" << endl;
            cout << "Most Preferred Mode          : " << allModes[prefIdx]
                 << " (" << modeCounts[prefIdx] << " residents)" << endl;
        }
        cout << "\n" << string(65, '=') << endl;
    }

    // --------------------------------------------------------
    // Task 5: Total carbon emissions per mode and per city
    // --------------------------------------------------------
    void printTask5() {
        string allModes[6] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
        string cities[3]   = {"City A", "City B", "City C"};

        double modeEmissions[6] = {0,0,0,0,0,0};
        int    modeCounts[6]    = {0,0,0,0,0,0};
        double cityEmissions[3] = {0,0,0};
        int    cityCounts[3]    = {0,0,0};
        double grandTotal       = 0.0;

        NodeType* curr = head;
        while (curr != NULL) {
            grandTotal += curr->info.monthlyEmission;
            for (int m = 0; m < 6; m++) {
                if (curr->info.mode == allModes[m]) {
                    modeEmissions[m] += curr->info.monthlyEmission;
                    modeCounts[m]++;
                    break;
                }
            }
            for (int c = 0; c < 3; c++) {
                if (curr->info.cityName == cities[c]) {
                    cityEmissions[c] += curr->info.monthlyEmission;
                    cityCounts[c]++;
                    break;
                }
            }
            curr = curr->link;
        }

        // 5a: By mode
        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 5A: CARBON EMISSIONS BY MODE OF TRANSPORT" << endl;
        cout << string(65, '=') << endl;
        cout << left
             << setw(15) << "Mode"
             << setw(8)  << "Count"
             << setw(28) << "Total Emission (kg CO2)"
             << "Avg per Resident" << endl;
        cout << string(65, '-') << endl;
        for (int m = 0; m < 6; m++) {
            if (modeCounts[m] == 0) continue;
            double avg = modeEmissions[m] / modeCounts[m];
            cout << left
                 << setw(15) << allModes[m]
                 << setw(8)  << modeCounts[m]
                 << setw(28) << fixed << setprecision(2) << modeEmissions[m]
                 << avg << " kg" << endl;
        }
        cout << string(65, '-') << endl;
        cout << "Grand Total Emissions: " << fixed << setprecision(2) << grandTotal << " kg CO2" << endl;

        // 5b: By city
        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 5B: CARBON EMISSIONS BY DATASET (CITY)" << endl;
        cout << string(65, '=') << endl;
        cout << left
             << setw(12) << "City"
             << setw(12) << "Residents"
             << setw(28) << "Total Emission (kg CO2)"
             << "Avg per Resident" << endl;
        cout << string(65, '-') << endl;
        for (int c = 0; c < 3; c++) {
            double avg = (cityCounts[c] > 0) ? cityEmissions[c] / cityCounts[c] : 0.0;
            cout << left
                 << setw(12) << cities[c]
                 << setw(12) << cityCounts[c]
                 << setw(28) << fixed << setprecision(2) << cityEmissions[c]
                 << avg << " kg" << endl;
        }
        cout << string(65, '-') << endl;
        cout << "Grand Total Emissions: " << fixed << setprecision(2) << grandTotal << " kg CO2" << endl;
        cout << "\n" << string(65, '=') << endl;
    }

    // --------------------------------------------------------
    // Task 6: Bubble Sort by Age (ascending)
    // Swaps Resident data between nodes (not pointers).
    // Displays first 10 and last 10 records before and after.
    // Time Complexity: O(n^2) | Space Complexity: O(1)
    // --------------------------------------------------------
    void sortByAge() {
        if (!head) return;
        bool swapped;
        do {
            swapped = false;
            NodeType* curr = head;
            while (curr->link != NULL) {
                if (curr->info.age > curr->link->info.age) {
                    swap(curr->info, curr->link->info);
                    swapped = true;
                }
                curr = curr->link;
            }
        } while (swapped);
    }

    void printFirstN(int n, string label) {
        cout << "\n" << label << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(15) << "Mode"
             << setw(10) << "Dist (km)"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        NodeType* curr = head;
        int count = 0;
        while (curr != NULL && count < n) {
            cout << left
                 << setw(10) << curr->info.id
                 << setw(6)  << curr->info.age
                 << setw(15) << curr->info.mode
                 << setw(10) << fixed << setprecision(2) << curr->info.dailyDist
                 << curr->info.monthlyEmission << " kg" << endl;
            curr = curr->link;
            count++;
        }
    }

    void printLastN(int n, string label) {
        // Collect last n nodes
        NodeType* nodes[10];
        int collected = 0;
        NodeType* curr = head;
        while (curr != NULL) {
            nodes[collected % n] = curr;
            collected++;
            curr = curr->link;
        }
        cout << "\n" << label << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(15) << "Mode"
             << setw(10) << "Dist (km)"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        int start = (collected >= n) ? collected % n : 0;
        for (int i = 0; i < n && i < collected; i++) {
            NodeType* node = nodes[(start + i) % n];
            cout << left
                 << setw(10) << node->info.id
                 << setw(6)  << node->info.age
                 << setw(15) << node->info.mode
                 << setw(10) << fixed << setprecision(2) << node->info.dailyDist
                 << node->info.monthlyEmission << " kg" << endl;
        }
    }

    void printTask6() {
        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 6: SORTING - BUBBLE SORT BY AGE (ASCENDING)" << endl;
        cout << string(65, '=') << endl;

        printFirstN(10, "[Before Sorting] - First 10 Records:");

        cout << "\nSorting all " << size << " residents by Age using Bubble Sort..." << endl;
        sortByAge();
        cout << "Sorting complete." << endl;

        printFirstN(10, "[After Sorting] - First 10 Records (Youngest):");
        printLastN(10,  "[After Sorting] - Last 10 Records (Oldest):");

        cout << "\n" << string(65, '=') << endl;
    }

    // --------------------------------------------------------
    // Task 7: Searching using Linear Search
    // Traverses the linked list node by node.
    // 7a: Search by age group
    // 7b: Search by mode of transport
    // 7c: Search by daily distance threshold (> x km)
    // --------------------------------------------------------
    void searchByAgeGroup(int lo, int hi, string label) {
        cout << "\nSearch: Age Group " << label << " (" << lo << " - " << hi << ")" << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(15) << "Mode"
             << setw(12) << "City"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        int count = 0;
        NodeType* curr = head;
        while (curr != NULL) {
            if (curr->info.age >= lo && curr->info.age <= hi) {
                cout << left
                     << setw(10) << curr->info.id
                     << setw(6)  << curr->info.age
                     << setw(15) << curr->info.mode
                     << setw(12) << curr->info.cityName
                     << fixed << setprecision(2) << curr->info.monthlyEmission << " kg" << endl;
                count++;
            }
            curr = curr->link;
        }
        cout << string(65, '-') << endl;
        cout << "Total residents found: " << count << endl;
    }

    void searchByMode(string targetMode) {
        cout << "\nSearch: Mode of Transport = " << targetMode << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(12) << "City"
             << setw(10) << "Dist (km)"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        int count = 0;
        NodeType* curr = head;
        while (curr != NULL) {
            if (curr->info.mode == targetMode) {
                cout << left
                     << setw(10) << curr->info.id
                     << setw(6)  << curr->info.age
                     << setw(12) << curr->info.cityName
                     << setw(10) << fixed << setprecision(2) << curr->info.dailyDist
                     << curr->info.monthlyEmission << " kg" << endl;
                count++;
            }
            curr = curr->link;
        }
        cout << string(65, '-') << endl;
        cout << "Total residents found: " << count << endl;
    }

    void searchByDistanceThreshold(double threshold) {
        cout << "\nSearch: Daily Distance > " << fixed << setprecision(1) << threshold << " km" << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(15) << "Mode"
             << setw(12) << "City"
             << setw(10) << "Dist (km)"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        int count = 0;
        NodeType* curr = head;
        while (curr != NULL) {
            if (curr->info.dailyDist > threshold) {
                cout << left
                     << setw(10) << curr->info.id
                     << setw(6)  << curr->info.age
                     << setw(15) << curr->info.mode
                     << setw(12) << curr->info.cityName
                     << setw(10) << fixed << setprecision(2) << curr->info.dailyDist
                     << curr->info.monthlyEmission << " kg" << endl;
                count++;
            }
            curr = curr->link;
        }
        cout << string(65, '-') << endl;
        cout << "Total residents found: " << count << endl;
    }

    void printTask7() {
        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 7: SEARCHING EXPERIMENTS (LINEAR SEARCH)" << endl;
        cout << string(65, '=') << endl;

        cout << "\n--- TASK 7A: SEARCH BY AGE GROUP ---" << endl;
        searchByAgeGroup(26, 45, "26-45");

        cout << "\n--- TASK 7B: SEARCH BY MODE OF TRANSPORT ---" << endl;
        searchByMode("Car");

        cout << "\n--- TASK 7C: SEARCH BY DAILY DISTANCE THRESHOLD ---" << endl;
        searchByDistanceThreshold(15.0);

        cout << "\n" << string(65, '=') << endl;
    }
};

// ============================================================
// Main
// ============================================================
int main() {
    CityList list;

    cout << string(65, '=') << endl;
    cout << "  LOADING DATASETS..." << endl;
    cout << string(65, '=') << endl;
    list.loadCSV("dataset1-cityA.csv", "City A");
    list.loadCSV("dataset2-cityB.csv", "City B");
    list.loadCSV("dataset3-cityC.csv", "City C");
    cout << "Total residents loaded: " << list.size << endl;

    // Task 4b: Age Group Analysis
    list.printTask4b();

    // Task 5: Emissions by Mode and City
    list.printTask5();

    // Task 6: Sorting
    list.printTask6();

    // Task 7: Searching
    list.printTask7();

    return 0;
}