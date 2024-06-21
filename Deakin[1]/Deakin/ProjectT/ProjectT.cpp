#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <limits>
#include <cmath>
#include <sys/stat.h> // For creating directories (For TDM-GCC)

#ifdef _WIN32
#include <direct.h> // For creating directories (For Windows)
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace std;

struct TemperatureData {
    string productCode;
    string stationNumber;
    int year;
    int month;
    int day;
    double maxTemperature;
    int daysOfAccumulation;
    string quality;
};

// Function to read temperature data from a CSV file
vector<TemperatureData> ReadTemperatureData(const string& filename) {
    vector<TemperatureData> data;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Unable to open the file." << endl;
        return data;
    }

    string line;
    getline(file, line); // Read and discard the header line

    while (getline(file, line)) {
        stringstream ss(line);
        TemperatureData entry;

        getline(ss, entry.productCode, ',');
        getline(ss, entry.stationNumber, ',');
        ss >> entry.year;
        ss.ignore(); // Ignore the comma
        ss >> entry.month;
        ss.ignore();
        ss >> entry.day;
        ss.ignore();
        ss >> entry.maxTemperature;
        ss.ignore();
        ss >> entry.daysOfAccumulation;
        ss.ignore();
        getline(ss, entry.quality);

        data.push_back(entry);
    }

    file.close();
    return data;
}

// Function to calculate variance
double CalculateVariance(const vector<double>& values) {
    double sum = 0.0;
    for (double val : values) {
        sum += val;
    }
    double mean = sum / values.size();

    double variance = 0.0;
    for (double val : values) {
        variance += pow(val - mean, 2);
    }

    return variance / values.size();
}

// Function to filter data by year
vector<TemperatureData> FilterDataByYear(const vector<TemperatureData>& data, int year) {
    vector<TemperatureData> filteredData;
    for (const TemperatureData& entry : data) {
        if (entry.year == year) {
            filteredData.push_back(entry);
        }
    }
    return filteredData;
}

// Function to filter data by month
vector<TemperatureData> FilterDataByMonth(const vector<TemperatureData>& data, int month) {
    vector<TemperatureData> filteredData;
    for (const TemperatureData& entry : data) {
        if (entry.month == month) {
            filteredData.push_back(entry);
        }
    }
    return filteredData;
}

// Function to find the mode temperature
double FindMode(const vector<TemperatureData>& data) {
    // Create a map to store temperature frequencies
    map<double, int> freq;
    for (const TemperatureData& entry : data) {
        freq[entry.maxTemperature]++;
    }

    // Find the temperature with the highest frequency (mode)
    double mode = 0.0;
    int maxFreq = 0;
    for (const auto& pair : freq) {
        if (pair.second > maxFreq) {
            maxFreq = pair.second;
            mode = pair.first;
        }
    }

    return mode;
}

// Function to find the month with the coldest average temperature
void FindColdestMonth(const vector<TemperatureData>& data) {
    // Create a map to store the average temperatures for each month
    map<int, double> avgTemperatures;

    for (int month = 1; month <= 12; month++) {
        vector<TemperatureData> monthData = FilterDataByMonth(data, month);

        if (!monthData.empty()) {
            double sum = 0.0;
            for (const TemperatureData& entry : monthData) {
                sum += entry.maxTemperature;
            }
            double avgTemp = sum / monthData.size();
            avgTemperatures[month] = avgTemp;
        }
    }

    // Find the coldest month
    int coldestMonth = 0;
    double minAvgTemp = numeric_limits<double>::max();
    for (const auto& pair : avgTemperatures) {
        if (pair.second < minAvgTemp) {
            minAvgTemp = pair.second;
            coldestMonth = pair.first;
        }
    }

    cout << "Coldest month: " << coldestMonth << " Average Temperature: " << minAvgTemp << endl;
}

