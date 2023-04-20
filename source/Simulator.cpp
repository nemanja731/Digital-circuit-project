#include "Simulator.h"
#include "Exceptions.h"

double Simulator::time_ = 0;
int Simulator::numberOfElemets_ = 0;

void Simulator::loadCircuit(const string& filepath){
	try {
		double newTime,nomOfEl;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();//ako nesto nije izbrsano da se ocisti simulator
		ifstream inputFile(filepath, ios::in);

		getAndCheck(inputFile, newTime);//cita prvi red i proverava vrednost
		getAndCheck(inputFile, nomOfEl);//cita drugi red i proverava vrednost
		Simulator::time_ = newTime;
		Simulator::numberOfElemets_ = nomOfEl;//konverizja double u int (ne gubi se vrednost jer je upisan ceo broj)

		createElements(inputFile);// pravi u memoriji mesta za svaki element i dodaje sve elemente u vector allElements_

		connectElements(inputFile);//povezuje sve elementi medjusobno i pamnti koliko svaki element ima izlaza
	}
	catch (InvalidValue* e) {
		cout<<e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(2);
	}
}

void Simulator::simulate(const string& filepath){
	writeToProbeString();//za celu simulaciju zapisuje svakoj sondi promenu 

	writeToTxtFile(filepath);//ispisuje string iz svake sonde u izlazni txt file

	emptySimulator();
}

Simulator::~Simulator()
{
	emptySimulator();
}

void Simulator::connectElements(ifstream& inputFile){
	string line;
	stringstream lineStream(line);
	int upId, downId, pin;
	try {
		while (inputFile.peek() != EOF) {
			getline(inputFile, line);// cita liniju po liniju
			lineStream.str(line);
			lineStream >> downId >> upId >> pin;

			errorCheckingWhileConnecting(inputFile, upId, downId, pin);

			allElements_[downId - 1]->outputs_++;// povecava se broj izlaza
			allElements_[upId - 1]->input_[pin]= allElements_[downId - 1];// medjusobno povezivanje inputa

			lineStream.clear();
		}
		errorCheckingAfterConnecting(inputFile);
		inputFile.close();
	}
	catch (AlreadyConnected* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(3);
	}
	catch (InvalidInput* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(5);
	}
	catch (InputMissing* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(6);
	}
}

void Simulator::writeToProbeString(){
	while (Element::currentTime_ < time_) {
	 { //uzmi vrdenost remainingTime_ od prvog napravljenog generator za uporedjivanje koji je najmanji interval za naredni ciklus simulacije
			Element::nextInterval_ = GenForReference_->remainingTime_;
		}
		for (Probe* t : this->probes_) {
			t->writeState(t->getState());/* prolazi kroz svaku sondu iz vektora probes kao pocetak stabla, vraca
			vrednost stanja sonde i zapisuje promenu u stringa ako je novo stanje razlicito od prethodnog*/
		}
		Element::currentInterval_ = Element::nextInterval_;
		Element::currentTime_ += Element::currentInterval_;// zamena intervala i resetovanje flaga da se ponovo uzme referentan vrednost preostalog vremna za narednu promenu
	}
	
}

void Simulator::writeToTxtFile(const string& filepath){
	string newName;
	for (Probe* t : this->probes_) {
		newName = filepath.substr(0, filepath.length() - 4);
		newName = newName + "_" + to_string(t->id_) + ".txt";
		ofstream outputFile(newName, ios::out);
		outputFile <<t->outputString_;
		outputFile.close();
	}
	Element::currentInterval_ = Element::nextInterval_ = Element::currentTime_ = 0;

	
}

