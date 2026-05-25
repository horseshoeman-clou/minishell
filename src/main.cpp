#include<iostream>
#include<string>

int main(){

char* username=getenv("USER");

while(true){

std::cout<<"\033[1;31m"<<username<<"\033[0m"<<"\033[1;36m@minishell>\033[0m ";

std::string input;
std::getline(std::cin,input);

if(input=="exit"){
break;
}

std::cout<<"You typed: "<<input<<'\n';
}

return 0;
}
