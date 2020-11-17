#include <stdio.h>
#include <string>
#include "StringManipulator.h"
#include <cstdio>

int main(){
	std::string source = "ADD dest 1 \"trtr\"";
	StringManipulator::Tokenizer tokenizer;
	tokenizer.add_deliminator(' ');
	tokenizer.add_wrapper('\"', '\"');
	std::vector<std::string> tokens = tokenizer.tokenize_to_vector(source.c_str());
	for(unsigned i=0; i< tokens.size(); i++){
		printf("%s\n", tokens[i].c_str());
	}
}
