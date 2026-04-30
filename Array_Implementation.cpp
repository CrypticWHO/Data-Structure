#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// ============================================================
// Struct: Resident
// Stores all data fields for a single resident from the CSV.
// monthlyEmission is computed via calculate().
// ============================================================
struct Resident {
    string id;
    int age;
    string mode;
    double dist;
    double factor;
    int days;
    double monthlyEmission;
    string cityName;

    void calculate() {
        monthlyEmission = dist * factor * days;
    }
};

// ============================================================
// Class: CityArray
// Stores all residents from all three datasets in a single
// dynamically allocated array.
// ============================================================
class CityArray {
public:
    Resident* list;
    int size;
    int capacity;

    CityArray(int cap) {
        capacity = cap;
        list = new Resident[capacity];
        size = 0;
    }

    ~CityArray() { delete[] list; }

    // --------------------------------------------------------
    // loadCSV: Reads a CSV file and appends residents to array
    // --------------------------------------------------------
    void loadCSV(string filename, string city) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file " << filename << endl;
            return;
        }

        string line, word;
        getline(file, line); // Skip header row

        while (getline(file, line) && size < capacity) {
            if (line.empty()) continue;

            // Remove carriage return if present (Windows line endings)
            if (!line.empty() && line.back() == '\r')
                line.pop_back();

            stringstream ss(line);
            Resident r;
            getline(ss, r.id,   ',');
            getline(ss, word,   ','); r.age    = stoi(word);
            getline(ss, r.mode, ',');
            getline(ss, word,   ','); r.dist   = stod(word);
            getline(ss, word,   ','); r.factor = stod(word);
            getline(ss, word,   ','); r.days   = stoi(word);
            r.cityName = city;
            r.calculate();
            list[size++] = r;
        }
        file.close();
    }

    // --------------------------------------------------------
    // Task 4b: For each age group print per-mode breakdown:
    //   Count, Total Emission, Avg per Resident,
    //   Total & Average for group, Most Preferred Mode
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

            for (int i = 0; i < size; i++) {
                int a = list[i].age;
                if (a < ageLow[g] || a > ageHigh[g]) continue;
                groupCount++;
                groupTotal += list[i].monthlyEmission;
                for (int m = 0; m < 6; m++) {
                    if (list[i].mode == allModes[m]) {
                        modeEmissions[m] += list[i].monthlyEmission;
                        modeCounts[m]++;
                        break;
                    }
                }
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

    // ============================================================
    // TASK 5: CARBON EMISSION ANALYSIS
    //
    // 5a. Total carbon emissions per dataset (City A / B / C)
    // 5b. Carbon emissions per mode of transport
    // 5c. Emissions compared across datasets and age groups
    // 5d. All results displayed as formatted text tables
    //
    // Formula:
    //   Monthly Emission (kg CO2) = dist x factor x days
    //   (already stored in list[i].monthlyEmission via calculate())
    // ============================================================
    void printTask5() {

        // ── Shared lookup data ────────────────────────────────
        string cityLabels[3]  = {"City A", "City B", "City C"};
        string cityFull[3]    = {
            "City A - Metropolitan City",
            "City B - University Town  ",
            "City C - Suburban/Rural   "
        };
        string allModes[6]    = {
            "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"
        };
        string ageLabels[5]   = {
            "6-17   (Children & Teenagers)     ",
            "18-25  (University / Young Adults)",
            "26-45  (Working Adults Early)     ",
            "46-60  (Working Adults Late)      ",
            "61-100 (Senior Citizens)          "
        };
        string ageShort[5]    = {"6-17","18-25","26-45","46-60","61-100"};
        int    ageLow[5]      = {6,  18, 26, 46, 61};
        int    ageHigh[5]     = {17, 25, 45, 60, 100};

        // Helper lambda: returns age-group index (0-4) for a given age
        // Written as an inline function since lambdas need C++11
        // ── we just inline the lookup where needed below ──────

        cout << "\n" << string(76, '=') << endl;
        cout << "  TASK 5: CARBON EMISSION ANALYSIS  [Array]" << endl;
        cout << string(76, '=') << endl;

        // ════════════════════════════════════════════════════════
        // TASK 5a – Total carbon emissions per dataset
        // Single pass through the array; accumulate by cityName.
        // ════════════════════════════════════════════════════════
        cout << "\n" << string(76, '-') << endl;
        cout << "  TASK 5a | Total Carbon Emissions Per Dataset" << endl;
        cout << string(76, '-') << endl;

        double cityTotal[3] = {0.0, 0.0, 0.0};
        int    cityCount[3] = {0,   0,   0  };

        for (int i = 0; i < size; i++) {
            for (int c = 0; c < 3; c++) {
                if (list[i].cityName == cityLabels[c]) {
                    cityTotal[c] += list[i].monthlyEmission;
                    cityCount[c]++;
                    break;
                }
            }
        }

        cout << left
             << setw(30) << "Dataset"
             << setw(12) << "Residents"
             << setw(24) << "Total Emission (kg CO2)"
             << "Avg / Resident (kg CO2)" << endl;
        cout << string(76, '-') << endl;

        double grandTotal5a = 0.0;
        int    grandCount5a = 0;
        for (int c = 0; c < 3; c++) {
            double avg = (cityCount[c] > 0) ? cityTotal[c] / cityCount[c] : 0.0;
            cout << left
                 << setw(30) << cityFull[c]
                 << setw(12) << cityCount[c]
                 << fixed << setprecision(2)
                 << setw(24) << cityTotal[c]
                 << avg << " kg CO2" << endl;
            grandTotal5a += cityTotal[c];
            grandCount5a += cityCount[c];
        }
        cout << string(76, '-') << endl;
        cout << left
             << setw(30) << "GRAND TOTAL (All Cities)"
             << setw(12) << grandCount5a
             << fixed << setprecision(2)
             << setw(24) << grandTotal5a
             << (grandTotal5a / grandCount5a) << " kg CO2" << endl;

        // ════════════════════════════════════════════════════════
        // TASK 5b – Carbon emissions per mode of transport
        // Single pass; match against the 6 known modes.
        // ════════════════════════════════════════════════════════
        cout << "\n" << string(76, '-') << endl;
        cout << "  TASK 5b | Carbon Emissions Per Mode of Transport" << endl;
        cout << string(76, '-') << endl;

        double modeTotal[6] = {0,0,0,0,0,0};
        int    modeCount[6] = {0,0,0,0,0,0};

        for (int i = 0; i < size; i++) {
            for (int m = 0; m < 6; m++) {
                if (list[i].mode == allModes[m]) {
                    modeTotal[m] += list[i].monthlyEmission;
                    modeCount[m]++;
                    break;
                }
            }
        }

        cout << left
             << setw(15) << "Mode"
             << setw(12) << "Residents"
             << setw(24) << "Total Emission (kg CO2)"
             << "Avg / Resident (kg CO2)" << endl;
        cout << string(76, '-') << endl;

        double grandTotal5b = 0.0;
        for (int m = 0; m < 6; m++) {
            if (modeCount[m] == 0) continue;
            double avg = modeTotal[m] / modeCount[m];
            cout << left
                 << setw(15) << allModes[m]
                 << setw(12) << modeCount[m]
                 << fixed << setprecision(2)
                 << setw(24) << modeTotal[m]
                 << avg << " kg CO2" << endl;
            grandTotal5b += modeTotal[m];
        }
        cout << string(76, '-') << endl;
        cout << "Total Emissions Across All Modes: "
             << fixed << setprecision(2) << grandTotal5b << " kg CO2" << endl;

        // ════════════════════════════════════════════════════════
        // TASK 5c (part 1) – Emissions per age group
        // Matches the sample table format shown in the brief.
        // ════════════════════════════════════════════════════════
        cout << "\n" << string(76, '-') << endl;
        cout << "  TASK 5c | Carbon Emissions Per Age Group" << endl;
        cout << string(76, '-') << endl;

        for (int g = 0; g < 5; g++) {
            // Per-mode breakdown within this age group
            double ageModeEmit[6] = {0,0,0,0,0,0};
            int    ageModeCnt[6]  = {0,0,0,0,0,0};
            double groupTotal     = 0.0;
            int    groupCount     = 0;

            for (int i = 0; i < size; i++) {
                int a = list[i].age;
                if (a < ageLow[g] || a > ageHigh[g]) continue;
                groupTotal += list[i].monthlyEmission;
                groupCount++;
                for (int m = 0; m < 6; m++) {
                    if (list[i].mode == allModes[m]) {
                        ageModeEmit[m] += list[i].monthlyEmission;
                        ageModeCnt[m]++;
                        break;
                    }
                }
            }

            // Most preferred mode: highest count in this age group
            int prefIdx = 0;
            for (int m = 1; m < 6; m++)
                if (ageModeCnt[m] > ageModeCnt[prefIdx]) prefIdx = m;

            cout << "\nAge Group: " << ageLabels[g] << endl;
            cout << string(68, '-') << endl;
            cout << left
                 << setw(15) << "Mode"
                 << setw(10) << "Count"
                 << setw(24) << "Total Emission (kg CO2)"
                 << "Avg / Resident" << endl;
            cout << string(68, '-') << endl;

            if (groupCount == 0) {
                cout << "  (No residents in this age group)" << endl;
                continue;
            }

            for (int m = 0; m < 6; m++) {
                if (ageModeCnt[m] == 0) continue;
                double avg = ageModeEmit[m] / ageModeCnt[m];
                cout << left
                     << setw(15) << allModes[m]
                     << setw(10) << ageModeCnt[m]
                     << fixed << setprecision(2)
                     << setw(24) << ageModeEmit[m]
                     << avg << " kg CO2" << endl;
            }

            cout << string(68, '-') << endl;
            cout << "Total Emission for Age Group  : "
                 << fixed << setprecision(2) << groupTotal << " kg CO2" << endl;
            cout << "Most Preferred Mode           : "
                 << allModes[prefIdx]
                 << " (" << ageModeCnt[prefIdx] << " residents)" << endl;
            cout << "Average Emission per Resident : "
                 << fixed << setprecision(2) << (groupTotal / groupCount)
                 << " kg CO2" << endl;
        }

        // ════════════════════════════════════════════════════════
        // TASK 5c (part 2) – Cross-dataset × age-group matrix
        // Rows = cities, Columns = age groups, values = kg CO2
        // ════════════════════════════════════════════════════════
        cout << "\n" << string(76, '-') << endl;
        cout << "  TASK 5c | Cross-Dataset Emission Matrix (City x Age Group)" << endl;
        cout << string(76, '-') << endl;

        double matrix[3][5] = {};   // [city][ageGroup]

        for (int i = 0; i < size; i++) {
            // Identify city index
            int ci = -1;
            for (int c = 0; c < 3; c++)
                if (list[i].cityName == cityLabels[c]) { ci = c; break; }

            // Identify age-group index
            int gi = -1;
            for (int g = 0; g < 5; g++)
                if (list[i].age >= ageLow[g] && list[i].age <= ageHigh[g])
                    { gi = g; break; }

            if (ci >= 0 && gi >= 0)
                matrix[ci][gi] += list[i].monthlyEmission;
        }

        // Header row
        cout << left << setw(28) << "City \\ Age Group";
        for (int g = 0; g < 5; g++)
            cout << setw(12) << ageShort[g];
        cout << setw(12) << "Total" << endl;
        cout << string(28 + 12*5 + 12, '-') << endl;

        double colSum[5]    = {};
        double grandTotal5c = 0.0;

        for (int c = 0; c < 3; c++) {
            cout << left << setw(28) << cityFull[c];
            double rowTotal = 0.0;
            for (int g = 0; g < 5; g++) {
                cout << fixed << setprecision(1) << setw(12) << matrix[c][g];
                rowTotal      += matrix[c][g];
                colSum[g]     += matrix[c][g];
                grandTotal5c  += matrix[c][g];
            }
            cout << fixed << setprecision(1) << setw(12) << rowTotal << endl;
        }

        cout << string(28 + 12*5 + 12, '-') << endl;
        cout << left << setw(28) << "Total (all cities)";
        for (int g = 0; g < 5; g++)
            cout << fixed << setprecision(1) << setw(12) << colSum[g];
        cout << fixed << setprecision(1) << setw(12) << grandTotal5c << endl;
        cout << "(All values in kg CO2 per month)" << endl;

        cout << "\n" << string(76, '=') << endl;
        cout << "  END OF TASK 5" << endl;
        cout << string(76, '=') << endl;
    }
    // ── END OF TASK 5 ─────────────────────────────────────────

    // --------------------------------------------------------
    // Task 6: Bubble Sort by Age (ascending)
    // Displays first 10 and last 10 records before and after.
    // Time Complexity: O(n^2) | Space Complexity: O(1)
    // --------------------------------------------------------
    void bubbleSortByAge() {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (list[j].age > list[j + 1].age) {
                    Resident temp = list[j];
                    list[j]       = list[j + 1];
                    list[j + 1]   = temp;
                }
            }
        }
    }

    void printRecords(int from, int to, string label) {
        cout << "\n" << label << endl;
        cout << string(65, '-') << endl;
        cout << left
             << setw(10) << "ID"
             << setw(6)  << "Age"
             << setw(15) << "Mode"
             << setw(10) << "Dist (km)"
             << "Emission (kg)" << endl;
        cout << string(65, '-') << endl;
        for (int i = from; i < to && i < size; i++) {
            cout << left
                 << setw(10) << list[i].id
                 << setw(6)  << list[i].age
                 << setw(15) << list[i].mode
                 << setw(10) << fixed << setprecision(2) << list[i].dist
                 << list[i].monthlyEmission << " kg" << endl;
        }
    }

    void printTask6() {
        cout << "\n" << string(65, '=') << endl;
        cout << "  TASK 6: SORTING - BUBBLE SORT BY AGE (ASCENDING)" << endl;
        cout << string(65, '=') << endl;

        printRecords(0, 10, "[Before Sorting] - First 10 Records:");

        cout << "\nSorting all " << size << " residents by Age using Bubble Sort..." << endl;
        bubbleSortByAge();
        cout << "Sorting complete." << endl;

        printRecords(0, 10,        "[After Sorting] - First 10 Records (Youngest):");
        printRecords(size - 10, size, "[After Sorting] - Last 10 Records (Oldest):");

        cout << "\n" << string(65, '=') << endl;
    }

    // --------------------------------------------------------
    // Task 7: Searching using Linear Search
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
        for (int i = 0; i < size; i++) {
            if (list[i].age >= lo && list[i].age <= hi) {
                cout << left
                     << setw(10) << list[i].id
                     << setw(6)  << list[i].age
                     << setw(15) << list[i].mode
                     << setw(12) << list[i].cityName
                     << fixed << setprecision(2) << list[i].monthlyEmission << " kg" << endl;
                count++;
            }
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
        for (int i = 0; i < size; i++) {
            if (list[i].mode == targetMode) {
                cout << left
                     << setw(10) << list[i].id
                     << setw(6)  << list[i].age
                     << setw(12) << list[i].cityName
                     << setw(10) << fixed << setprecision(2) << list[i].dist
                     << list[i].monthlyEmission << " kg" << endl;
                count++;
            }
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
        for (int i = 0; i < size; i++) {
            if (list[i].dist > threshold) {
                cout << left
                     << setw(10) << list[i].id
                     << setw(6)  << list[i].age
                     << setw(15) << list[i].mode
                     << setw(12) << list[i].cityName
                     << setw(10) << fixed << setprecision(2) << list[i].dist
                     << list[i].monthlyEmission << " kg" << endl;
                count++;
            }
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
    CityArray myCities(3000);

    cout << string(65, '=') << endl;
    cout << "  LOADING DATASETS..." << endl;
    cout << string(65, '=') << endl;
    myCities.loadCSV("dataset1-cityA.csv", "City A");
    myCities.loadCSV("dataset2-cityB.csv", "City B");
    myCities.loadCSV("dataset3-cityC.csv", "City C");
    cout << "Total residents loaded: " << myCities.size << endl;

    // Task 4b: Age Group Analysis
    myCities.printTask4b();

    // Task 5: Carbon Emission Analysis
    myCities.printTask5();

    // Task 6: Sorting
    myCities.printTask6();

    // Task 7: Searching
    myCities.printTask7();

    return 0;
}
