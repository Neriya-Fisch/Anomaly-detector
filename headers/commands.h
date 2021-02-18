
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <string.h>
#include <fstream>
#include <sstream>
#include "HybridAnomalyDetector.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

class DefaultIO
{
public:
	virtual string read() = 0;
	virtual void write(string text) = 0;
	virtual void write(float f) = 0;
	virtual void read(float *f) = 0;

	virtual ~DefaultIO() {}
};

class SocketIO : public DefaultIO
{
	int serverFD;

public:
	SocketIO(int ServerFD)
	{
		this->serverFD = ServerFD;
	}
	virtual void write(string input)
	{
		::write(serverFD, input.c_str(), input.length());
	}

	virtual void write(float f)
	{
		::write(serverFD, &f, sizeof(f));
	}
	virtual string read()
	{
		string serverInput = "";
		char c = 0;
		::read(serverFD, &c, sizeof(char));
		while (c != '\n')
		{
			serverInput += c;
			::read(serverFD, &c, sizeof(char));
		}
		return serverInput;
	}
	virtual void read(float *f)
	{
		string serverInput = "";
		char c = 0;
		::read(serverFD, &c, sizeof(char));
		while (c != '\n')
		{
			serverInput += c;
			::read(serverFD, &c, sizeof(char));
		}
		*f = stof(serverInput);
	}
};
class StandartIO : public DefaultIO
{
	ifstream in;
	ofstream out;

public:
	StandartIO(string inputFile, string outputFile)
	{
		in.open(inputFile);
		out.open(outputFile);
	}

	virtual void write(string text)
	{
		cout << text << endl;
		out << text;
	}

	virtual void write(float f)
	{
		cout << f << endl;
		out << f;
	}
	virtual string read()
	{
		string s;
		in >> s;
		return s;
	}
	virtual void read(float *f)
	{
		in >> *f;
	}
	void close()
	{
		if (in.is_open())
			in.close();
		if (out.is_open())
			out.close();
	}

	~StandartIO()
	{
		close();
	}
};

class CliData
{
public:
	CliData()
	{
		this->ptrTrain = nullptr;
		this->ptrTest = nullptr;
		HybridAnomalyDetector *had1 = new HybridAnomalyDetector();
		had = had1;
	}
	TimeSeries *ptrTrain;
	TimeSeries *ptrTest;
	HybridAnomalyDetector *had;
	void setTrain(const char *c)
	{
		TimeSeries *train = new TimeSeries(c);
		ptrTrain = train;
	}
	void setTest(const char *c)
	{
		TimeSeries *test = new TimeSeries(c);
		ptrTest = test;
	}
	void setHad(float threashold)
	{
		had->setThreash(threashold);
	}

	~CliData()
	{
		delete ptrTrain;
		delete ptrTest;
		delete had;
	}
};

class Command
{
protected:
	string description;
	DefaultIO *dio;
	CliData *cliData;

public:
	Command(DefaultIO *dio, CliData *cliData) : dio(dio), cliData(cliData) {}
	Command() {}
	virtual string getDescription()
	{
		return description;
	}
	virtual void execute() = 0;
	virtual ~Command() {}
};

class UploadCommand : public Command
{
public:
	UploadCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "1.upload a time series csv file\n";
	}
	string readfile()
	{
		string s1, s2;
		while (s1 != "done")
		{
			s1 = dio->read();
			s2 += s1 + '\n';
		}
		return s2;
	}
	//gets from the client 2 csv files and make from them 2 timeseriesess.
	void execute()
	{
		ofstream f1;
		ofstream f2;
		f1.open("anomalyTrain.csv");
		f2.open("anomalyTest.csv");
		string toTrainFile;
		string toTestFile;
		dio->write("Please upload your local train CSV file.\n");
		toTrainFile = readfile();
		f1 << toTrainFile;
		dio->write("Upload complete.\n");
		dio->write("Please upload your local test CSV file.\n");
		toTestFile = readfile();
		f2 << toTestFile;
		dio->write("Upload complete.\n");
		f1.close();
		f2.close();
		cliData->setTrain("anomalyTrain.csv");
		cliData->setTest("anomalyTest.csv");
	}
};

//gets input's nunber and set the threshold of the HybridAnomalyDetector to it.
class ThresholdCommand : public Command
{

public:
	ThresholdCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "2.algorithm settings\n";
	}
	void execute()
	{
		float *inputThreash;
		dio->write("The current correlation threshold is 0.9\nType a new threshold\n");
		dio->read(inputThreash);
		while (*inputThreash < 0 && *inputThreash > 1)
		{
			dio->write("please choose a value between 0 and 1.\n");
			dio->write("The current correlation threshold is 0.9\n");
			dio->read(inputThreash);
		}
		cliData->setHad(*inputThreash);
	}
};