// Function to find the month with the hottest average temperature
void FindHottestMonth(const vector<TemperatureData>& data) {
    // Create a map to store the average temperatures for each month
    map<int, double> avgTemperatures;

    for (int month = 1; month <= 12; month++) {
        vector<TemperatureData> monthData = FilterDataByMonth(data, month);

        if (!monthData.empty()) {
            double sum = 0.0;
            for (const TemperatureData& entry : monthData) {
                sum += entry.maxTemperature;
            }
            double avgTemp = sum / monthData.size();
            avgTemperatures[month] = avgTemp;
        }
    }

    // Find the hottest month
    int hottestMonth = 0;
    double maxAvgTemp = numeric_limits<double>::min();
    for (const auto& pair : avgTemperatures) {
        if (pair.second > maxAvgTemp) {
            maxAvgTemp = pair.second;
            hottestMonth = pair.first;
        }
    }

    cout << "Hottest month: " << hottestMonth << " Average Temperature: " << maxAvgTemp << endl;
}

//Function to Calculate the most consistant month
void FindMostConsistentMonth(const vector<TemperatureData>& data) {
    map<int, vector<double>> monthToTemperatures; // Map to store temperatures for each month

    // Populate the map
    for (const TemperatureData& entry : data) {
        int month = entry.month;
        monthToTemperatures[month].push_back(entry.maxTemperature);
    }

    // Calculate variance for each month
    map<int, double> monthToVariance;
    for (const auto& pair : monthToTemperatures) {
        int month = pair.first;
        const vector<double>& temperatures = pair.second;
        double variance = CalculateVariance(temperatures);
        monthToVariance[month] = variance;
    }

    // Find the month with the smallest variance
    int mostConsistentMonth = -1;
    double smallestVariance = numeric_limits<double>::max();
    for (const auto& pair : monthToVariance) {
        int month = pair.first;
        double variance = pair.second;
        if (variance < smallestVariance) {
            smallestVariance = variance;
            mostConsistentMonth = month;
        }
    }

    // Calculate average temperature for the most consistent month
    vector<TemperatureData> consistentMonthData = FilterDataByMonth(data, mostConsistentMonth);
    double sum = 0.0;
    for (const TemperatureData& entry : consistentMonthData) {
        sum += entry.maxTemperature;
    }
    double avgTemp = sum / consistentMonthData.size();

    cout << "Most consistent month: " << mostConsistentMonth << " Average Temperature: " << avgTemp
        << " Variance: " << smallestVariance << endl;
}

// Function to calculate the percentage of days within one standard deviation
void CalculatePercentageWithinOneStdDev(const vector<TemperatureData>& data) {
    vector<double> temperatures;
    for (const TemperatureData& entry : data) {
        temperatures.push_back(entry.maxTemperature);
    }

    double mean = CalculateVariance(temperatures);
    double stdDev = sqrt(mean);

    int countWithinOneStdDev = 0;
    for (const TemperatureData& entry : data) {
        if (abs(entry.maxTemperature - mean) <= stdDev) {
            countWithinOneStdDev++;
        }
    }

    double percentage = (static_cast<double>(countWithinOneStdDev) / data.size()) * 100;

    cout << "Percentage of days within one standard deviation: " << fixed << setprecision(2) << percentage << "%" << endl;
}

// Function to filter data by year, excluding entries with missing information
vector<TemperatureData> FilterDataByYearExcludeMissing(const vector<TemperatureData>& data, int year) {
    vector<TemperatureData> filteredData;
    for (const TemperatureData& entry : data) {
        if (entry.year == year && entry.maxTemperature != -9999.0) {
            filteredData.push_back(entry);
        }
    }
    return filteredData;
}

// Function to display start and end dates of the data
void DisplayDataStartEndDate(const vector<TemperatureData>& data) {
    if (data.empty()) {
        cout << "No data available." << endl;
        return;
    }

    int startYear = data.front().year;
    int endYear = data.back().year;

    int startMonth = data.front().month;
    int endMonth = data.back().month;

    int startDay = data.front().day;
    int endDay = data.back().day;

    cout << "Data available from " << startYear << "-" << startMonth << "-" << startDay << " to " << endYear << "-" << endMonth << "-" << endDay << endl;
}



