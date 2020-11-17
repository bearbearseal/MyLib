#include <string.h>
#include "StringManipulator.h"

StringManipulator::Tokenizer::Tokenizer() {
}

void StringManipulator::Tokenizer::add_deliminator(char deliminator)
{
	deliminators.push_back(deliminator);
}

void StringManipulator::Tokenizer::add_encloser(char left, char right)
{
	Encloser encloser(left, right);
	enclosers.push_back(encloser);
}

void StringManipulator::Tokenizer::add_wrapper(char left, char right) {
	Encloser wrapper(left, right);
	wrappers.push_back(wrapper);
}

void StringManipulator::Tokenizer::add_locker(char locker) {
	lockers.push_back(locker);
}

void StringManipulator::Tokenizer::add_token(char token) {
	tokens.push_back(token);
}


vector<string> StringManipulator::Tokenizer::tokenize_to_vector(const std::string& input)
{
	vector<string> retVal;
	tokenize(retVal, input);
	return retVal;
}

list<string> StringManipulator::Tokenizer::tokenize_to_list(const std::string& input)
{
	list<string> retVal;
	tokenize(retVal, input);
	return retVal;
}

void StringManipulator::Tokenizer::tokenize(vector<string>& dest, const std::string& inData)
{
	string theString = inData;
	string aToken;
	unsigned tokenStartIndex = 0;
	for (unsigned i = 0; i<theString.length(); i++)
	{
		char rightEncloser;
		if (match_deliminators(theString[i]))
		{
			if (i - tokenStartIndex > 0)
			{
				aToken = theString.substr(tokenStartIndex, i - tokenStartIndex);
				dest.push_back(aToken);
			}
			tokenStartIndex = i + 1;
		}
		else if (match_left_enclosers(theString[i], rightEncloser))
		{
			if (i - tokenStartIndex > 0)
			{
				aToken = theString.substr(tokenStartIndex, i - tokenStartIndex);
				dest.push_back(aToken);
			}
			tokenStartIndex = i;
			size_t tokenEndIndex = theString.find(rightEncloser, i);
			if (tokenEndIndex != string::npos)
			{
				aToken = theString.substr(tokenStartIndex, tokenEndIndex - tokenStartIndex + 1);
				dest.push_back(aToken);
				i = tokenStartIndex = tokenEndIndex + 1;
			}
		}
	}
	if (theString.length() > tokenStartIndex)
	{
		aToken = theString.substr(tokenStartIndex, theString.length() - tokenStartIndex);
		dest.push_back(aToken);
	}
}


void StringManipulator::Tokenizer::tokenize(list<string>& dest, const std::string& inData)
{
	string theString = inData;
	string aToken;
	unsigned tokenStartIndex = 0;
	for (unsigned i = 0; i<theString.length(); i++)
	{
		char rightEncloser;
		if (match_deliminators(theString[i]))
		{
			if (i - tokenStartIndex > 0)
			{
				aToken = theString.substr(tokenStartIndex, i - tokenStartIndex);
				dest.push_back(aToken);
			}
			tokenStartIndex = i + 1;
		}
		else if (match_left_enclosers(theString[i], rightEncloser))
		{
			if (i - tokenStartIndex > 0)
			{
				aToken = theString.substr(tokenStartIndex, i - tokenStartIndex);
				dest.push_back(aToken);
			}
			tokenStartIndex = i;
			size_t tokenEndIndex = theString.find(rightEncloser, i);
			if (tokenEndIndex != string::npos)
			{
				aToken = theString.substr(tokenStartIndex, tokenEndIndex - tokenStartIndex + 1);
				dest.push_back(aToken);
				i = tokenStartIndex = tokenEndIndex + 1;
			}
		}
	}
	if (theString.length() > tokenStartIndex)
	{
		aToken = theString.substr(tokenStartIndex, theString.length() - tokenStartIndex);
		dest.push_back(aToken);
	}
}

