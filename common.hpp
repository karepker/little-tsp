#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <sstream>

class MessageError
{

public:
	std::string message;
	MessageError(std::string m) : message(m) {};

	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, const MessageError& me)
	{ os << me.message; return os; };
};

class IndexOutOfBoundsError
{

public:
	unsigned int attempt;
	unsigned int max;
	std::string name;
	IndexOutOfBoundsError(unsigned int a, unsigned int m, std::string n) : 
		attempt(a), max(m), name(n) {};

	// ostream operator
	friend std::ostream& operator<<(std::ostream& os, 
		const IndexOutOfBoundsError& ie)
	{ os << "You tried to access \"" << ie.attempt << 
		"\" of \"" << ie.name << "\" when max index is \"" << 
		ie.max << "\"" << std::endl; return os; };
};

#endif // COMMON_H
