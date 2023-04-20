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

	/* Funkcija za generatore sluzi da im pomeri remainingTime_ za currentInterval i vrati novo stanje,
	gate-ovima uzme trenutno stanje, a za sonde da pocne od njih prolazak kroz stablo elementa i vrati stanje*/
	virtual bool getState() = 0;

	// Funckija sluzi da se pozove getState svih inputa i promeni trenutno stanje tog elemnta u zavisnosti od funkcije elementa i njegovig inputa
	virtual void logic();

	// Provera da li element na input_[checkedInputs_] je neki generator
	bool isInputGenerator() const;

	/*Koristi se u getState() sonde. Element na inputs_[checkedInputs_] inkrementira checkedOutput_
	i ako je jednak outputs_(broj outputa) resetuje ga na 0 za naredni ciklus simulacije,ako je generator vraca
	true ako nije generator onda je gate i vraca true ako je vec obisao za taj ciklus jer onda
	moze da mu uzme stanje i ne treba da ide dalje*/
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

	virtual bool getState() override;
	void writeState(bool newState); // zapisuje promene u ouputString_

	string outputString_;
};
class Generator : public Element
{
public:
	Generator(const Type elType, const double remainigTime, const int id) : Element(elType, 0, id), remainingTime_(remainigTime){};
	Generator(const Generator &) = delete;

	double remainingTime_; // vreme preostalo do naredne promene
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

	// Vreme poluperiode odnosno vreme potrebno za promenu signala
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

	// vektor trenutaka kada dolazi do promene signala
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