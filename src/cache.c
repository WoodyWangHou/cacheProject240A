//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include "utils.h"

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

//------------------------------------//
//          Cache Init Functions      //
//------------------------------------//
void 
init_icache()
{
  icacheSize = icacheSets * icacheAssoc * blocksize;
  icache = (uint32_t **) malloc(sizeof(uint32_t *) * icacheSets);
  for(int i = 0; i < icacheSets; i++){
    icache[i] = (uint32_t *) malloc(sizeof(uint32_t) * icacheAssoc);
    icache[i] = INVALID;
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
    dcache[i] = INVALID;
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
    l2cache[i] = INVALID;
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

  blockOffsetBits = log2(blocksize);

  // Corner case: when sets , assoc and block = 0 -> route I and D to l2
  init_icache();
  init_dcache();
  init_l2cache();

}

//------------------------------------//
//      Cache Set Search Functions    //
//------------------------------------//

uint32_t 
icacheFind(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  uint32_t tags = igetTag(addr);
  for(int i = 0; i < icacheAssoc; i++)
  {
    if(*(icache[index][i]) == tags)
    {
      return TRUE;
    }
  }

  return FALSE;
}

uint32_t 
dcacheFind(uint32_t addr)
{
  uint32_t index = dgetIndex(addr);
  uint32_t tags = dgetTag(addr);
  for(int i = 0; i < icacheAssoc; i++)
  {
    if(*(dcache[index][i]) == tags)
    {
      return TRUE;
    }
  }

  return FALSE;
}

uint32_t 
l2cacheFind(uint32_t addr)
{
  uint32_t index = l2getIndex(addr);
  uint32_t tags = l2getTag(addr);
  for(int i = 0; i < icacheAssoc; i++)
  {
    if(*(l2cache[index][i]) == tags)
    {
      return TRUE;
    }
  }

  return FALSE;
}

// TODO: update statistics collection
//------------------------------------//
//      Cache getter Functions        //
//------------------------------------//

uint32_t
igetIndex(uint32_t addr)
{
  uint32_t mask = 1 << (iIndexBits + 1) - 1;
  return (addr >> blockOffsetBits) & mask;
}

uint32_t
igetTag(uint32_t addr)
{
  return addr >> (iIndexBits + blockOffsetBits);
}

uint32_t
dgetIndex(uint32_t addr)
{
  uint32_t mask = 1 << (dIndexBits + 1) - 1;
  return (addr >> blockOffsetBits) & mask;
}

uint32_t
dgetTag(uint32_t addr)
{
  return addr >> (dIndexBits + blockOffsetBits);
}

uint32_t
l2getIndex(uint32_t addr)
{
  uint32_t mask = 1 << (l2IndexBits + 1) - 1;
  return (addr >> blockOffsetBits) & mask;
}

uint32_t
l2getTag(uint32_t addr)
{
  return addr >> (l2IndexBits + blockOffsetBits);
}

//------------------------------------//
//      Cache load data Functions     //
//------------------------------------//

// Common function base templates
uint32_t 
_isSetFull(uint32_t **cache, uint32_t index, uint32_t cacheAssoc)
{
  for(int i = 0; i < cacheAssoc; i++){
    if(cache[index][i] == INVALID)
    {
      return FALSE;
    }
  }

  return TRUE;
}

void
_cacheRemoveLRU(uint32_t **cache, uint32_t index, uint32_t cacheAssoc)
{
  if(cacheAssoc > 0)
  {
    uint32_t lru = 0;
    while(lru + 1 < cacheAssoc && icache[index][lru + 1] != INVALID)
    {
      lru++;
    }
    cache[index][lru] = INVALID;
  }
}

void
_cacheAddData(uint32_t **cache, uint32_t index, uint32_t tag, uint32_t cacheAssoc)
{
  // shift original data right by 1
  uint32_t cur = 0;
  uint32_t *prev = INVALID;
  while(cur < cacheAssoc - 1 && icache[index][cur] != INVALID)
  {
    uint32_t *temp = cache[index][cur];
    cache[index][cur] = prev; 
    prev = temp;
    cur++;
  }
  cache[index][cur] = prev;

  // add new data at MRU
  if(cacheAssoc > 0){
    cache[index][0] = tag;
  }
}

//
// Wrapper for I$ 
//

uint32_t
i_isSetFull(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  return _isSetFull(icache, index, icacheAssoc);
}

void
icacheRemoveLRU(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  _cacheRemoveLRU(icache, index, icacheAssoc);
}

void 
icacheAddData(uint32_t addr)
{
  // assume there is always empty slot in set
  uint32_t index = igetIndex(addr);
  uint32_t tag = igetTag(addr);
  _cacheAddData(icache, index, tag, icacheAssoc);
}

//
// Wrapper for D$ 
//

uint32_t
d_isSetFull(uint32_t addr)
{
  uint32_t index = dgetIndex(addr);
  return _isSetFull(dcache, index, dcacheAssoc);
}

void
dcacheRemoveLRU(uint32_t addr)
{
  uint32_t index = dgetIndex(addr);
  _cacheRemoveLRU(dcache, index, dcacheAssoc);
}

void 
dcacheAddData(uint32_t addr)
{
  // assume there is always empty slot in set
  uint32_t index = dgetIndex(addr);
  uint32_t tag = dgetTag(addr);
  _cacheAddData(dcache, index, tag, dcacheAssoc);
}

//
// Wrapper for L2$ 
// L2$ differs from I$ and D$ in terms of Inclusive Policy
// TODO: Implement L2$ Inclusive Policy
//

uint32_t
l2_isSetFull(uint32_t addr)
{
  uint32_t index = igetIndex(addr);
  return _isSetFull(l2cache, index, l2cacheAssoc);
}

void
l2cacheRemoveLRU(uint32_t addr)
{
  uint32_t index = l2getIndex(addr);
  _cacheRemoveLRU(l2cache, index, l2cacheAssoc);
}

void 
l2cacheAddData(uint32_t addr)
{
  // assume there is always empty slot in set
  uint32_t index = l2getIndex(addr);
  uint32_t tag = l2getTag(addr);
  _cacheAddData(l2cache, index, tag, l2cacheAssoc);
}

// internal helpers used by cache_access functions

void 
icacheLoad(uint32_t addr)
{
    if(i_isSetFull(addr) == TRUE)
    {
      icacheRemoveLRU(addr);
    }
    icacheAddData(addr);
}

void 
dcacheLoad(uint32_t addr)
{
    if(d_isSetFull(addr) == TRUE)
    {
      dcacheRemoveLRU(addr);
    }
    dcacheAddData(addr);
}

void 
l2cacheLoad(uint32_t addr)
{
    if(l2_isSetFull(addr) == TRUE)
    {
      l2cacheRemoveLRU(addr);
    }
    l2cacheAddData(addr);
}

//------------------------------------//
//      Cache update LRU Functions    //
//------------------------------------//
// TODO: Implement update LRU functions


// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{
  if(icacheFind(addr) == TRUE){
    icacheUpdate(addr);
    return icacheHitTime;
  }

  // if not found
  uint32_t penalities = icachePenalties + l2cache_access(addr);
  icacheLoad(addr);
  return penalities;
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  if(dcacheFind(addr) == TRUE){
    dcacheUpdate(addr);
    return dcacheHitTime;
  }

  uint32_t penalities = dcachePenalties + l2cache_access(addr);
  dcacheLoad(addr);
  return penalities;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  if(l2cacheFind(addr) == TRUE){
    l2cacheUpdate(addr);
    return l2cacheHitTime;
  }

  // not found in l2 cache
  // handle inclusive in loading, if inclusive -> invalidate l1 as well / if not do nothing
  l2cacheLoad(addr);
  return l2cachePenalties + memspeed;
}
