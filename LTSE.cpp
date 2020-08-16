#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>    // std::find_if
#include <sstream>
#include <list>
#include <unordered_map>
#include "myExchange.cpp"

using namespace std;

//g++ -o se LTSE.cpp
int main()
{
  std::cout << "Welcome to LTSE!\n\n";
  
  string dir = "C:\\Olya\\LTSE\\";
  string inFirms = dir + "firms.txt";
  string inSymb = dir + "symbols.txt";
  string inTrades = dir + "trades.csv";
  
	myExchange *myEx = new myExchange(dir);
  
	std::ifstream inF(inFirms);
	std::ifstream inS(inSymb);
	std::ifstream inT(inTrades);
  
	std::string line;
    while (std::getline(inF, line)) 
		myEx->addFirm(trim(line));
	
    while (std::getline(inS, line))
		myEx->addSymbol(trim(line));
	
	inF.close(); inS.close();
	
	std::getline(inT, line);
	myEx->addOutFileHeader(line);
	
	int i = 0;
    while (std::getline(inT, line) && 2000 > i++) {
		//cout << "Line:" << i << "        " << line << "\n";
		myEx->processTrade(line);
	}
	
	inT.close();
}

