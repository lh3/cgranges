//=====================================================================================
//Common structs, parameters, functions
//by Jianglin Feng  09/5/2018
//-------------------------------------------------------------------------------------
#ifndef __AILIST_H__
#define __AILIST_H__
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <glob.h>
#include <errno.h>
#include <sysexits.h>
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

extern uint64_t maxCount;
extern uint32_t *g2ichr;

//-------------------------------------------------------------------------------------
struct g_data
{
    uint32_t r_start;      			//region start: 0-based
    uint32_t r_end;        			//region end: not included
};

//-------------------------------------------------------------------------------------
int compare_uint32(const void *a, const void *b);
int compare_rend(const void *a, const void *b);
int compare_rstart(const void *a, const void *b);
struct g_data** openBed(char* bFile, int* nD);

//assume .end not inclusive:component
int bSearch(struct g_data* As, int idxS, int idxE, uint32_t qe);
void AIListIntersect(char* fQuery, struct g_data** B, int* nB, int cLen);
#endif
