
/*******************************************************************************
 *
 *      gcc_hashmap.cpp
 *
 *      @brief
 *
 *      @author   Yukang Chen  @date  2012-03-07 17:19:00
 *
 *      COPYRIGHT (C) 2011, Nextop INC., ALL RIGHTS RESERVED.
 *
 *******************************************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <assert.h>
using namespace std;
int main(){
    int k;
    map<int ,int> my_map;
    for(k=0; k<1000000; k++)
    {
        int val = k+1;
        //printf("insert1 : %d val:%d\n", k, val);
        my_map.insert(make_pair(k, val));
        int ret = my_map[k];
        //assert(my_map[k] == val);

        val = k-1;

        //my_map.insert(make_pair(k, val));
        //assert(my_map[k] == val);
    }
}

