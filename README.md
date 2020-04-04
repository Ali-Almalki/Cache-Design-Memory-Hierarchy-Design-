# Cache-Design-Memory-Hierarchy-Design-
1- Implement a flexible cache and memory hierarchy simulator and use it to compare the performance, area, and energy of different memory hierarchy configurations, using a subset of the SPEC-2000 benchmark suite.

2. Specification of Memory Hierarchy 
 
Design a generic cache module that can be used at any level in a memory hierarchy. For example, this cache module can be “instantiated” as an L1 cache, an L2 cache, an L3 cache, and so on. Since it can be used at any level of the memory hierarchy, it will be referred to generically as CACHE throughout this specification. 
 
2.1. Configurable parameters 
 
CACHE should be configurable in terms of supporting any cache size, associativity, and block size, specified at the beginning of simulation:    o SIZE: Total bytes of data storage.  o ASSOC: The associativity of the cache (ASSOC = 1 is a direct-mapped cache).  o    BLOCKSIZE: The number of bytes in a block. 
 
There are a few constraints on the above parameters: 1) BLOCKSIZE is a power of two and 2) the number of sets is a power of two. Note that ASSOC (and, therefore, SIZE) need not be a power of two. As you know, the number of sets is determined by the following equation:  #	𝑠𝑒𝑡𝑠= 𝑆𝐼𝑍𝐸 𝐴𝑆𝑆𝑂𝐶	×	𝐵𝐿𝑂𝐶𝐾𝑆𝐼𝑍𝐸  
 
 
 2 
2.2. Replacement policy 
 
All students need to implement three replacement policies: LRU (least-recently-used), FIFO (firstin-first-out) and optimal policy. Replacement policy will be a configurable parameter for the CACHE simulator. 
 
2.2.1. LRU policy 
 
Replace the block that was least recently touched (updated on hits and misses). 
 
2.2.2. FIFO policy 
 
Replace the block that was placed first in the cache. 
 
2.2.3. Optimal policy 
 
Replace the block that will be needed farthest in the future. Note that this is the most difficult replacement policy and it is impossible to implement in a real system. This will need preprocessing the trace to determine reuse distance for each memory reference (i.e. how many accesses later we will need this cache block). You can then run the actual cache simulation on the output of the preprocessing stage. Note: 
 If there is more than one block (in a set) that’s not going to be reused again in the trace, replace the leftmost one that comes up from the search. 
 
2.3. Write Policy 
 
 
CACHE should use the WBWA (write-back + write-allocate) write policy. o Write-allocate: A write that misses in CACHE will cause a block to be allocated in  CACHE. Therefore, both write misses and read misses cause blocks to be allocated in CACHE.  o Write-back: A write updates the corresponding block in CACHE, making the block dirty. It does not update the next level in the memory hierarchy (next level of cache or memory). If a dirty block is evicted from CACHE, a writeback (i.e., a write of the entire block) will be sent to the next level in the memory hierarchy. 
 
2.4. Allocating a block: Sending requests to next level in the memory hierarchy 
 
Your simulator must be capable of modeling one or more instances of CACHE to form an overall memory hierarchy, as shown in Fig. 1. 
 
CACHE receives a read or write request from whatever is above it in the memory hierarchy (either the CPU or another cache). The only situation where CACHE must interact with the next level below it (either another CACHE or main memory) is when the read or write request misses in CACHE. When the read or write request misses in CACHE, CACHE must “allocate” the requested block so that the read or write can be performed. 
 
 3 
Thus, let us think in terms of allocating a requested block X in CACHE. The allocation of requested block X is actually a two-step process. The two steps must be performed in the following order. 
 1. Make space for the requested block X. If there is at least one invalid block in the set, then there is already space for the requested block X and no further action is required (go to step 2). On the other hand, if all blocks in the set are valid, then a victim block V must be singled out for eviction, according to the replacement policy (Section 3.2). If this victim block V is dirty, then a write of the victim block V must be issued to the next level of the memory hierarchy.  2. Bring in the requested block X. Issue a read of the requested block X to the next level of the memory hierarchy and put the requested block X in the appropriate place in the set (as per step 1). 
