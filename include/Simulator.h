#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Element.h"

using namespace std;

class Simulator
{
public:
	Simulator() : GenForReference_(nullptr){};
	~Simulator();

	void loadCircuit(const string &filepath);
	void simulate(const string &filepath);

private:
	void connectElements(ifstream &inputFile);
	void writeToProbeString();
	void writeToTxtFile(const string &filepath);
	void emptySimulator();
	void createElements(ifstream &inputFile);
	void getAndCheck(ifstream &inputFile, double &num);
	void createOneElement(ifstream &inputFile);
	void errorCheckingAfterConnecting(ifstream &inputFile);
	void errorCheckingWhileConnecting(ifstream &inputFile, const int &upId, const int &downId, const int &pin);
	void errorCheckingBeforConnecting(ifstream &inputFile);

	// all elements including probes
	vector<Element *> allElements_;
	// all probes
	vector<Probe *> probes_;
	Generator *GenForReference_;

	static double time_;
	static int numberOfElemets_;
};

#endif