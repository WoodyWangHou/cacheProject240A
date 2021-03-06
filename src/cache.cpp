//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>
#include "cache.h"
#include "utils.h"

using namespace std;
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
// LRU Structure:
// blockLine -> MRU -> data 1 -> data 2 .... -> LRU
// LRU is at the end of array
//
uint64_t blockOffsetBits;

uint64_t icacheSize; // I$ size
uint32_t iIndexBits;
uint32_t iTagBits;
map<uint32_t, vector<uint32_t>> icache;

uint64_t dcacheSize; // D$ size
uint32_t dIndexBits;
uint32_t dTagBits;
map<uint32_t, vector<uint32_t>> dcache;

uint64_t l2cacheSize; // L2$ size
uint32_t l2IndexBits;
uint32_t l2TagBits;
map<uint32_t, vector<uint32_t>> l2cache;

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

//------------------------------------//
//      Cache getter Functions        //
//------------------------------------//

uint32_t
igetIndex(uint32_t addr)
{
  uint32_t mask = (1 << iIndexBits) - 1;
  uint32_t ans = (addr >> blockOffsetBits) & mask;
  return ans << blockOffsetBits;
}

uint32_t
igetTag(uint32_t addr)
{
  uint32_t ans = addr >> (iIndexBits + blockOffsetBits);
  return ans << (iIndexBits + blockOffsetBits);
}

uint32_t
dgetIndex(uint32_t addr)
{
  uint32_t mask = (1 << dIndexBits) - 1;
  uint32_t ans = (addr >> blockOffsetBits) & mask;
  return ans << blockOffsetBits;
}

uint32_t
dgetTag(uint32_t addr)
{
  uint32_t ans = addr >> (dIndexBits + blockOffsetBits);
  return ans << (dIndexBits + blockOffsetBits);
}

uint32_t
l2getIndex(uint32_t addr)
{
  uint32_t mask = (1 << l2IndexBits) - 1;
  uint32_t ans = (addr >> blockOffsetBits) & mask;
  return ans << blockOffsetBits;
}

uint32_t
l2getTag(uint32_t addr)
{
  uint32_t ans = addr >> (l2IndexBits + blockOffsetBits);
  return ans << (l2IndexBits + blockOffsetBits);
}

//------------------------------------//
//      Cache Helper Functions        //
//------------------------------------//

uint32_t
icacheGet(uint32_t addr)
{
  uint32_t tag = igetTag(addr);
  uint32_t index = igetIndex(addr);

  vector<uint32_t> &block = icache[index];
  int target = 0;
  for (target = 0; target < block.size(); target++) 
  {
    if (block[target] == tag)
    {
      break;
    }
  }

  return target;
}

uint32_t
dcacheGet(uint32_t addr)
{
  uint32_t tag = dgetTag(addr);
  uint32_t index = dgetIndex(addr);

  vector<uint32_t> &block = dcache[index];
  int target = 0;
  for (target = 0; target < block.size(); target++) 
  {
    if (block[target] == tag)
    {
      break;
    }
  }

  return target;
}

uint32_t
l2cacheGet(uint32_t addr)
{
  uint32_t tag = l2getTag(addr);
  uint32_t index = l2getIndex(addr);

  vector<uint32_t> &block = l2cache[index];
  int target = 0;
  for (target = 0; target < block.size(); target++) {
    if (block[target] == tag)
      break;
  }

  return target;
}

// Update l1 cache to ensure the inclusive property
void 
icacheInvalidate(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  uint32_t tag = igetTag(addr);
  uint32_t target = 0;

  if ((target = icacheGet(addr)) < icache[index].size()) {
    vector<uint32_t> &block = icache[index];
    block.erase(block.begin() + target);
  }
}

void 
dcacheInvalidate(uint32_t addr)
{
  uint32_t index = dgetIndex(addr);
  uint32_t tag = dgetTag(addr);
  uint32_t target = 0;

  if ((target = dcacheGet(addr)) < dcache[index].size()) {
    vector<uint32_t> &block = dcache[index];
    block.erase(block.begin() + target);
  }
}

