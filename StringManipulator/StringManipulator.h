#pragma once
#include <vector>
#include <list>
#include <string>

using namespace std;

namespace StringManipulator {
	class Tokenizer {
	public:
		Tokenizer();
		void add_deliminator(char deliminator);
		void add_token(char token);
		void add_locker(char locker);
		void add_encloser(char left, char right);
		void add_wrapper(char left, char right);
		vector<string> tokenize_to_vector(const std::string& theString);
		list<string> tokenize_to_list(const std::string& theString);
		void tokenize(vector<string>& dest, const std::string& theString);
		void tokenize(list<string>& dest, const std::string& theString);
		string get_a_token(string& source);

	private:
		bool match_deliminators(char character);
		bool match_tokens(char character);
		bool match_lockers(char character);
		bool match_left_wrappers(char character, char& right);
		bool match_left_enclosers(char character, char& right);
		struct Encloser
		{
			Encloser(char _left, char _right)
			{
				left = _left;
				right = _right;
			}
			char left;
			char right;
		};
		vector<char> deliminators;
		vector<Encloser> enclosers;
		vector<char> lockers;
		vector<Encloser> wrappers;
		vector<char> tokens;
	};

	void to_lower_case(std::string& source);
	void to_lower_case(char* source);

	inline std::string& left_trim(std::string& theString, const char* trimmers = " \t\n\r\f\v")
	{
		theString.erase(0, theString.find_first_not_of(trimmers));
		return theString;
	}
	inline std::string& right_trim(std::string& theString, const char* trimmers = " \t\n\r\f\v")
	{
		theString.erase(theString.find_last_not_of(trimmers) + 1);
		return theString;
	}
	inline std::string& trim(std::string& theString, const char* trimmers = " \t\n\r\f\v")
	{
		return left_trim(right_trim(theString, trimmers), trimmers);
	}
	inline std::string left_trim_copy(std::string theString, const char* trimmers = " \t\n\r\f\v")
	{
		theString.erase(0, theString.find_first_not_of(trimmers));
		return theString;
	}
	inline std::string right_trim_copy(std::string theString, const char* trimmers = " \t\n\r\f\v")
	{
		theString.erase(theString.find_last_not_of(trimmers) + 1);
		return theString;
	}
	inline std::string trim_copy(std::string theString, const char* trimmers = " \t\n\r\f\v")
	{
		return left_trim(right_trim(theString, trimmers), trimmers);
	}
}
