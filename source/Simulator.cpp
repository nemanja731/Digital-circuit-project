#include "Simulator.h"
#include "Exceptions.h"

double Simulator::time_ = 0;
int Simulator::numberOfElemets_ = 0;

void Simulator::loadCircuit(const string &filepath)
{
	try
	{
		double newTime, nomOfEl;
		// if something has not been deleted to clean the simulator
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		ifstream inputFile(filepath, ios::in);
		// read the first two lines and check
		getAndCheck(inputFile, newTime);
		getAndCheck(inputFile, nomOfEl);
		Simulator::time_ = newTime;
		Simulator::numberOfElemets_ = nomOfEl;
		// make space in memory for each element and add all elements to vector allElements_
		createElements(inputFile);
		// connects all elements to each other and remembers how many outputs each element has
		connectElements(inputFile);
	}
	catch (InvalidValue *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(2);
	}
}

void Simulator::simulate(const string &filepath)
{
	// writes the change to each probe for the entire simulation
	writeToProbeString();
	// print the string from each probe to the output txt file
	writeToTxtFile(filepath);
	emptySimulator();
}

Simulator::~Simulator()
{
	emptySimulator();
}

void Simulator::connectElements(ifstream &inputFile)
{
	string line;
	stringstream lineStream(line);
	int upId, downId, pin;
	try
	{
		while (inputFile.peek() != EOF)
		{
			// reads line by line
			getline(inputFile, line);
			lineStream.str(line);
			lineStream >> downId >> upId >> pin;

			errorCheckingWhileConnecting(inputFile, upId, downId, pin);
			// the number of outputs is increased
			allElements_[downId - 1]->outputs_++;
			// interconnection of inputs
			allElements_[upId - 1]->input_[pin] = allElements_[downId - 1];

			lineStream.clear();
		}
		errorCheckingAfterConnecting(inputFile);
		inputFile.close();
	}
	catch (AlreadyConnected *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(3);
	}
	catch (InvalidInput *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(5);
	}
	catch (InputMissing *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(6);
	}
}

void Simulator::writeToProbeString()
{
	while (Element::currentTime_ < time_)
	{
		{ // get the remainingTime_ value from the first comparison generator
			// created which is the smallest interval for the next simulation cycle
			Element::nextInterval_ = GenForReference_->remainingTime_;
		}
		for (Probe *t : this->probes_)
		{
			// goes through each probe from the probes vector as the beginning
			// of the tree, returns the value of the state of the probe and writes
			// the change to a string if the new state is different from the previous one
			t->writeState(t->getState());
		}
		Element::currentInterval_ = Element::nextInterval_;
		Element::currentTime_ += Element::currentInterval_; // zamena intervala i resetovanje flaga da se ponovo uzme referentan vrednost preostalog vremna za narednu promenu
	}
}

void Simulator::writeToTxtFile(const string &filepath)
{
	string newName;
	for (Probe *t : this->probes_)
	{
		newName = filepath.substr(0, filepath.length() - 4);
		newName = newName + "_" + to_string(t->id_) + ".txt";
		ofstream outputFile(newName, ios::out);
		outputFile << t->outputString_;
		outputFile.close();
	}
	Element::currentInterval_ = Element::nextInterval_ = Element::currentTime_ = 0;
}

void Simulator::emptySimulator()
{
	for (Element *t : allElements_)
	{
		delete t;
	}
	probes_.clear();
	allElements_.clear();
	GenForReference_ = nullptr;
	Element::currentInterval_ = Element::nextInterval_ = Element::currentTime_ = 0;
}
void Simulator::createElements(ifstream &inputFile)
{
	allElements_.resize(numberOfElemets_, nullptr);
	try
	{
		for (int i = 0; i < numberOfElemets_; i++)
		{
			createOneElement(inputFile);
		}
		errorCheckingBeforConnecting(inputFile);
	}
	catch (SameDefinedElements *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(4);
	}
	catch (ElementMissing *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(4);
	}
	catch (GeneratorMissing *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(7);
	}
	catch (ProbeMissing *e)
	{
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())
			emptySimulator();
		exit(8);
	}
}

void Simulator::getAndCheck(ifstream &inputFile, double &num)
{
	string line;
	stringstream lineStream;
	getline(inputFile, line);
	lineStream.str(line);
	lineStream >> num;
	if (num < 0)
	{
		inputFile.close();
		throw new InvalidValue("An invalid value was entered");
	}
}

void Simulator::createOneElement(ifstream &inputFile)
{
	string line;
	stringstream lineStream(line);
	double sum = 0, freq, num;
	int inputs = 2, id, type;
	vector<double> changes;
	Element *newE = nullptr;
	Probe *newP;
	UserGenerator *newGen;
	Generator *newG = nullptr;
	// reads the line, inserts it into the stream and takes the first 2 values
	getline(inputFile, line);
	lineStream.str(line);
	lineStream >> id >> type;
	if (allElements_[id - 1] != nullptr)
	{
		inputFile.close();
		lineStream.clear();
		throw new SameDefinedElements("The same element is defined more than once");
	}
	switch (type)
	{
	case 0:
		newP = new Probe(id);
		newE = newP;
		probes_.push_back(newP);
		break;
	case 1:
		lineStream >> freq;
		newG = new SimpleGenerator(freq, id);
		if (GenForReference_ == nullptr)
			GenForReference_ = newG;
		newE = newG;
		break;
	case 2:
		while (lineStream.peek() != EOF)
		{
			lineStream >> num;
			sum = sum + num;
			changes.push_back(num);
		}
		newGen = new UserGenerator(changes, id);
		newGen->changes_.push_back(time_ - sum);
		if (GenForReference_ == nullptr)
			GenForReference_ = newGen;
		newE = newGen;
		changes.clear();
		break;
	case 3:
		lineStream >> inputs;
		newE = new Not(id);
		break;
	case 4:
		lineStream >> inputs;
		newE = new And(inputs, id);
		break;
	case 5:
		lineStream >> inputs;
		newE = new Or(inputs, id);
		break;
	}
	lineStream.clear();
	allElements_[id - 1] = newE;
}

void Simulator::errorCheckingAfterConnecting(ifstream &inputFile)
{
	for (Element *t : allElements_)
	{
		if (!t->input_.empty())
		{
			for (Element *b : t->input_)
			{
				if (b == nullptr)
				{
					inputFile.close();
					throw InputMissing("The element is missing an input");
				}
			}
		}
	}
}

void Simulator::errorCheckingWhileConnecting(ifstream &inputFile, const int &upId, const int &downId, const int &pin)
{
	if (allElements_[upId - 1]->input_[pin] != nullptr)
	{
		inputFile.close();
		throw new AlreadyConnected("The input of the element is reconnected");
	}
	if (allElements_[upId - 1]->inputs_ < pin)
	{
		inputFile.close();
		throw new InvalidInput("Bad access to element input");
	}
}

void Simulator::errorCheckingBeforConnecting(ifstream &inputFile)
{
	for (int k = 0; k < numberOfElemets_; k++)
		if (allElements_[k] == nullptr)
		{
			inputFile.close();
			throw new ElementMissing("One or more elements are missing");
		}
	if (GenForReference_ == nullptr)
	{
		inputFile.close();
		throw GeneratorMissing("Insert at least one generator");
	}
	if (probes_.empty())
	{
		inputFile.close();
		throw ProbeMissing("Insert at least one probe");
	}
}