void
icacheUpdate(uint32_t addr, uint32_t target)
{
  uint32_t index = igetIndex(addr);
  uint32_t tag = igetTag(addr);
  vector<uint32_t> &block = icache[index];
  block.erase(block.begin() + target);
  block.push_back(tag);
}

void
dcacheUpdate(uint32_t addr, uint32_t target)
{
  uint32_t index = dgetIndex(addr);
  uint32_t tag = dgetTag(addr);
  vector<uint32_t> &block = dcache[index];
  block.erase(block.begin() + target);
  block.push_back(tag);
}

void
l2cacheUpdate(uint32_t addr, uint32_t target)
{
  uint32_t index = l2getIndex(addr);
  uint32_t tag = l2getTag(addr);
  vector<uint32_t> &block = l2cache[index];
  block.erase(block.begin() + target);
  block.push_back(tag);
}

void
icacheAddData(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  uint32_t tag = igetTag(addr);
  vector<uint32_t> &block = icache[index];
  block.push_back(tag);
  if (block.size() > icacheAssoc) 
  {
    block.erase(block.begin());
  }
}

void
dcacheAddData(uint32_t addr)
{
  uint32_t index = dgetIndex(addr);
  uint32_t tag = dgetTag(addr);
  vector<uint32_t> &block = dcache[index];
  block.push_back(tag);
  if (block.size() > dcacheAssoc) 
  {
    block.erase(block.begin());
  }
}

void
l2cacheAddData(uint32_t addr)
{
  uint32_t index = l2getIndex(addr);
  uint32_t tag = l2getTag(addr);
  vector<uint32_t> &block = l2cache[index];
  block.push_back(tag);
  if (block.size() > l2cacheAssoc) 
  {
    if(inclusive)
    {
      uint32_t reconstruct = block[0] | index;
      icacheInvalidate(reconstruct);
      dcacheInvalidate(reconstruct);
    }
    block.erase(block.begin());
  }
}

//------------------------------------//
//          Cache Init Functions      //
//------------------------------------//

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

  blockOffsetBits = log2(blocksize);

  l2IndexBits = log2(l2cacheSets);
  l2TagBits = ADDRESS_BITS - blockOffsetBits - l2IndexBits;

  dIndexBits = log2(dcacheSets);
  dTagBits = ADDRESS_BITS - blockOffsetBits - dIndexBits;

  iIndexBits = log2(icacheSets); 
  iTagBits = ADDRESS_BITS - blockOffsetBits - iIndexBits;

}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  if (icacheSets == 0)
  {
    return l2cache_access(addr);
  }
  icacheRefs++;
  uint32_t target = 0;
  uint32_t index = igetIndex(addr);
  if ((target = icacheGet(addr)) < icache[index].size()) {
    icacheUpdate(addr, target);
    return icacheHitTime;
  }

  // if tag is not found in I$
  icacheMisses++;
  uint32_t penalties = l2cache_access(addr);
  icacheAddData(addr);
  icachePenalties += penalties;
  return icacheHitTime + penalties;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  if (dcacheSets == 0)
  {
    return l2cache_access(addr);
  }
  dcacheRefs++;
  uint32_t target = 0;
  uint32_t index = dgetIndex(addr);
  if ((target = dcacheGet(addr)) < dcache[index].size()) {
    dcacheUpdate(addr, target);
    return dcacheHitTime;
  }

  // if tag is not found in D$
  dcacheMisses++;
  uint32_t penalities = l2cache_access(addr);
  dcacheAddData(addr);
  dcachePenalties += penalities;
  return dcacheHitTime + penalities;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  l2cacheRefs++;
  uint32_t target = 0;
  uint32_t index = l2getIndex(addr);
  uint32_t tag = l2getTag(addr);
  if ((target = l2cacheGet(addr)) < l2cache[index].size()) {
    l2cacheUpdate(addr, target);
    return l2cacheHitTime;
  }

  // if tag is not found in L2$
  l2cacheMisses++;
  l2cacheAddData(addr);
  l2cachePenalties += memspeed;
  return l2cacheHitTime + memspeed;
}