void Simulator::emptySimulator(){
	for (Element* t : allElements_) {
		delete t;
	}
	probes_.clear();
	allElements_.clear();
	GenForReference_ = nullptr;
	Element::currentInterval_ = Element::nextInterval_ = Element::currentTime_ = 0;
}
void Simulator::createElements(ifstream& inputFile){
	allElements_.resize(numberOfElemets_, nullptr);
	try {
		for (int i = 0; i < numberOfElemets_; i++) {
			createOneElement(inputFile);
		}
		errorCheckingBeforConnecting(inputFile);
	}
	catch (SameDefinedElements* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(4);
	}
	catch (ElementMissing* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(4);
	}
	catch (GeneratorMissing* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(7);
	}
	catch (ProbeMissing* e) {
		cout << e->what();
		delete e;
		if (!probes_.empty() || !allElements_.empty())emptySimulator();
		exit(8);
	}
}

void Simulator::getAndCheck(ifstream& inputFile, double& num){
	string line;
	stringstream lineStream;
	getline(inputFile, line);
	lineStream.str(line);
	lineStream >> num;
	if (num < 0){
		inputFile.close();
		throw new InvalidValue("Uneta je invalidan vrednost.");
	}
}

void Simulator::createOneElement(ifstream& inputFile)
{
	string line;
	stringstream lineStream(line);
	double sum=0, freq, num;
	int inputs=2, id, type;
	vector  <double> changes;
	Element* newE = nullptr;
	Probe* newP;
	UserGenerator* newGen;
	Generator* newG = nullptr;

	getline(inputFile, line);//cita liniju ubacuje u stream i uzima prve 2 vrednost 
	lineStream.str(line);
	lineStream >> id >> type;
	if (allElements_[id - 1] != nullptr) {
		inputFile.close();
		lineStream.clear();
		throw new SameDefinedElements("Definisan je isti elemenat vise puta");
	}
	switch (type) {
	case 0:newP = new Probe(id);
		newE = newP;
		probes_.push_back(newP);
		break;
	case 1:lineStream >> freq;
		newG = new SimpleGenerator(freq, id);
		if (GenForReference_ == nullptr)GenForReference_ = newG;
		newE = newG;
		break;
	case 2:
		while (lineStream.peek() != EOF) {
			lineStream >> num;
			sum = sum + num;
			changes.push_back(num);
		}
		newGen = new UserGenerator(changes, id);
		newGen->changes_.push_back(time_ - sum);
		if (GenForReference_ == nullptr)GenForReference_ = newGen;
		newE = newGen;
		changes.clear();
		break;
	case 3:lineStream >> inputs;
		newE = new Not(id);
		break;
	case 4:lineStream >> inputs;
		newE = new And(inputs, id);
		break;
	case 5:lineStream >> inputs;
		newE = new Or(inputs, id);
		break;
	}
	lineStream.clear();
	allElements_[id - 1] = newE;
}

void Simulator::errorCheckingAfterConnecting(ifstream& inputFile){
	for (Element* t : allElements_) {
		if (!t->input_.empty()) {
			for (Element* b : t->input_) {
				if (b == nullptr) {
					inputFile.close();
					throw InputMissing("Elementu nedostaje ulaz");
				}
			}
		}

	}
}

void Simulator::errorCheckingWhileConnecting(ifstream& inputFile, const int& upId, const int& downId, const int& pin){
	if (allElements_[upId - 1]->input_[pin] != nullptr) {
		inputFile.close();
		throw new AlreadyConnected("Ulaz elementa ponovo se povezuje");

	}
	if (allElements_[upId - 1]->inputs_ < pin) {
		inputFile.close();
		throw new InvalidInput("Los pristup ulazu elementa");
	}
}

void Simulator::errorCheckingBeforConnecting(ifstream& inputFile){
	for (int k = 0; k < numberOfElemets_; k++)
		if (allElements_[k] == nullptr) {
			inputFile.close();
			throw new ElementMissing("Jedan ili vise elementa nedostaju");
		}
	if (GenForReference_ == nullptr) {
		inputFile.close();
		throw GeneratorMissing("Ubacite barem jedan generator");
	}
	if (probes_.empty()) {
		inputFile.close();
		throw ProbeMissing("Ubacite barem jednu sondu");
	}
}