// Function to write results to a CSV file in the "Results" folder
void WriteResultsToCSV(const string& filename, const vector<TemperatureData>& data) {
    string folderName = "Results";

#ifdef _WIN32
    // Create a new directory called "Results" if it doesn't exist (For Windows)
    if (_mkdir(folderName.c_str()) != 0 && errno != EEXIST) {
        cerr << "Error creating the 'Results' directory." << endl;
        return;
    }
#else
    // Create a new directory called "Results" if it doesn't exist (For TDM-GCC)
    struct stat info;
    if (stat(folderName.c_str(), &info) != 0) {
        if (mkdir(folderName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            cerr << "Error creating the 'Results' directory." << endl;
            return;
        }
    }
#endif

    string fullPath = folderName + "/" + filename; // Combine folder and filename

    // Create and open the CSV file
    ofstream outputFile(fullPath);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open '" << fullPath << "' for writing." << endl;
        return;
    }

    // Write the CSV header
    outputFile << "Year,Month,Hottest Temperature,Coldest Temperature,Average Temperature,Median Temperature" << endl;

    // Write the results
    for (const TemperatureData& entry : data) {
        outputFile << entry.year << "," << entry.month << "," << entry.maxTemperature << "," << entry.maxTemperature << ","
            << entry.maxTemperature << "," << entry.maxTemperature << endl;
    }

    outputFile.close();
}

