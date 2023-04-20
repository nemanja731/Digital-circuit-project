#include "Element.h"
#include <stack>
double Element::currentInterval_ = 0;
double Element::nextInterval_ = 0;
double Element::currentTime_ = 0;

UserGenerator::~UserGenerator()
{
	changes_.clear();
}

bool UserGenerator::getState()
{
	if ((checkedOutputs_ == 1 && outputs_ > 1) || (checkedOutputs_ == 0 && outputs_ == 1))
	{
		remainingTime_ = remainingTime_ - currentInterval_;
		if (remainingTime_ <= 0)
		{
			state_ = !state_;
			remainingTime_ = changes_[changeIndex_++];
		}
		if (remainingTime_ <= nextInterval_)
			nextInterval_ = remainingTime_;
	}
	return state_;
}
bool Probe::getState()
{
	bool state;
	Element *top = this;
	stack<Element *> path;
	path.push(this);
	while (1)
	{
		while (top->checkedInputs_ < top->inputs_)
		{
			while (!top->checkForGenOrAlreadyEvalutedGate())
			{
				path.push(top->input_[top->checkedInputs_]);
				top = path.top();
			}
			top->checkedInputs_++;
		}
		top->checkedInputs_ = 0;
		path.pop();
		if (top == this)
		{
			return state = this->input_[0]->getState();
		}
		top->logic();
		top = path.top();
		top->checkedInputs_++;
	}
}

void Probe::writeState(bool newState)
{
	stringstream Stream;
	string num;
	if (this->state_ != newState)
	{
		Stream << currentTime_;
		Stream >> num;
		this->outputString_ += to_string((!newState ? 1 : 0)) + " -> " + to_string((newState ? 1 : 0)) + ": " + num + "us" + '\n';
		this->state_ = newState;
	}
	Stream.clear();
}

bool Element::getState()
{
	return this->state_;
}

void Element::logic()
{
	// dodaj gresku
}

bool Element::isInputGenerator() const
{
	if (input_.size() > 0)
	{
		if (input_[checkedInputs_]->type_ == USERGENERATOR || input_[checkedInputs_]->type_ == SIMPLEGENERATOR)
			return true;
		else
			return false;
	}
}

bool Element::checkForGenOrAlreadyEvalutedGate()
{
	bool Gen = isInputGenerator();
	int checked = input_[checkedInputs_]->checkedOutputs_;
	input_[checkedInputs_]->checkedOutputs_++;
	if (input_[checkedInputs_]->checkedOutputs_ == input_[checkedInputs_]->outputs_)
		input_[checkedInputs_]->checkedOutputs_ = 0;
	if (Gen)
		return true;
	if (checked == 0)
		return false;
	else
		return true;
}

bool SimpleGenerator::getState()
{
	if ((checkedOutputs_ == 1 && outputs_ > 1) || (checkedOutputs_ == 0 && outputs_ == 1))
	{
		remainingTime_ = remainingTime_ - currentInterval_;
		if (remainingTime_ <= 0)
		{
			state_ = !state_;
			remainingTime_ = halfPeriod_;
		}
		if (remainingTime_ < nextInterval_)
			nextInterval_ = remainingTime_;
	}
	return state_;
}

bool And::getState()
{
	return state_;
}

void And::logic()
{
	bool result;
	for (Element *t : this->input_)
	{
		if (t == input_[0])
			result = t->getState();
		else
		{
			result = t->getState() && result;
		}
	}
	this->state_ = result;
}

bool Or::getState()
{
	return state_;
}

void Or::logic()
{
	bool result;
	for (Element *t : this->input_)
	{
		if (t == input_[0])
			result = t->getState();
		else
		{
			result = t->getState() || result;
		}
	}
	this->state_ = result;
}

bool Not::getState()
{
	return state_;
}

void Not::logic()
{
	this->state_ = !this->input_[0]->getState();
}
