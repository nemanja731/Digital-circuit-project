#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>
#include <iostream>

using namespace std;

class InvalidValue : public exception
{
public:
	InvalidValue(const char *errorDesc) : exception(errorDesc) {}
};
class SameDefinedElements : public exception
{
public:
	SameDefinedElements(const char *errorDesc) : exception(errorDesc) {}
};
class ElementMissing : public exception
{
public:
	ElementMissing(const char *errorDesc) : exception(errorDesc) {}
};
class AlreadyConnected : public exception
{
public:
	AlreadyConnected(const char *errorDesc) : exception(errorDesc) {}
};
class InvalidInput : public exception
{
public:
	InvalidInput(const char *errorDesc) : exception(errorDesc) {}
};
class InputMissing : public exception
{
public:
	InputMissing(const char *errorDesc) : exception(errorDesc) {}
};
class GeneratorMissing : public exception
{
public:
	GeneratorMissing(const char *errorDesc) : exception(errorDesc) {}
};
class ProbeMissing : public exception
{
public:
	ProbeMissing(const char *errorDesc) : exception(errorDesc) {}
};
#endif
