#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>    // std::find_if
#include <sstream>
#include <list>
#include <unordered_map>

using namespace std;

class myExchange {
	
	struct trade {										//data struct for trade info
		string time_stamp_date, broker, sequence_id,
			type, symbol, quantity, price, side ;
		int sec;
		
		trade(string ts, int sec, string brk, string sid,
			string tp,string sym,string qnt,string prc,string side)
			: time_stamp_date(ts), sec(sec), broker(brk), sequence_id(sid),
			type(tp), symbol(sym), quantity(qnt), price(prc), side(side) {};
	};
	
	std::vector<pair<string,int>> _firms;				//array of name/id pairs
	std::vector<string> _symbols;						//array of symbols
	
	std::list<trade> _ivtrades;							//invalid trades collection
	std::unordered_map<int, std::list<trade>> _vtrades;	//valid trades collection per firm
														//hash map structure
	
	int _firmCount = 0;									//count of firms

	std::ofstream *outRejected;
	std::ofstream *outValid;
	

public:	
	myExchange (string &work_dir) {
		string outRej = work_dir + "rejected.txt";
		string outComp = work_dir + "accepted.txt";
		
		outRejected = new ofstream();
		outValid = new ofstream();
		
		try
		{
			cout << "Opening out files....\n";
			outRejected->open(outRej, std::ofstream::out);
			outValid->open(outComp, std::ofstream::out);
			
			if (!outValid->is_open() || !outRejected->is_open()) {
				throw runtime_error("Could not open files in dir " + work_dir);
			}
		}
		catch (exception& e)
		{
			cerr << "ERROR Standard exception: " << e.what() << endl;
			exit(-1) ;
		}
	}
	
	//flush and close files on exit
	~myExchange() {
		outRejected->flush();
		outValid->flush();
		outRejected->close();
		outValid->close();
	}
	
	void addOutFileHeader(string &hdr) {
		*outValid << hdr << endl;
		*outRejected << hdr << endl;
	}
	
	//check if firm exists in the collection
	bool findFirm(string &firm) {
		return find_if(_firms.begin(), _firms.end(), [&](pair<string,int> &s)
			{return s.first == firm;})
				!= _firms.end();
	}
	
	//get assigned firm id
	int getFirm(string &firm) {
		auto it = find_if(_firms.begin(), _firms.end(), [&](pair<string,int> &s)
			{return s.first == firm;});
		if (it != _firms.end()) return it->second;
		return -1;
	}
	
	bool findSeqId(int bk, string sid) {
		return find_if(_vtrades[bk].begin(), _vtrades[bk].end(), [&](trade &t)
			{return t.sequence_id == sid;})
				!= _vtrades[bk].end();
	}
	
	//check if symbol exists in the collection
	bool findSymbol(string &sym) {
		return find(_symbols.begin(), _symbols.end(), sym) != _symbols.end();
	}
	
	void addFirm(string &&firm) {
		if(!findFirm(firm))
		_firms.push_back(make_pair(firm,_firmCount++));
	}
	
	void addFirm(string &firm) {
		if(!findFirm(firm))
		_firms.push_back(make_pair(firm,_firmCount++));
	}
	
	void addSymbol(string &&sym) {
		if(!findSymbol(sym))
			_symbols.push_back(sym);
	}
	
	void addSymbol(string &sym) {
		if(!findSymbol(sym))
			_symbols.push_back(sym);
	}
	
	void showFirms() {
		std::cout << "\n  Firms:\n";
		for(auto x : _firms)
			std::cout << x.second << " - " << x.first << "\n";
	}
	
	void showSymbols() {
		std::cout << "\n  Symbls:\n";
		for(auto x : _symbols)
			std::cout << x << "\n";
	}
	
	//-------------------  get counts --------------------------
	int getFirmCount() {
		return _firms.size();
	}
	
	int getSymCount() {
		return _symbols.size();
	}
	
	int getInvalidTradesCount() {
		return _ivtrades.size();
	}
	
	int getTradesCount(int bk_id) {
		if (bk_id >= _firmCount) return -1;			//invalid broker requested
		return _vtrades[bk_id].size();
	}
	//-----------------------------------------------------------
	
	//process trade
	bool processTrade (string &&str) {
		return processTrade(str);
	}
	
	//process trade
	bool processTrade (string &str) {
		stringstream sLine(str);
		string time_stamp_date,time_stamp_hr,time_stamp_min,time_stamp_sec,broker,sequence_id,type,symbol,quantity,price,side;

		bool isValid = true;
		
		//get values from trade input
		getline(sLine, time_stamp_date, ' ');
		getline(sLine, time_stamp_hr, ':');
		getline(sLine, time_stamp_min, ':');
		getline(sLine, time_stamp_sec, ',');
		getline(sLine, broker, ',');
		getline(sLine, sequence_id, ',');
		getline(sLine, type, ',');
		getline(sLine, symbol, ',');
		getline(sLine, quantity, ',');
		getline(sLine, price, ',');
		getline(sLine, side, ',');
		
		//check if all values are present
		if(time_stamp_date.length() < 2 || time_stamp_hr.length() < 2 || 
			time_stamp_min.length() < 2 || time_stamp_sec.length() < 2 || 
			broker.length() < 2 || 
			sequence_id.length() < 1 || type.length() < 1 || 
			symbol.length() < 1 || quantity.length() < 2 || 
			price.length() < 2 || side.length() < 2)
		{
			isValid = false;
			cout << "WARNING: Missing fields: " << str << endl;
		}
		
		//check if symbol is valid
		if(!findSymbol(symbol)) {
			cout << "WARNING: Invalid symbol: " << symbol << endl;
			isValid = false;
		}
		
		//check if broker is valid
		int broker_id = getFirm(broker);
		//if(!findFirm(broker)) {
		if(broker_id == -1) {
			cout << "WARNING: Invalid broker: " << broker << endl;
			isValid = false;
		}
		
		//check if sequence id unique per broker
		if(findSeqId(broker_id, sequence_id)) {
			cout << "WARNING: Invalid sequence_id: " << sequence_id << " for " << broker << endl;
			isValid = false;
		}
		
		//seconds from midnight
		int sec = atoi(time_stamp_hr.c_str())*60*60 + atoi(time_stamp_min.c_str())*60 + atoi(time_stamp_sec.c_str());
		
		bool enough = false; int order_count = 0;
		//Check only trades in past 60 sec for a broker is under 3
		for(auto it=_vtrades[broker_id].rbegin(); it!=_vtrades[broker_id].rend() && !enough; ++it) {
			if(it->sec < sec - 60) enough = true;
			order_count++;
			if(!enough && order_count >= 3) {
				cout << "WARNING: More than 3 trades permin perbroker: " << broker << endl;
				cout << "  --> " << time_stamp_date << " "; 
				cout << time_stamp_hr << ":" << time_stamp_min << ":" << time_stamp_sec << " " ;
				cout << sec << " " << broker << " " << sequence_id << " " << type << " " << symbol << " " << quantity << " " << price << " " << side << endl;
				isValid = false;
				enough = true;
			}
		}
		
		trade trd(time_stamp_date, sec,broker,sequence_id,type,symbol,quantity,price,side);
		
		if(!isValid) {
			*outRejected << str + "\n";
			_ivtrades.push_back(trd);
			return false;
		}
		
		_vtrades[broker_id].push_back(trd);
		*outValid << str + "\n";
		
		return true;
	}
};

string trim(string& str)
{
    size_t first = str.find_first_not_of(" \n\r\t");
    size_t last = str.find_last_not_of(" \n\r\t");
    return str.substr(first, (last-first+1));
}
