#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector()
{
}

HybridAnomalyDetector::~HybridAnomalyDetector()
{
}
// create detect's report by line regresion or min circle algorithm in accordiance to the threshold
void HybridAnomalyDetector::createReport(vector<correlatedFeatures> cf, int index, Point p, vector<AnomalyReport> &vec, long timeStep)
{
	if (cf.at(index).corrlation > minThreash)
	{
		SimpleAnomalyDetector::createReport(cf, index, p, vec, timeStep);
	}
	if (cf.at(index).corrlation > 0.5 && cf.at(index).corrlation <= minThreash)
	{
		cf.at(index).threshold = minCircle.at(index).radius * 1.1;
		int size = cf.at(index).featuresPoints.size();
		float dist1 = dist(p, minCircle.at(index).center);
		if (dist1 > cf.at(index).threshold)
		{
			const string s = cf.at(index).feature1 + "-" + cf.at(index).feature2;
			AnomalyReport ar(s, timeStep);
			vec.push_back(ar);
		}
	}
}

void HybridAnomalyDetector::setThreash(float x)
{
	minThreash = x;
}