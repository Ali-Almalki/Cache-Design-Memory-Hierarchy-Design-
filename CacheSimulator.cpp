#include "CacheSimulator.h"
#include <math.h>

using namespace std;

CacheSimulator :: CacheSimulator(int block_size, int cache_size, int assoc_size, string RP, int Is_inclusive, vector<int> * listOfAddress =NULL)
{
	Init();
	this->assoc_size = assoc_size;
	this->RP = RP;
	this->Is_inclusive = Is_inclusive;				
	this->set_count = (cache_size)/(assoc_size * block_size);	
	this->offset_length = (int)log2(block_size);
	this->index_length = (int)log2(set_count);	
	block = new MyCahceItem*[set_count];
	for(int i=0; i<set_count; i++)
		block[i] = new MyCahceItem[assoc_size];	
	this -> listOfAddress = listOfAddress;
	Next_Cache = NULL;
}
void CacheSimulator :: Init(){
	this->read_cnt = 0;
	this->write_cnt = 0;
	this->read_miss_cnt = 0;
	this->write_miss_cnt = 0;
	this->write_back_count = 0;
	this->mem_traffic_count = 0;
	this->back_invalidation_cnt = 0;
	this->trace_number = 0;
}
void CacheSimulator :: PreProcess(){
	if(RP != "Optimal") return;
	for (int i = 0; i < this->listOfAddress->size(); i++) {
		int address = listOfAddress->at(i);
		int tag = this->getTag(address);
		int index = this->getIndex(address);
		int tag_index_address = this->getTagIndex(address);

		vector<int> v = this->Reuse_Distance_map[tag_index_address];
		v.push_back(i + 1);
		this->Reuse_Distance_map[tag_index_address] = v;
	}
}

void CacheSimulator :: DoOperation(char rw, int address)
{
	
	this->is_hitted = false;
	this->is_evicted = false;
	this->is_write_back = false;
	
	trace_number++;

	if(rw == 'r')
		read_cnt++;
	else
		write_cnt++; 

	MyCahceItem *cache_line = getBlockIfExist(address);
	if(cache_line == NULL)					// address Miss in Cache
	{
		this->is_hitted = false;		
		if(rw == 'r') read_miss_cnt++;
		else write_miss_cnt++;
		mem_traffic_count++;
		cache_line = loadBlockAndReplace(address);			// Function to replace MyCahceItem in case of Miss
	}
	else							// address Hit in Cache
	{
		this->is_hitted = true;		
		if(RP == "LRU")
			cache_line->factor_number = trace_number;
	}

	if(rw == 'w') cache_line->flag = "Dirty";
}

MyCahceItem * CacheSimulator :: getBlockIfExist(int address)
{
	int Tag = getTag(address);
	int Index = getIndex(address);
	for(int i=0; i<assoc_size; i++)
	{
		if((block[Index][i].tag == Tag) && (block[Index][i].flag != "Invalid")){			
			return &(block[Index][i]);
		}
	}
	return NULL;
}

MyCahceItem *CacheSimulator :: loadBlockAndReplace(int address)
{
	int Tag = getTag(address);
	int Index = getIndex(address);
	MyCahceItem *block_toReplace = NULL;	

	// Check If there is Invali Block.
	for(int i=0; i<(int)assoc_size; i++)
	{
		if(block[Index][i].flag == "Invalid")
		{
			block_toReplace = &block[Index][i];
			break;
		}
	}

	if(block_toReplace == NULL)
	{
		block_toReplace = applyReplacementPolicy(Index);
		this->is_evicted = true;
		this->evicted_address = block_toReplace->address;
	}

	if(block_toReplace->flag == "Dirty")
	{
		this->is_write_back = true;
		this->write_back_count++;
		this->mem_traffic_count++;
	}

	block_toReplace->tag = Tag;
	block_toReplace->address = address;
	block_toReplace->flag = "Valid";
	block_toReplace->factor_number = trace_number;

	return block_toReplace;
}

void CacheSimulator :: showContent() {
	if(block == NULL) return;			
	for (int i = 0; i < set_count; i ++){
		cout << "Set     " << dec << i << ":      ";
		for (int j =0; j < assoc_size; j ++){
			cout << hex << block[i][j].tag;
			if(block[i][j].flag == "Dirty") 
				cout << " D  ";
			else
				cout << "    ";
		}
		cout << endl;
	}
	cout << dec;
}
void CacheSimulator :: invalidateMyCahceItem(int address)
{
	MyCahceItem *cache_line = getBlockIfExist(address);
	if (cache_line != NULL)
	{
		if(Is_inclusive == 1 && Next_Cache)
		{
			if(cache_line->flag == "Dirty")
			{
				this->is_write_back = true;
				back_invalidation_cnt++;
			} 
		}
		cache_line->flag = "Invalid";	
	}
}

MyCahceItem *CacheSimulator :: applyReplacementPolicy(int Index)
{
	int tmp_index;
	int min = trace_number;
	if (RP == "LRU" || RP == "FIFO")
	{
		for(int i=0; i<(int)assoc_size; i++)
		{
			if(block[Index][i].factor_number < min)	
			{
				min = block[Index][i].factor_number;
				tmp_index = i;
			}
		}
	}

	if(RP == "Optimal"){		
		int max = 0;
		for(int i = int(assoc_size)-1; i>= 0; i --){
			int Tag = block[Index][i].tag;
			int TagIndex = ((Tag << index_length) | (Index));
			std::vector<int> v = Reuse_Distance_map[TagIndex];
			int reuse_distance = -1;
			for(int j =0; j< v.size(); j ++){
				if(v[j]-trace_number > 0)
				{
					reuse_distance = v[j]-trace_number;
					break;
				}
			}
			if((max != -1 && max <= reuse_distance) || reuse_distance == -1){				
				tmp_index = i;
				max = reuse_distance;
			}
		}
	}	
	return &(block[Index][tmp_index]);
}