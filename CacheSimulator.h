#ifndef __CacheSimulator_H__
#define __CacheSimulator_H__
#include <string>
#include <iostream>
#include <map>			// Dictionary 
#include <vector>

using namespace std;

class MyCahceItem
{
	public:
		int address;
		int tag;
		int factor_number; // LRU: lastly used trace number, FIFO: trace number when block is placed on cahce.
		string flag;
	public:
		MyCahceItem()
		{
			this->address = -1;
			this->tag = 0;
			this->factor_number = 0;
			this->flag = "Invalid";
		}
		~MyCahceItem(){

		}
};

class CacheSimulator
{
	public:
		MyCahceItem ** block;		
		int assoc_size;
		int set_count;
		CacheSimulator *Next_Cache;
		string RP;
		int Is_inclusive;
		int index_length, offset_length;
		int trace_number;
		int read_cnt;
		int write_cnt;
		int read_miss_cnt, write_miss_cnt;
		int write_back_count, mem_traffic_count, back_invalidation_cnt;		
		
		std::map<int, vector<int> > Reuse_Distance_map;
		std::vector<int> * listOfAddress;
		bool is_hitted, is_evicted, is_write_back;
		int evicted_address;	
	public:
		CacheSimulator(int, int, int, string, int, vector<int> *);
		~CacheSimulator(){}

		void Init();		
		void PreProcess();	// PreProcess for Optimal Replace Policy
		void DoOperation(char, int);
		MyCahceItem* getBlockIfExist(int);
		MyCahceItem* loadBlockAndReplace(int);		
		MyCahceItem* applyReplacementPolicy(int);
		void showContent();		
		void invalidateMyCahceItem(int);

		int getBackInvalidationCount() {return this-> back_invalidation_cnt;}
		int getReadCount() { return this->read_cnt;}
		int getWriteCount() {return this->write_cnt;}
		int getReadMissCount() {return this->read_miss_cnt;}
		int getWriteMissCount() {return this->write_miss_cnt;}
		int getWriteBackCount() {return this->write_back_count;}
		int get_mem_traffic_count() {return this->mem_traffic_count;}
		int getTag(int address) {return ((address)>>(index_length + offset_length));}
		int getTagIndex(int address) {return (address) >> (offset_length);}	

		int getIndex(int address) {
			int msk = 0;
			for(int i = 0; i < index_length + offset_length; i++){
				msk <<= 1;
				msk |= 1;
			}
			return ((address & msk)>>(offset_length));
		}		
		
};
#endif
