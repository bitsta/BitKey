//###############################################################
//# project: librootcoin										#
//# author:  bitsta												#
//# date:    08.11.2014											#
//# spec:    Type-definition for librootcoin. 					#
//# version: 0.1 - THIS IS EXPERIMENTAL SOFTWARE! 				#
//###############################################################


#ifndef LIBROOTCOIN_EXCEPTION_H
#define LIBROOTCOIN_EXCEPTION_H

#include <string>
#include <sstream>

class RootcoinException: public std::exception
{
private:
	int code;
	std::string msg;

public:
	explicit RootcoinException(int errcode, const std::string& message) {
		this->code = errcode;
		this->msg = parse(message);

	}
	~RootcoinException() throw() { };

	int getCode(){
		return code;
	}

	std::string getMessage(){
		return msg;
	}

	std::string parse(const std::string& in){
		std::string out = in;
		std::string pattern = ": ";
		unsigned int pos = out.find(pattern);
		if(pos <= out.size()){
			out.erase(pos, pattern.size());
			out[0] = toupper(out[0]);
		}

		return out;
	}

	virtual const char* what() const throw (){
		std::stringstream out;
		out << "Error " << code << ": " << msg;
		return out.str().c_str();
	}
};

#endif
