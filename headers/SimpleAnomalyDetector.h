/*
 * SimpleAnomalyDetector.h
 *
 * Author: 315558692 Neriya Fisch
 */
#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_
#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include "minCircle.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures
{
	string feature1, feature2; // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	vector<Point> featuresPoints;
};

class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector
{
protected:
	vector<correlatedFeatures> cf;
	vector<Circle> minCircle;
	vector<float> maxDev;

public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();
	virtual void learnNormal(const TimeSeries &ts);
	virtual vector<AnomalyReport> detect(const TimeSeries &ts);
	virtual void createReport(vector<correlatedFeatures> cf, int index, Point p, vector<AnomalyReport> &v, long timeStep);

	vector<correlatedFeatures> getNormalModel()
	{
		return cf;
	}
};

#endif /* SIMPLEANOMALYDETECTOR_H_ */