string StringManipulator::Tokenizer::get_a_token(string& source) {
	string retVal;
	char right;
	if (!source.length()) {
		return retVal;
	}
	for (unsigned i = 0; i < source.length();) {
		if (match_deliminators(source[i])) {
			if (i==0) {
				source = source.substr(1, source.length());
				continue;
			}
			retVal = source.substr(0, i);
			source = source.substr(i + 1, source.length());
			return retVal;
		}
		else if (match_tokens(source[i])) {
			if (i>0) {
				retVal = source.substr(0, i);
				source = source.substr(i, source.length());
			}
			else {
				retVal = source[0];
				source = source.substr(1, source.length());
			}
			return retVal;
		}
		else if (match_lockers(source[i])) {
			size_t secondLockerIndex = source.find(source[i], i + 1);
			if (i>0) {
				if (secondLockerIndex != string::npos) {
					retVal = source.substr(0, i);
					source = source.substr(i, source.length());
					return retVal;
				}
			}
			else if (secondLockerIndex != string::npos) {
				retVal = source.substr(0, secondLockerIndex+1);
				if (secondLockerIndex < source.length()) {
					source = source.substr(secondLockerIndex + 1, source.length());
				}
				else {
					source = "";
				}
				return retVal;
			}
		}
		else if (match_left_enclosers(source[i], right)) {
			size_t rightEncloserIndex = source.find(right, i + 1);
			if (i>0) {
				if (rightEncloserIndex != string::npos) {
					retVal = source.substr(0, i);
					source = source.substr(i, source.length());
					return retVal;
				}
			}
			else if (rightEncloserIndex != string::npos) {
				retVal = source.substr(0, rightEncloserIndex + 1);
				if (rightEncloserIndex < source.length()) {
					source = source.substr(rightEncloserIndex + 1, source.length());
				}
				else {
					source = "";
				}
				return retVal;
			}
		}
		else if (match_left_wrappers(source[i], right)) {
			//printf("Found a left wrapper.\n");
			size_t rightWrapperIndex = source.find(right, i + 1);
			if (rightWrapperIndex != string::npos) {
				i = rightWrapperIndex;
				//printf("Right wrapper at %u %c.\n", rightWrapperIndex, source[i]);
			}
		}
		i++;
	}
	retVal = source;
	source = "";
	return retVal;
}

bool StringManipulator::Tokenizer::match_deliminators(char character)
{
	for (vector<char>::iterator i = deliminators.begin(); i != deliminators.end(); i++)
	{
		if ((*i) == character)
		{
			return true;
		}
	}
	return false;
}

bool StringManipulator::Tokenizer::match_tokens(char character) {
	for (vector<char>::iterator i = tokens.begin(); i != tokens.end(); i++) {
		if ((*i) == character) {
			return true;
		}
	}
	return false;
}

bool StringManipulator::Tokenizer::match_lockers(char character) {
	for (vector<char>::iterator i = lockers.begin(); i != lockers.end(); i++) {
		if ((*i) == character) {
			return true;
		}
	}
	return false;
}

bool StringManipulator::Tokenizer::match_left_wrappers(char character, char& right) {
	for (vector<Encloser>::iterator i = wrappers.begin(); i != wrappers.end(); i++)
	{
		if (i->left == character)
		{
			right = i->right;
			return true;
		}
	}
	return false;
}

bool StringManipulator::Tokenizer::match_left_enclosers(char character, char& right) {
	for (vector<Encloser>::iterator i = enclosers.begin(); i != enclosers.end(); i++)
	{
		if (i->left == character)
		{
			right = i->right;
			return true;
		}
	}
	return false;
}

void StringManipulator::to_lower_case(std::string& source)
{
	for (unsigned i = 0; i<source.length(); i++)
	{
		if (source[i] <= 'Z' && source[i] >= 'A')
		{
			source[i] = source[i] - ('Z' - 'z');
		}
	}
}

void StringManipulator::to_lower_case(char* source)
{
	size_t length = strlen(source);
	for (unsigned i = 0; i<length; i++)
	{
		if (source[i] <= 'Z' && source[i] >= 'A')
		{
			source[i] = source[i] - ('Z' - 'z');
		}
	}
}
