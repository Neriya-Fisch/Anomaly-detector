/*
 * SimpleAnomalyDetector.cpp
 *
 * Author: 315558692 Neriya Fisch
 */
#include "SimpleAnomalyDetector.h"
#include <map>
#include <vector>
#include <cmath>

SimpleAnomalyDetector::SimpleAnomalyDetector()
{
}

SimpleAnomalyDetector::~SimpleAnomalyDetector()
{
}
//makes vector of correlated features by a determinated threshold
void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts)
{
	TimeSeries t(ts);
	map<string, vector<float>> tTable = t.getTable();
	float correlated, mostCorrelated;
	string correlatedTo;
	correlatedFeatures correlatedFeat;
	Circle circle = {{0, 0}, 0};
	vector<Point> points;
	for (map<string, vector<float>>::iterator it1 = tTable.begin(); it1 != tTable.end(); ++it1)
	{
		float tempMaxDev = 0;
		mostCorrelated = 0;
		for (map<string, vector<float>>::iterator it2 = std::next(it1); it2 != tTable.end(); ++it2)
		{
			float *features1 = it1->second.data();
			float *features2 = it2->second.data();
			correlated = pearson(features1, features2, it1->second.size());
			if (!(fabs(correlated) > mostCorrelated && fabs(correlated) > 0.5))
				continue;
			points.clear();
			for (int i = 0; i < it1->second.size(); i++)
			{
				points.push_back(Point(features1[i], features2[i]));
			}
			Point **ptr = new Point *[points.size()];
			for (int i = 0; i < points.size(); i++)
			{
				ptr[i] = new Point(points.at(i));
			}
			circle = findMinCircle(ptr, points.size());
			mostCorrelated = fabs(correlated);
			correlatedTo = it2->first;
			float tempThreshold = 0;
			correlatedFeat = {it1->first,
							  it2->first,
							  mostCorrelated,
							  linear_reg(ptr, it1->second.size()),
							  tempThreshold};

			for (int i = 0; i < points.size(); i++)
			{
				delete ptr[i];
			}
			delete[] ptr;
		}
		if (mostCorrelated != 0)
		{
			for (vector<Point>::iterator it3 = points.begin(); it3 != points.end(); ++it3)
			{
				float temp = dev(*it3, correlatedFeat.lin_reg);
				if (temp > tempMaxDev)
				{
					tempMaxDev = temp;
				}
			}
			maxDev.push_back(tempMaxDev * 1.1);
			correlatedFeat.threshold = tempMaxDev * 1.1;
			correlatedFeat.featuresPoints = points;
			cf.push_back(correlatedFeat);
			minCircle.push_back(circle);
		}
	}
}

// gets new timeseries and check for every correlated feature if there is detection
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts)
{
	TimeSeries t(ts);
	map<string, vector<float>> tTable = t.getTable();
	vector<AnomalyReport> vec;
	int vecCounter;
	int vecIndex = 0;
	map<string, vector<float>>::iterator it1 = tTable.begin();
	for (vector<correlatedFeatures>::iterator it2 = cf.begin(); it2 != cf.end(); ++it2)
	{
		long timeStep = 1;
		for (int i = 0; i < it1->second.size(); i++)
		{
			Point point = t.pointOfCorrelatedFeatures(it2->feature1, it2->feature2, timeStep);
			createReport(cf, vecIndex, point, vec, timeStep);
			timeStep++;
		}
		vecIndex++;
	}
	return vec;
}

// create detect's report by line regresion algorithm
void SimpleAnomalyDetector::createReport(vector<correlatedFeatures> cf, int index, Point p, vector<AnomalyReport> &vec, long timeStep)
{
	float dev1 = dev(p, cf.at(index).lin_reg);
	if (dev1 > maxDev.at(index))
	{
		const string s = cf.at(index).feature1 + "-" + cf.at(index).feature2;
		AnomalyReport ar(s, timeStep);
		vec.push_back(ar);
	}
}
