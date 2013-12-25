/*
 * OrkaException.h
 *
 *  Created on: Dec 15, 2013
 *      Author: vilhelm
 */

#ifndef ORKAEXCEPTION_H_
#define ORKAEXCEPTION_H_

namespace orka {

class OrkaException: virtual public std::exception {
public:
	OrkaException(std::string description) :
			description_(description) {
	}
	virtual ~OrkaException() throw () {
	}
	virtual const char* what() const throw () {
		return this->description_.c_str();
	}
private:
	std::string description_;
};

} // end namespace orka

#endif /* ORKAEXCEPTION_H_ */
