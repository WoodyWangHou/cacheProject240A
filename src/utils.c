//========================================================//
//  utils.c                                               //
//  Implementation for the Utility                        //
//                                                        //
//  Include common functions required for binary ops      //
//========================================================//

#include "utils.h"

uint64_t 
log2(uint64_t num)
{
    if(num == 0)
    {
        // should be -INFINITY, but here return 0 to indicate
        return 0;
    }

    if(num == 1)
    {
        return 0;
    }

    uint64_t ans = 0;
    while(num > 1){
        num /= 2;
        ans++;
    }

    return ans;
}