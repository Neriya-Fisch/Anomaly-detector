/*
 * timeseries.cpp
 *
 * Author: 315558692 Neriya Fisch
 */
#include "timeseries.h"
#include "anomaly_detection_util.h"
#include <map>
#include <vector>
#include <fstream>
#include <stdexcept> // std::runtime_error
#include <sstream>   // std::stringstream

using namespace std;
const char *fileName;
map<string, vector<float>> table;

TimeSeries::TimeSeries(const char *CSVfileName)
{
    fileName = CSVfileName;
    makeTable();
}

TimeSeries::TimeSeries(const TimeSeries &ts)
{
    this->fileName = ts.fileName;
    makeTable();
}

void TimeSeries::makeTable()
{
    std::ifstream myFile(fileName);
    if (!myFile.is_open())
        throw std::runtime_error("Could not open file");
    std::string line, featureName;
    float val;
    // Read the features names
    if (myFile.good())
    {
        std::getline(myFile, line); // The line of the features
        std::stringstream ss(line);
        //Extract all the features one by one
        while (std::getline(ss, featureName, ','))
        {
            table.insert({featureName, std::vector<float>{}});
        }
    }
    while (std::getline(myFile, line))
    {
        std::stringstream ss(line);
        map<string, vector<float>>::iterator it = table.begin();
        // Extract each float
        while (ss >> val)
        {
            it->second.push_back(val);
            it++;
            if (ss.peek() == ',')
                ss.ignore();
        }
    }
    myFile.close();
}

TimeSeries::~TimeSeries()
{
}

map<string, vector<float>> TimeSeries::getTable()
{
    return table;
}

Point TimeSeries::pointOfCorrelatedFeatures(string a, string b, long rowNum)
{
    float x = table.at(a).at(rowNum - 1);
    float y = table.at(b).at(rowNum - 1);
    return Point(x, y);
}

int TimeSeries::numOfValues()
{
    map<string, vector<float>> table = getTable();
    map<string, vector<float>>::iterator it = table.begin();
    return it->second.size();
}