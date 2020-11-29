#include <stdio.h>
#include <string>
#include "StringManipulator.h"
#include <iostream>

int main(){
	string formatted = "6国\t产CPU,1999元兆芯4核W\\nin10轻薄笔记本\\n测试\\n";
	string formatted1 = "\\n6国产CPU,1999元兆芯4核W\\nin10轻\t薄笔记本\\n测试";
	std::cout<<StringManipulator::Formatter::to_unformatted(formatted)<<endl;
	std::cout<<StringManipulator::Formatter::to_unformatted(formatted1)<<endl;
	std::string source = "ADD dest 1 \"trtr\"";
	StringManipulator::Tokenizer tokenizer;
	tokenizer.add_deliminator(' ');
	tokenizer.add_wrapper('\"', '\"');
	std::vector<std::string> tokens = tokenizer.tokenize_to_vector(source.c_str());
	for(unsigned i=0; i< tokens.size(); i++){
		printf("%s\n", tokens[i].c_str());
	}
}
