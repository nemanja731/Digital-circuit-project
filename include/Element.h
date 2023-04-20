#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include <vector>
#include <sstream>

using namespace std;

enum Type
{
	PROBE,
	SIMPLEGENERATOR,
	USERGENERATOR,
	NOTGATE,
	ANDGATE,
	ORGATE
};

// abstract class
class Element
{
public:
	Element(const Type elType, const int inputs, const int id) : id_(id), inputs_(inputs), outputs_(0), checkedInputs_(0), checkedOutputs_(0), state_(0), type_(elType), input_(inputs, nullptr){};
	Element(const Element &) = delete;
	virtual ~Element() { input_.clear(); };

	// The function for the generators is to move the remainingTime_ to the currentInterval
	// and return the new state, for the gates, take the current state, and for the probes,
	// start from them passing through the element tree and return the state
	virtual bool getState() = 0;

	// The function serves to call getState of all inputs and change the current state
	// of that element depending on the function of the element and its input
	virtual void logic();

	// Checking if the element at input_[checkedInputs_] is some generator
	bool isInputGenerator() const;

	// Used in getState() probes. The element at inputs_[checkedInputs_] increments
	// checkedOutput_ and if it is equal to outputs_(number of outputs) it resets it
	// to 0 for the next simulation cycle, if the generator returns true if it is not
	// a generator then it is gate and returns true if it has already run for that
	// cycle because then can take his condition and should not go further
	bool checkForGenOrAlreadyEvalutedGate();

	bool state_;
	const int id_, inputs_;
	const Type type_;
	int outputs_, checkedInputs_, checkedOutputs_;
	static double currentInterval_, nextInterval_, currentTime_;
	vector<Element *> input_;
};

class Probe : public Element
{
public:
	Probe(const int id) : Element(PROBE, 1, id){};
	Probe(const Probe &) = delete;

	// writes changes to ouputString_
	void writeState(bool newState);
	virtual bool getState() override;

	string outputString_;
};
class Generator : public Element
{
public:
	Generator(const Type elType, const double remainigTime, const int id) : Element(elType, 0, id), remainingTime_(remainigTime){};
	Generator(const Generator &) = delete;

	// time remaining until the next change
	double remainingTime_;
};
class Gate : public Element
{
public:
	Gate(const Type elType, const int pin, const int id) : Element(elType, pin, id){};
	Gate(const Gate &) = delete;
};
class SimpleGenerator : public Generator
{
public:
	SimpleGenerator(const double freq, const int id) : Generator(SIMPLEGENERATOR, 1 / (2 * freq), id), halfPeriod_(1 / (2 * freq)){};
	SimpleGenerator(const SimpleGenerator &) = delete;

	virtual bool getState() override;

	// half-period time, ie the time required for a signal change
	const double halfPeriod_;
};
class UserGenerator : public Generator
{
public:
	UserGenerator(const vector<double> &changes, const int id) : Generator(USERGENERATOR, changes[0], id), changes_(changes), changeIndex_(1){};
	UserGenerator(const UserGenerator &) = delete;
	~UserGenerator();

	virtual bool getState() override;

	int changeIndex_;

	// vector of moments when the signal changes
	vector<double> changes_;
};
class And : public Gate
{
public:
	And(const int inputs, const int id) : Gate(ANDGATE, inputs, id){};
	And(const And &) = delete;

	virtual bool getState() override;
	virtual void logic() override;
};
class Or : public Gate
{
public:
	Or(const int inputs, const int id) : Gate(ORGATE, inputs, id){};
	Or(const Or &) = delete;

	virtual bool getState() override;
	virtual void logic() override;
};
class Not : public Gate
{
public:
	Not(const int id) : Gate(NOTGATE, 1, id){};
	Not(const Not &) = delete;

	virtual bool getState() override;
	virtual void logic() override;
};

#endif