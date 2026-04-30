/*
 * ============================================================
 *  CT077-3-2-DSTR  |  Lab Work #1
 *  Task 5: Carbon Emission Analysis  –  ARRAY Implementation
 * ============================================================
 *
 *  Covers:
 *    5a. Total carbon emissions per dataset (City A / B / C)
 *    5b. Carbon emissions per mode of transport
 *    5c. Emissions compared across datasets AND age groups
 *    5d. All output as formatted text tables
 *
 *  Formula used:
 *    Monthly Emission (kg CO2)
 *        = DailyDistance (km)
 *        x CarbonEmissionFactor (kg CO2/km)
 *        x AverageDayPerMonth
 *
 *  Compile:  g++ -std=c++11 -o task5_array task5_array.cpp
 *  Run:      ./task5_array
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
using namespace std;

// ─────────────────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────────────────
const int MAX_RESIDENTS  = 500;   // max rows across all three CSVs
const int NUM_CITIES     = 3;
const int NUM_AGE_GROUPS = 5;
const int NUM_MODES      = 8;     // distinct transport modes across all cities

// ─────────────────────────────────────────────────────────
//  Resident struct
// ─────────────────────────────────────────────────────────
struct Resident {
    char   residentID[10];
    int    age;
    char   transportMode[20];
    double dailyDistance;          // km
    double carbonEmissionFactor;   // kg CO2 / km
    int    avgDaysPerMonth;
    int    cityID;                 // 1 = City A, 2 = City B, 3 = City C

    // ── Derived: monthly carbon emission ──────────────
    double monthlyEmission() const {
        return dailyDistance * carbonEmissionFactor * avgDaysPerMonth;
    }


    int ageGroupIndex() const {
        if (age >=  6 && age <= 17) return 0;
        if (age >= 18 && age <= 25) return 1;
        if (age >= 26 && age <= 45) return 2;
        if (age >= 46 && age <= 60) return 3;
        return 4;                             // 61-100
    }
};


Resident residents[MAX_RESIDENTS];
int      totalResidents = 0;



const char* CITY_NAMES[NUM_CITIES] = {
    "City A - Metropolitan City",
    "City B - University Town  ",
    "City C - Suburban/Rural   "
};

const char* AGE_LABELS[NUM_AGE_GROUPS] = {
    "6-17   (Children & Teenagers)     ",
    "18-25  (University / Young Adults)",
    "26-45  (Working Adults Early)     ",
    "46-60  (Working Adults Late)      ",
    "61-100 (Senior Citizens)          "
};



void loadCSV(const char* filename, int cityID) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Cannot open: " << filename << "\n";
        return;
    }

    string line;
    getline(file, line);          

    while (getline(file, line) && totalResidents < MAX_RESIDENTS) {
        if (line.empty()) continue;


        if (!line.empty() && line.back() == '\r') line.pop_back();

        istringstream ss(line);
        string token;
        Resident r;
        r.cityID = cityID;


        getline(ss, token, ',');
        strncpy(r.residentID, token.c_str(), sizeof(r.residentID) - 1);
        r.residentID[sizeof(r.residentID)-1] = '\0';


        getline(ss, token, ',');
        r.age = stoi(token);


        getline(ss, token, ',');
        strncpy(r.transportMode, token.c_str(), sizeof(r.transportMode) - 1);
        r.transportMode[sizeof(r.transportMode)-1] = '\0';


        getline(ss, token, ',');
        r.dailyDistance = stod(token);


        getline(ss, token, ',');
        r.carbonEmissionFactor = stod(token);


        getline(ss, token, ',');
        r.avgDaysPerMonth = stoi(token);

        residents[totalResidents++] = r;
    }
    file.close();
}



void printLine(char ch, int len) {
    for (int i = 0; i < len; i++) cout << ch;
    cout << "\n";
}

void printHeader(const string& title) {
    cout << "\n";
    printLine('=', 76);
    cout << "  " << title << "\n";
    printLine('=', 76);
}


//  TASK 5a – Total carbon emissions per dataset

void task5a_totalPerDataset() {
    printHeader("TASK 5a | Total Carbon Emissions Per Dataset ");

    double cityTotal[NUM_CITIES] = {0.0, 0.0, 0.0};
    int    cityCount[NUM_CITIES] = {0,   0,   0  };


    for (int i = 0; i < totalResidents; i++) {
        int c = residents[i].cityID - 1;
        cityTotal[c] += residents[i].monthlyEmission();
        cityCount[c]++;
    }


    cout << left
         << setw(30) << "Dataset"
         << setw(12) << "Residents"
         << setw(24) << "Total Emission (kg CO2)"
         << setw(22) << "Avg / Resident (kg CO2)"
         << "\n";
    printLine('-', 76);

    double grandTotal = 0.0;
    int    grandCount = 0;

    for (int c = 0; c < NUM_CITIES; c++) {
        double avg = (cityCount[c] > 0) ? cityTotal[c] / cityCount[c] : 0.0;
        cout << left
             << setw(30) << CITY_NAMES[c]
             << setw(12) << cityCount[c]
             << fixed << setprecision(2)
             << setw(24) << cityTotal[c]
             << setw(22) << avg
             << "\n";
        grandTotal += cityTotal[c];
        grandCount += cityCount[c];
    }

    printLine('-', 76);
    cout << left
         << setw(30) << "GRAND TOTAL (All Cities)"
         << setw(12) << grandCount
         << fixed << setprecision(2)
         << setw(24) << grandTotal
         << setw(22) << (grandTotal / grandCount)
         << "\n";
}


//  TASK 5b – Carbon emissions per mode of transport

void task5b_emissionsPerMode() {
    printHeader("TASK 5b | Carbon Emissions Per Mode of Transport  ");

    // Collect unique mode names (no STL containers – manual array)
    char   modeNames [NUM_MODES][20] = {};
    double modeTotal [NUM_MODES]     = {};
    int    modeCount [NUM_MODES]     = {};
    int    numModes  = 0;

    for (int i = 0; i < totalResidents; i++) {
        // Search for existing mode
        int idx = -1;
        for (int m = 0; m < numModes; m++) {
            if (strcmp(modeNames[m], residents[i].transportMode) == 0) {
                idx = m; break;
            }
        }

        if (idx == -1 && numModes < NUM_MODES) {
            idx = numModes;
            strncpy(modeNames[numModes], residents[i].transportMode,
                    sizeof(modeNames[0]) - 1);
            numModes++;
        }
        if (idx >= 0) {
            modeTotal[idx] += residents[i].monthlyEmission();
            modeCount[idx]++;
        }
    }


    cout << left
         << setw(15) << "Mode"
         << setw(12) << "Residents"
         << setw(24) << "Total Emission (kg CO2)"
         << setw(22) << "Avg / Resident (kg CO2)"
         << "\n";
    printLine('-', 76);

    double grandTotal = 0.0;
    for (int m = 0; m < numModes; m++) {
        double avg = (modeCount[m] > 0) ? modeTotal[m] / modeCount[m] : 0.0;
        cout << left
             << setw(15) << modeNames[m]
             << setw(12) << modeCount[m]
             << fixed << setprecision(2)
             << setw(24) << modeTotal[m]
             << setw(22) << avg
             << "\n";
        grandTotal += modeTotal[m];
    }

    printLine('-', 76);
    cout << "  Total Emissions Across All Modes: "
         << fixed << setprecision(2) << grandTotal << " kg CO2\n";
}


string mostPreferredMode(int ageGroupIdx) {
    char   modeName[NUM_MODES][20] = {};
    int    modeFreq[NUM_MODES]     = {};
    int    numModes = 0;

    for (int i = 0; i < totalResidents; i++) {
        if (residents[i].ageGroupIndex() != ageGroupIdx) continue;
        int idx = -1;
        for (int m = 0; m < numModes; m++)
            if (strcmp(modeName[m], residents[i].transportMode) == 0)
                { idx = m; break; }
        if (idx == -1 && numModes < NUM_MODES) {
            idx = numModes;
            strncpy(modeName[numModes], residents[i].transportMode,
                    sizeof(modeName[0]) - 1);
            numModes++;
        }
        if (idx >= 0) modeFreq[idx]++;
    }

    if (numModes == 0) return "N/A";

    int bestIdx = 0;
    for (int m = 1; m < numModes; m++)
        if (modeFreq[m] > modeFreq[bestIdx]) bestIdx = m;

    return string(modeName[bestIdx]);
}


//  TASK 5c (part 1) – Emissions per age group


void task5c_perAgeGroup() {
    printHeader("TASK 5c | Carbon Emissions Per Age Group  ");

    for (int ag = 0; ag < NUM_AGE_GROUPS; ag++) {

        char   modeName[NUM_MODES][20] = {};
        double modeEmit[NUM_MODES]     = {};
        int    modeCnt [NUM_MODES]     = {};
        int    numModes = 0;

        double groupTotal = 0.0;
        int    groupCount = 0;

        for (int i = 0; i < totalResidents; i++) {
            if (residents[i].ageGroupIndex() != ag) continue;

            double emit = residents[i].monthlyEmission();
            groupTotal += emit;
            groupCount++;


            int idx = -1;
            for (int m = 0; m < numModes; m++)
                if (strcmp(modeName[m], residents[i].transportMode) == 0)
                    { idx = m; break; }
            if (idx == -1 && numModes < NUM_MODES) {
                idx = numModes;
                strncpy(modeName[numModes], residents[i].transportMode,
                        sizeof(modeName[0]) - 1);
                numModes++;
            }
            if (idx >= 0) { modeEmit[idx] += emit; modeCnt[idx]++; }
        }


        cout << "\nAge Group: " << AGE_LABELS[ag] << "\n";
        printLine('-', 68);
        cout << left
             << setw(15) << "Mode"
             << setw(10) << "Count"
             << setw(24) << "Total Emission (kg CO2)"
             << setw(22) << "Avg / Resident"
             << "\n";
        printLine('-', 68);

        if (groupCount == 0) {
            cout << "  (No residents in this age group)\n";
            continue;
        }

        for (int m = 0; m < numModes; m++) {
            double avg = (modeCnt[m] > 0) ? modeEmit[m] / modeCnt[m] : 0.0;
            cout << left
                 << setw(15) << modeName[m]
                 << setw(10) << modeCnt[m]
                 << fixed << setprecision(2)
                 << setw(24) << modeEmit[m]
                 << setw(22) << avg
                 << "\n";
        }

        printLine('-', 68);
        string preferred = mostPreferredMode(ag);
        double avgGroup  = groupTotal / groupCount;
        cout << "  Total Emission for Age Group  : "
             << fixed << setprecision(2) << groupTotal << " kg CO2\n";
        cout << "  Most Preferred Mode           : " << preferred << "\n";
        cout << "  Average Emission per Resident : "
             << fixed << setprecision(2) << avgGroup << " kg CO2\n";
    }
}


//  TASK 5c (part 2) – Cross-dataset × age-group matrix

void task5c_crossDatasetMatrix() {
    printHeader("TASK 5c | Cross-Dataset Emission Matrix (City x Age Group)  ");

    double matrix[NUM_CITIES][NUM_AGE_GROUPS] = {};
    int    counts[NUM_CITIES][NUM_AGE_GROUPS] = {};

    for (int i = 0; i < totalResidents; i++) {
        int ci  = residents[i].cityID - 1;
        int agi = residents[i].ageGroupIndex();
        matrix[ci][agi] += residents[i].monthlyEmission();
        counts[ci][agi]++;
    }

    const char* shortAge[NUM_AGE_GROUPS] = {
        "6-17", "18-25", "26-45", "46-60", "61-100"
    };

    cout << left << setw(30) << "City / Age Group";
    for (int ag = 0; ag < NUM_AGE_GROUPS; ag++)
        cout << setw(12) << shortAge[ag];
    cout << setw(12) << "Total" << "\n";
    printLine('-', 30 + 12 * NUM_AGE_GROUPS + 12);

    double colSum[NUM_AGE_GROUPS] = {};
    double grandTotal = 0.0;

    for (int ci = 0; ci < NUM_CITIES; ci++) {
        cout << left << setw(30) << CITY_NAMES[ci];
        double rowTotal = 0.0;
        for (int ag = 0; ag < NUM_AGE_GROUPS; ag++) {
            cout << fixed << setprecision(1) << setw(12) << matrix[ci][ag];
            rowTotal    += matrix[ci][ag];
            colSum[ag]  += matrix[ci][ag];
            grandTotal  += matrix[ci][ag];
        }
        cout << fixed << setprecision(1) << setw(12) << rowTotal << "\n";
    }

    printLine('-', 30 + 12 * NUM_AGE_GROUPS + 12);
    cout << left << setw(30) << "Total (all cities)";
    for (int ag = 0; ag < NUM_AGE_GROUPS; ag++)
        cout << fixed << setprecision(1) << setw(12) << colSum[ag];
    cout << fixed << setprecision(1) << setw(12) << grandTotal << "\n";
    cout << "(All values in kg CO2 per month)\n";
}


//  Main

int main() {
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║  Task 5: Carbon Emission Analysis                        ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";

    loadCSV("dataset1-cityA.csv", 1);
    loadCSV("dataset2-cityB.csv", 2);
    loadCSV("dataset3-cityC.csv", 3);

    cout << "\n[INFO] Residents loaded -> "
         << "City A: ";
    int ca=0, cb=0, cc=0;
    for(int i=0;i<totalResidents;i++){
        if(residents[i].cityID==1) ca++;
        else if(residents[i].cityID==2) cb++;
        else cc++;
    }
    cout << ca << "  City B: " << cb << "  City C: " << cc
         << "  (Total: " << totalResidents << ")\n";

    task5a_totalPerDataset();
    task5b_emissionsPerMode();
    task5c_perAgeGroup();
    task5c_crossDatasetMatrix();

    cout << "\n[Done] Task 5 Carbon Emission Analysis complete.\n\n";
    return 0;
}
