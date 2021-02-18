

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include <vector>
// if the threashold of the features is greater than the current threshold of the HybridAnomalyDetector
// then the normal (Line regresion) is activated. if the threshold is greater than 0.5 but smaller
// than the current one, the radius of the min circle of the features is the threshold.
class HybridAnomalyDetector : public SimpleAnomalyDetector
{
protected:
	float minThreash = 0.9;

public:
	void setThreash(float x);
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();
	virtual void createReport(vector<correlatedFeatures> cf, int index, Point p, vector<AnomalyReport> &v, long timeStep);
};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
