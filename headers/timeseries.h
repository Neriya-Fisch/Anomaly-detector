/*
 * timeseries.h
 *
 * Author: 315558692 Neriya Fisch
 */
#include <map>
#include <vector>
#include "anomaly_detection_util.h"
#ifndef TIMESERIES_H_
#define TIMESERIES_H_

using namespace std;

class TimeSeries
{
	const char *fileName;

protected:
	map<string, vector<float>> table;

public:
	TimeSeries(const char *CSVfileName);
	TimeSeries(const TimeSeries &ts);
	~TimeSeries();
	map<string, vector<float>> getTable();
	void makeTable();
	Point pointOfCorrelatedFeatures(string a, string b, long rowNum);
	int numOfValues();
};
#endif /* TIMESERIES_H_ */