int main() {
    ofstream logFile("log.txt");
    if (!logFile.is_open()) {
        cout << "Error: Unable to open log file for writing." << endl;
        return 1;
    }

    cout << "Turing Moore Engineering Temperature Data Analyzer 1.0" << endl;
    logFile << "Turing Moore Engineering Temperature Data Analyzer 1.0" << endl;

    cout << "Instructions: " << endl;
    cout << "1. Enter 'b' for Bendigo data, 'a' for Avalon data, or 'c' for both." << endl;
    cout << "2. Choose to analyze data for a single year or all recorded years." << endl;
    cout << "3. Select from various analysis options." << endl;
    cout << "4. Save results to a CSV file." << endl;
    cout << "5. Enter 'e' to exit the program." << endl;

    cout << "Enter the option ('b' for Bendigo, 'a' for Avalon, 'c' for both): ";
    char option;
    cin >> option;

    vector<TemperatureData> temperatureData;
    if (option == 'b' || option == 'c') {
        vector<TemperatureData> bendigoData = ReadTemperatureData("Bendigo Airport Temperature Data-all.csv");
        temperatureData.insert(temperatureData.end(), bendigoData.begin(), bendigoData.end());
    }
    if (option == 'a' || option == 'c') {
        vector<TemperatureData> avalonData = ReadTemperatureData("Avalon Airport Temperature Data-all.csv");
        temperatureData.insert(temperatureData.end(), avalonData.begin(), avalonData.end());
    }

    cout << "Do you want all recorded years?" << endl;
    cout << "Enter the option ('y' for All recorded years, 'n' for single year): ";
    char record;
    cin >> record;

    if (record == 'y') {
        // Display start and end dates of the loaded data
        DisplayDataStartEndDate(temperatureData);

        cout << "Total rows: " << temperatureData.size() << " Total columns: 8" << endl;
        logFile << "Total rows: " << temperatureData.size() << " Total columns: 8" << endl;

        if (!temperatureData.empty()) {
            // Find the hottest day
            TemperatureData hottest = *max_element(temperatureData.begin(), temperatureData.end(), [](const TemperatureData& a, const TemperatureData& b) {
                return a.maxTemperature < b.maxTemperature;
                });
            cout << "Hottest day: " << hottest.year << "-" << hottest.month << "-" << hottest.day << " Temperature: " << hottest.maxTemperature << endl;
            logFile << "Hottest day: " << hottest.year << "-" << hottest.month << "-" << hottest.day << " Temperature: " << hottest.maxTemperature << endl;

            // Find the coldest day
            TemperatureData coldest = *min_element(temperatureData.begin(), temperatureData.end(), [](const TemperatureData& a, const TemperatureData& b) {
                return a.maxTemperature < b.maxTemperature;
                });
            cout << "Coldest day: " << coldest.year << "-" << coldest.month << "-" << coldest.day << " Temperature: " << coldest.maxTemperature << endl;
            logFile << "Coldest day: " << coldest.year << "-" << coldest.month << "-" << coldest.day << " Temperature: " << coldest.maxTemperature << endl;

            // Calculate mean temperature
            double sum = 0.0;
            for (const TemperatureData& entry : temperatureData) {
                sum += entry.maxTemperature;
            }
            double mean = sum / temperatureData.size();
            cout << "Mean temperature for the year: " << mean << endl;
            logFile << "Mean temperature for the year: " << mean << endl;

            // Calculate median temperature
            vector<double> temperatures;
            for (const TemperatureData& entry : temperatureData) {
                temperatures.push_back(entry.maxTemperature);
            }
            sort(temperatures.begin(), temperatures.end());
            double median;
            if (temperatures.size() % 2 == 0) {
                median = (temperatures[temperatures.size() / 2 - 1] + temperatures[temperatures.size() / 2]) / 2;
            }
            else {
                median = temperatures[temperatures.size() / 2];
            }
            cout << "Median temperature for the year: " << median << endl;
            logFile << "Median temperature for the year: " << median << endl;


            // Menu for selecting options
            cout << "Select an option:" << endl;
            cout << "1. Find Mode" << endl;
            cout << "2. Find Coldest Month" << endl;
            cout << "3. Find Hottest Month" << endl;
            cout << "4. Find Most Consistent Month" << endl;
            cout << "5. Calculate Percentage Within One Standard Deviation" << endl;
            cout << "6. Choose the files with data" << endl;
            cout << "7. Save Results to CSV" << endl;
            cout << "8. Exit" << endl;
            int selectedOption;
            cin >> selectedOption;

            double mode; // Declare mode outside of the switch statement

            switch (selectedOption) {
            case 1:
                // Find Mode
                mode = FindMode(temperatureData);
                cout << "Mode temperature: " << mode << endl;
                break;
            case 2:
                // Find Coldest Month
                FindColdestMonth(temperatureData);
                break;
            case 3:
                // Find Hottest Month
                FindHottestMonth(temperatureData);
                break;
            case 4:
                // Find the most consistent month
                FindMostConsistentMonth(temperatureData);
                break;
            case 5:
                // Calculate Percentage Within One Standard Deviation
                CalculatePercentageWithinOneStdDev(temperatureData);
                break;
            case 6:
                // Calculate Percentage Within One Standard Deviation
                CalculatePercentageWithinOneStdDev(temperatureData);
                break;
            case 7: {
                // Save Results to CSV
                cout << "Enter the name of the CSV file to save results: ";
                string filename;
                cin >> filename;
                WriteResultsToCSV(filename, temperatureData);
                cout << "Results saved to '" << filename << "'." << endl;
                break;
            }
            case 8:
                cout << "Exiting the program." << endl;
                return 0;
            default:
                cout << "Invalid option." << endl;
                break;
            }
        }
    }
    else if (record == 'n') {
        cout << "Enter the year want: ";
        int rYear;
        cin >> rYear;

        vector<TemperatureData> rYearData = FilterDataByYear(temperatureData, rYear);

        if (!rYearData.empty()) {
            // Display details for the selected year
            cout << "Details for the year " << rYear << endl;

            // You can add code here to display details for the selected year
            cout << "Total rows: " << rYearData.size() << " Total columns: 8" << endl;
            logFile << "Total rows: " << rYearData.size() << " Total columns: 8" << endl;

            if (!rYearData.empty()) {
                // Find the hottest day
                TemperatureData hottest = *max_element(rYearData.begin(), rYearData.end(), [](const TemperatureData& a, const TemperatureData& b) {
                    return a.maxTemperature < b.maxTemperature;
                    });
                cout << "Hottest day: " << hottest.year << "-" << hottest.month << "-" << hottest.day << " Temperature: " << hottest.maxTemperature << endl;
                logFile << "Hottest day: " << hottest.year << "-" << hottest.month << "-" << hottest.day << " Temperature: " << hottest.maxTemperature << endl;

                // Find the coldest day
                TemperatureData coldest = *min_element(rYearData.begin(), rYearData.end(), [](const TemperatureData& a, const TemperatureData& b) {
                    return a.maxTemperature < b.maxTemperature;
                    });
                cout << "Coldest day: " << coldest.year << "-" << coldest.month << "-" << coldest.day << " Temperature: " << coldest.maxTemperature << endl;
                logFile << "Coldest day: " << coldest.year << "-" << coldest.month << "-" << coldest.day << " Temperature: " << coldest.maxTemperature << endl;

                // Calculate mean temperature
                double sum = 0.0;
                for (const TemperatureData& entry : rYearData) {
                    sum += entry.maxTemperature;
                }
                double mean = sum / rYearData.size();
                cout << "Mean temperature for the year: " << mean << endl;
                logFile << "Mean temperature for the year: " << mean << endl;

                // Calculate median temperature
                vector<double> temperatures;
                for (const TemperatureData& entry : rYearData) {
                    temperatures.push_back(entry.maxTemperature);
                }
                sort(temperatures.begin(), temperatures.end());
                double median;
                if (temperatures.size() % 2 == 0) {
                    median = (temperatures[temperatures.size() / 2 - 1] + temperatures[temperatures.size() / 2]) / 2;
                }
                else {
                    median = temperatures[temperatures.size() / 2];
                }
                cout << "Median temperature for the year: " << median << endl;
                logFile << "Median temperature for the year: " << median << endl;

                // Menu for selecting options
                cout << "Select an option:" << endl;
                cout << "1. Find Mode" << endl;
                cout << "2. Find Coldest Month" << endl;
                cout << "3. Find Hottest Month" << endl;
                cout << "4. Find Most Consistent Month" << endl;
                cout << "5. Calculate Percentage Within One Standard Deviation" << endl;
                cout << "6. Choose the files with data" << endl;
                cout << "7. Save Results to CSV" << endl;
                cout << "8. Exit" << endl;
                int selectedOption;
                cin >> selectedOption;

                double mode; // Declare mode outside of the switch statement

                switch (selectedOption) {
                case 1:
                    // Find Mode
                    mode = FindMode(rYearData);
                    cout << "Mode temperature: " << mode << endl;
                    break;
                case 2:
                    // Find Coldest Month
                    FindColdestMonth(rYearData);
                    break;
                case 3:
                    // Find Hottest Month
                    FindHottestMonth(rYearData);
                    break;
                case 4:
                    // Find the most consistent month
                    FindMostConsistentMonth(rYearData);
                    break;
                case 5:
                    // Calculate Percentage Within One Standard Deviation
                    CalculatePercentageWithinOneStdDev(rYearData);
                    break;
                case 6:
                    // Calculate Percentage Within One Standard Deviation
                    CalculatePercentageWithinOneStdDev(rYearData);
                    break;
                case 7: {
                    // Save Results to CSV
                    cout << "Enter the name of the CSV file to save results: ";
                    string filename;
                    cin >> filename;
                    WriteResultsToCSV(filename, rYearData);
                    cout << "Results saved to '" << filename << "'." << endl;
                    break;
                }
                case 8:
                    cout << "Exiting the program." << endl;
                    return 0;
                default:
                    cout << "Invalid option." << endl;
                    break;
                }
            }
        }
        else {
            cout << "No data found for the selected year." << endl;
        }
    }
    else {
        cout << "Enter a valid character!" << endl;
    }

    cout << "Enter 'c' to continue or 'e' to exit: ";
    cin >> option;
    if (option == 'e') {
        cout << "Exiting the program." << endl;
        return 0;
    }

    return main();
}
