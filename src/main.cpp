#include<iostream>
#include<string>
#include<sstream>
#include<vector>

std::vector<std::string> tokenizeWhitespace(const std::string& input){

std::istringstream stream(input);
std::vector<std::string> tokens;
std::string token;

while(stream >> token){
tokens.push_back(token);
}
return tokens;
}

int main(){

char* username=getenv("USER");

while(true){

std::cout<<"\033[1;31m"<<username<<"\033[0m"<<"\033[1;36m@minishell>\033[0m ";

std::string input;
std::getline(std::cin,input);

if(input=="exit"){
break;
}

std::vector<std::string> tokens=tokenizeWhitespace(input);

for(auto token:tokens)
std::cout<<"["<<token<<"]\n";

}

return 0;
}


