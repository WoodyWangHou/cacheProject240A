//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include "utils.h"
//
// TODO:Student Information
//
const char *studentName = "Hou Wang";
const char *studentID   = "A53241783";
const char *email       = "how038@eng.ucsd.edu";
const uint64_t ADDRESS_BITS = 32;

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

//
//TODO: Add your Cache data structures here
//
uint64_t blockOffsetBits;

uint64_t icacheSize; // I$ size
uint32_t iIndexBits;
uint32_t iTagBits;
// Cache stores array of pointers to sets (which is also array corresponding to ways)
uint32_t **icache; // I$ is an array, first dimension is set, second dimension a LRU set

uint64_t dcacheSize; // D$ size
uint32_t dIndexBits;
uint32_t dTagBits;
uint32_t **dcache; // D$ is an array, first dimension is set, second dimension a LRU set

uint64_t l2cacheSize; // L2$ size
uint32_t l2IndexBits;
uint32_t l2TagBits;
uint32_t **l2cache;

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//

void 
init_icache()
{
  icacheSize = icacheSets * icacheAssoc * blocksize;
  icache = (uint32_t **) malloc(sizeof(uint32_t *) * icacheSets);
  for(int i = 0; i < icacheSets; i++){
    icache[i] = (uint32_t *) malloc(sizeof(uint32_t) * icacheAssoc);
    memset(icache[i], 0, sizeof(uint32_t) * icacheAssoc);
  }

  iIndexBits = log2(icacheSets); 
  iTagBits = ADDRESS_BITS - blockOffsetBits - iIndexBits;
}

void 
init_dcache()
{
  dcacheSize = dcacheSets * dcacheAssoc * blocksize;
  dcache = (uint32_t **) malloc(sizeof(uint32_t *) * dcacheSets);
  for(int i = 0; i < dcacheSets; i++){
    dcache[i] = (uint32_t *) malloc(sizeof(uint32_t) * dcacheAssoc);
    memset(dcache[i], 0, sizeof(uint32_t) * dcacheAssoc);
  }

  dIndexBits = log2(dcacheSets);
  dTagBits = ADDRESS_BITS - blockOffsetBits - dIndexBits;
}

void 
init_l2cache()
{
  l2cacheSize = l2cacheSets * l2cacheAssoc * blocksize;
  l2cache = (uint32_t **) malloc(sizeof(uint32_t *) * l2cacheSets);
  for(int i = 0; i < l2cacheSets; i++){
    l2cache[i] = (uint32_t *) malloc(sizeof(uint32_t) * l2cacheAssoc);
    memset(l2cache[i], 0, sizeof(uint32_t) * l2cacheAssoc);
  }

  l2IndexBits = log2(l2cacheSets);
  l2TagBits = ADDRESS_BITS - blockOffsetBits - l2IndexBits;
}
void
init_cache()
{
  // Initialize cache stats
  icacheRefs        = 0;
  icacheMisses      = 0;
  icachePenalties   = 0;
  dcacheRefs        = 0;
  dcacheMisses      = 0;
  dcachePenalties   = 0;
  l2cacheRefs       = 0;
  l2cacheMisses     = 0;
  l2cachePenalties  = 0;
  
  //
  //TODO: Initialize Cache Simulator Data Structures
  //

  blockOffsetBits = log2(blocksize);

  // Corner case: when sets , assoc and block = 0 -> route I and D to l2
  init_icache();
  init_dcache();
  init_l2cache();

}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  //
  //TODO: Implement I$
  //
  if(icacheFind(addr) == TRUE){
    updateIcacheLRU(addr);
    return icacheHitTime;
  }
  
  // if not found
  return l2cache_access(addr);
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  //
  //TODO: Implement D$
  //
  return l2cache_access(addr);
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  //
  //TODO: Implement L2$
  //
  return memspeed;
}
