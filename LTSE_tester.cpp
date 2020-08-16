#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>    // std::find_if
#include <sstream>
#include <list>
#include <unordered_map>
#include "myExchange.cpp"
#include <cassert>

using namespace std;

//g++ -o setest LTSE_tester.cpp
int main()
{
  std::cout << "Welcome to LTSE tester!\n\n";
  
  string dir = "C:\\Olya\\LTSE\\";
  
	myExchange *myEx = new myExchange(dir);
  
	//Setup
	myEx->addFirm("Fidelity");
	myEx->addFirm("Fidelity");
	myEx->addFirm("Charles Schwab");
	myEx->addFirm("Ameriprise Financial");
	myEx->addFirm("TD Ameritrade");
	myEx->addFirm("Fidelity");
	
	myEx->addSymbol("BARK");
	myEx->addSymbol("BARK");
	myEx->addSymbol("CARD");
	myEx->addSymbol("HOOF");
	myEx->addSymbol("LOUD");
	myEx->addSymbol("BARK");
	
	string hdr = "Time stamp,broker,sequence id,type,Symbol,Quantity,Price,Side";
	myEx->addOutFileHeader(hdr);
	
	myEx->processTrade("10/5/2017 10:00:11,TD Ameritrade,1,K,LOUD,200,10.95,Buy");			// valid #1
	myEx->processTrade("10/5/2017 10:00:11,TD Ameritrade,1,K,LOUD,200,10.95,Buy");			// invalid seq
	myEx->processTrade("10/5/2017 10:00:12,TD Ameritrade,2,K,LOUD,200,10.95,Buy");			// valid #2
	myEx->processTrade("10/5/2017 10:00:40,Fidelity,2,2,LGHT,100,140.05,Buy");				//invalid sym
	myEx->processTrade("10/5/2017 10:00:41,Fidelity,3,K,BARK,100,1.19,Buy");				// valid #3
	myEx->processTrade("10/5/2017 10:00:41,Fidelity,4,K,BARK,100,1.19,Buy");				// valid #4
	myEx->processTrade("10/5/2017 10:00:41,Fidelity,5,K,,100,1.19,Buy");					//missing field
	myEx->processTrade("10/5/2017 10:00:41,TD Ameritrade,3,K,LOUD,200,10.95,Buy");			// valid #5
	myEx->processTrade("10/5/2017 10:00:42,TD Ameritrade,4,K,LOUD,200,10.95,Buy"); 			// > 3 per min
	myEx->processTrade("10/5/2017 10:00:42,Charles Schwab,2,2,DARK,100,20,Buy");			//invalid sym
	myEx->processTrade("10/5/2017 10:00:42,Charles Schwab,3,K,HOOF,100,39.11,Buy");			// valid #6
	myEx->processTrade("10/5/2017 10:00:43,Edward Jones,2,2,BARK,400,88.3,Buy"); 			//invalid broker
	myEx->processTrade("10/5/2017 10:00:43,TD Ameritrade,5,2,GLOO,500,40.01,Sell");			// > 3 per min
	myEx->processTrade("10/5/2017 10:00:44,Ameriprise Financial,2,K,LGHT,800,98.765,Sell"); //invalid sym
	myEx->processTrade("10/5/2017 10:00:44,Ameriprise Financial,3,2,LOUD,400,10.96,Sell");	// valid #7
	myEx->processTrade("10/5/2017 10:00:45,Ameriprise Financial,4,2,LOUD,400,10.96,Sell");	// valid #8
	myEx->processTrade("10/5/2017 10:00:46,Ameriprise Financial,5,2,LOUD,400,10.96,Sell");	// valid #9
	myEx->processTrade("10/5/2017 10:00:47,Ameriprise Financial,6,2,LOUD,400,10.96,Sell");	// > 3 per min
	myEx->processTrade("10/5/2017 10:00:48,Ameriprise Financial,7,2,LOUD,400,10.96,Sell");	// > 3 per min
	myEx->processTrade("10/5/2017 10:00:49,Ameriprise Financial,8,2,LOUD,400,10.96,Sell");	// > 3 per min
	
	
	cout << "\nStatistics:\n";
	myEx->showFirms();
	myEx->showSymbols();
	cout << endl;
	cout << "Firms count: " << myEx->getFirmCount() << endl;
	assert(myEx->getFirmCount() == 4);
	
	cout << "Symbols count: " << myEx->getSymCount() << endl; 
	assert(myEx->getSymCount() == 4);
	
	cout << "Invalid Trades count: " << myEx->getInvalidTradesCount() << endl; 
	assert(myEx->getInvalidTradesCount() == 11);
	
	int count = 0;
	int sum = 0;
	for (int i = 0; count >= 0; i ++) {
		count = myEx->getTradesCount(i);
		if (count >= 0)
			sum += count;
	}
	cout << "Valid Trades count: " << sum << endl;
	cout << "Total Trades count: " << sum + myEx->getInvalidTradesCount() << endl;
	
	assert(sum == 9); 									//valid trades count
	assert(sum + myEx->getInvalidTradesCount() == 20);	//total trades count
	
	return 0;
}

