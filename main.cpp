#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <fstream>
#include <iomanip>      // std::setprecision
#include "CacheSimulator.h"

void show_simulation_results(CacheSimulator *L1_cache);
int main(int argc, char *argv[]) {
	if (argc < 9) return 0;	// insufficient argument.	
	int block_size = atoi(argv[1]);
	int L1_size = atoi(argv[2]);
	int L1_assoc = atoi(argv[3]);
	int L2_size = atoi(argv[4]);
	int L2_assoc = atoi(argv[5]);	
	string RP = "LRU";
	int argv6 = atoi(argv[6]);
	int Is_inclusive = atoi(argv[7]);
	char trace_file_name[100] = {'\0'};
	if(argv6 == 0){
		RP = "LRU";
	}
	if(argv6 == 1)
		RP = "FIFO";
	if(argv6 == 2)
		RP = "Optimal";
	strcpy(trace_file_name, argv[8]);
	std::vector<int> listOfAddress;
	if (RP == "Optimal") {
		// Prepare the reuse distance of each memory
		char rw;
		int address;
		int trace_num = 0;
		ifstream fin;
		fin.open(trace_file_name);
		while(fin >> rw >> hex >> address){
			trace_num ++;
			listOfAddress.push_back(address);
		}
		fin.close();
	}
	CacheSimulator *L1_cache, *L2_cache;
	if(RP == "Optimal")
	{
		L1_cache = new CacheSimulator(block_size, L1_size, L1_assoc, RP, Is_inclusive, &listOfAddress);
		if(L2_size != 0){
			L2_cache = new CacheSimulator(block_size, L2_size, L2_assoc, RP, Is_inclusive, &listOfAddress);		
			L1_cache ->Next_Cache=L2_cache;
			L2_cache->PreProcess();
		}
		L1_cache->PreProcess();
		
	}
	else {
		L1_cache = new CacheSimulator(block_size, L1_size, L1_assoc, RP, Is_inclusive, NULL);
		if(L2_size != 0){
			L2_cache = new CacheSimulator(block_size, L2_size, L2_assoc, RP, Is_inclusive, NULL);		
			L1_cache -> Next_Cache = L2_cache;
		}		
	}
	ifstream fin;

	fin.open(trace_file_name);
	char rw;
	int address;
	while(fin >> rw >> hex >> address){
		L1_cache->DoOperation(rw, address);
		if(L2_size != 0){
			if(Is_inclusive){
				if(L1_cache->is_write_back){
					L2_cache->DoOperation('w',L1_cache->evicted_address);
					if(L2_cache->is_evicted) L1_cache->invalidateMyCahceItem(L2_cache->evicted_address);
				}
				if(!L1_cache->is_hitted){
					L2_cache->DoOperation('r',address);
					if(L2_cache->is_evicted) L1_cache->invalidateMyCahceItem(L2_cache->evicted_address);
				}
				
			}else{
				if(L1_cache->is_write_back == true) L2_cache->DoOperation('w',L1_cache->evicted_address);
				if(!L1_cache -> is_hitted) L2_cache->DoOperation('r', address);
			}
		}
	}
	fin.close();
	// Show Simulator Congiuration
	cout << "===== Simulator configuration =====" << endl;
	cout << "BLOCKSIZE:             " << block_size << endl;
	cout << "L1_SIZE:               " << L1_size<< endl;
	cout << "L1_ASSOC:              " << L1_assoc << endl;
	cout << "L2_SIZE:               " << L2_size << endl;
	cout << "L2_ASSOC:              " << L2_assoc << endl;
	cout << "REPLACEMENT POLICY:    " << RP << endl;
	cout << "INCLUSION PROPERTY:    " << (Is_inclusive ? "inclusive" : "non-inclusive" )<< endl;
	cout << "trace_file:            " << trace_file_name << endl;
	
	show_simulation_results(L1_cache);
	return 0;
}

void show_simulation_results(CacheSimulator *L1_cache)
{
	CacheSimulator *L2_cache = L1_cache->Next_Cache;

	cout << "===== L1 contents =====" << endl;

	L1_cache->showContent();
	if(L2_cache != NULL)
	{
		cout << "===== L2 contents =====" << endl;
		L2_cache->showContent();
	}


	cout << "===== Simulation results (raw) =====" << endl;
	cout << "a. number of L1 reads:        " << L1_cache->getReadCount() << endl;
	cout << "b. number of L1 read misses:  " << L1_cache->getReadMissCount() << endl;
	cout << "c. number of L1 writes:       " << L1_cache->getWriteCount() << endl;
	cout << "d. number of L1 write misses: " << L1_cache->getWriteMissCount() << endl;
	cout << "e. L1 miss rate:              " << fixed << setprecision(6) << (float)(((float)(L1_cache->getReadMissCount() + L1_cache->getWriteMissCount()))/(L1_cache->getReadCount() + L1_cache->getWriteCount())) << endl;
	cout << "f. number of L1 writebacks:   " << L1_cache->getWriteBackCount() << endl;
	
	
	
	if(L2_cache == NULL)
	{	
		cout << "g. number of L2 reads:        " << 0 << endl;
		cout << "h. number of L2 read misses:  " << 0 << endl;
		cout << "i. number of L2 writes:       " << 0 << endl;
		cout << "j. number of L2 write misses: " << 0 << endl;
		cout << "k. L2 miss rate:              " << 0 << endl;
		cout << "l. number of L2 writebacks:   " << 0 << endl;
		cout << "m. total memory traffic:      " << L1_cache->get_mem_traffic_count() << endl;
	}
	else
	{	
		cout << "g. number of L2 reads:        " << L2_cache->getReadCount() << endl;                   	
		cout << "h. number of L2 read misses:  " << L2_cache->getReadMissCount() << endl;                   
		cout << "i. number of L2 writes:       " << L2_cache->getWriteCount() << endl;
		cout << "j. number of L2 write misses: " << L2_cache->getWriteMissCount() << endl;
		cout << "k. L2 miss rate:              " << fixed << setprecision(6) << (float)L2_cache->getReadMissCount() / (float)L2_cache->getReadCount() << endl;
		cout << "l. number of L2 writebacks:   " << L2_cache->getWriteBackCount() << endl;
		cout << "m. total memory traffic:      " << L1_cache->getBackInvalidationCount() + L2_cache->get_mem_traffic_count() << endl;
	}
}