class DetectCommand : public Command
{

public:
	DetectCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "3.detect anomalies\n";
	}
	void execute()
	{
		cliData->had->learnNormal(*cliData->ptrTrain);
		dio->write("anomaly detection complete.\n");
	}
};

class DisplayCommand : public Command
{
public:
	DisplayCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "4.display results\n";
	}
	void execute()
	{
		vector<AnomalyReport> ar = cliData->had->detect(*cliData->ptrTest);
		for (AnomalyReport a : ar)
		{
			string s = to_string(a.timeStep) + '\t' + a.description;
			dio->write(s + '\n');
		}
		dio->write("Done.\n");
	}
};

//gets input file of detection and compare it to ours
class AnalyzeCommand : public Command
{
public:
	AnalyzeCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "5.upload anomalies and analyze results\n";
	}
	void execute()
	{
		dio->write("Please upload your local anomalies file.\n");
		map<string, vector<pair<long, long>>> unionReports = unionAnomalies();
		dio->write("Upload complete.\n");
		long P = 0;
		long num1, num2;
		vector<pair<long, long>> input;
		string s = dio->read();
		while (s != "done")
		{
			P++;
			stringstream ss(s);
			string num;
			getline(ss, num, ',');
			num1 = stol(num);
			getline(ss, num, ',');
			num2 = stol(num);
			pair<long, long> pair;
			pair.first = num1;
			pair.second = num2;
			input.push_back(pair);
			s = dio->read();
		}
		long N = cliData->ptrTest->numOfValues();
		double TP = 0, FP = 0;
		long sum = 0, FPcounter = 0;
		for (int i = 0; i < input.size(); i++)
		{
			sum += input.at(i).second - input.at(i).first + 1;
		}
		N -= sum;
		for (map<string, vector<pair<long, long>>>::iterator it = unionReports.begin(); it != unionReports.end(); ++it)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				//to compute the numbers of ranges in unionReports
				FP++;
				for (int j = 0; j < input.size(); j++)
				{
					if (!(it->second.at(i).first > input.at(j).second || input.at(j).first > it->second.at(i).second))
					{
						TP++;
						FPcounter++;
					}
				}
			}
		}
		double TPR = TP / P;
		TPR = TPR * 1000;
		TPR = floor(TPR);
		TPR = TPR / 1000;
		FP -= FPcounter;
		double AFR = FP / N;
		AFR = AFR * 1000;
		AFR = floor(AFR);
		AFR = AFR / 1000;
		stringstream ss1;
		ss1 << TPR;
		string str1 = ss1.str();
		dio->write("True Positive Rate: " + str1 + '\n');
		stringstream ss2;
		ss2 << AFR;
		string str2 = ss2.str();
		dio->write("False Positive Rate: " + str2 + '\n');
	}

	//united all same features' sequencess, and make a table with features' keys and vetor of pairs
	// of beginnig and ending of the detections
	map<string, vector<pair<long, long>>> unionAnomalies()
	{
		map<string, vector<pair<long, long>>> unionReports;
		vector<pair<long, long>> pairs;
		long index1, index2, endStrike;
		string arName1, arName2;
		vector<AnomalyReport> ar = cliData->had->detect(*cliData->ptrTest);
		index1 = ar.at(0).timeStep;
		endStrike = index1;
		arName1 = ar.at(0).description;
		for (int i = 0; i < ar.size(); i++)
		{
			if (i != ar.size() - 1)
			{
				index2 = ar.at(i + 1).timeStep;
				arName2 = ar.at(i + 1).description;
			}
			if (arName1 == arName2 && (endStrike + 1 == index2))
			{
				endStrike++;
				continue;
			}
			else
			{
				pair<long, long> p(index1, endStrike);
				pairs.push_back(p);
				unionReports.insert({arName1, pairs});
				arName1 = arName2;
				index1 = index2;
				endStrike = index1;
				pairs.clear();
			}
		}
		return unionReports;
	}
};

class ExitCommand : public Command
{

public:
	ExitCommand(DefaultIO *dio, CliData *cliData) : Command(dio, cliData)
	{
		Command::description = "6.exit\n";
	}
	void execute()
	{
		return;
	}
};

#endif /* COMMANDS_H_ */
