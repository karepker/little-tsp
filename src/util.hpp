#ifndef UTIL_H
#define UTIL_H

#include <exception>
#include <iosfwd>

/* class for errors that arise from an incorrect implementation */
class ImplementationError : public std::exception {
public:
	ImplementationError(const char* msg) : msg_{msg} {}
	const char* what() const noexcept override { return msg_; }

private:
	const char* msg_;
};

/* class for errors that arise from bad input (e.g. bad graph input file) */
class Error : public std::exception {
public:
	Error(const char* msg) : msg_{msg} {}
	const char* what() const noexcept override { return msg_; }

private:
	const char* msg_;
};

/* provides a basic encapsulation of a coordinate */
struct Coordinate {
	int x;
	int y;
};

#endif  // UTIL_H
