#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#include "interval_tree.h"
#include "interval_tree_generic.h"

struct g_data
{
    uint32_t start;      			//region start
    uint32_t end;        			//region end
};

unsigned long gstart[25] = {0, 15940, 31520, 44280, 56520, 68190, 79180, 89440, 
        98810, 107670, 116280, 124990, 133570, 140870, 147710, 154220, 160050,
        165420, 170580, 174400, 178550, 181530, 184770, 194650, 198160};

static struct rb_root root = RB_ROOT;

static inline unsigned long
search(unsigned long start, unsigned long end, struct rb_root *root)
{
	struct interval_tree_node *node;
	unsigned long results = 0;

	for (node = interval_tree_iter_first(root, start, end); node;
	     node = interval_tree_iter_next(node, start, end))
		results++;
	return results;
}

/*static void print_nodes(unsigned long start, unsigned long end)
{
	struct interval_tree_node *n = interval_tree_iter_first(&root,
								start, end);

	printf("Tree nodes:");
	while (n) {
		printf(" (%lu, %lu)", n->start, n->last);
		n = interval_tree_iter_next(n, start, end);
	}
	printf("\n");
}*/

struct g_data* openBed(char* bFile, int*nR)
{ 
    char buf[1024];
    FILE* fd = fopen(bFile, "r");
    int nD=0, ichr, lens;
    while(fgets(buf, 1024, fd)!=NULL)	
        nD++;
    fseek(fd, 0, SEEK_SET);
    struct g_data* pD = malloc(nD*sizeof(struct g_data));
    char *s1, *s2, *s3;    
    nD=0;    
    while (fgets(buf, 1024, fd)) {
        s1 = strtok(buf, "\t");
        s2 = strtok(NULL, "\t");
        s3 = strtok(NULL, "\t");   
        lens = strlen(s1);   
        if(lens > 5 || lens < 4)
            ichr = -1;  
        else if(strcmp(s1, "chrX")==0)
            ichr = 22;
        else if(strcmp(s1, "chrY")==0)
            ichr = 23;     
        else if (strcmp(s1, "chrM")==0)
            ichr = -1;    
        else{
            ichr = (int)(atoi(&s1[3])-1);
        }          
        if(ichr>=0){
            pD[nD].start  = (uint32_t)atol(s2) + gstart[ichr];
            pD[nD].end  = (uint32_t)atol(s3) + gstart[ichr]-1;  
            nD++;
        }     
    } 
    fclose(fd);
    *nR = nD;
    return pD;    
}

#define LINE_LEN	1024
int main(int argc, char **argv)
{
    if(argc!=3){
        printf("input: data file, query file \n");
        return 0;		    
    }
    //clock_t start1, end1, end2;
    //start1 = clock();     
	int i, ichr;
	char *qfile = argv[1];
	char *dfile = argv[2];
	char *s1, *s2, *s3;
	FILE *fp;
	char line[LINE_LEN];
    unsigned long start, end;
    for(i=0;i<25;i++){
        gstart[i]*=16384;
    }

	fp = fopen(dfile, "r");
	if (fp == NULL) {
        printf("File %s not found!\n", dfile);
        return 0;
	}

    while (fgets(line, LINE_LEN, fp)) {
        struct interval_tree_node *n;
        n = calloc(1, sizeof(*n));
        s1 = strtok(line, "\t");
        s2 = strtok(NULL, "\t");
        s3 = strtok(NULL, "\t");      
        if(strlen(s1)>5 || strlen(s1)<4 || strcmp(s1, "chrM")==0)
            ichr = -1;  
        else if(strcmp(s1, "chrX")==0)
            ichr = 22;
        else if(strcmp(s1, "chrY")==0)
            ichr = 23;         
        else{
            ichr = (int)(atoi(&s1[3])-1);
        }          
        if(ichr >= 0){         
            n->start  = atol(s2) + gstart[ichr];
            n->last   = atol(s3) + gstart[ichr]-1;         
            interval_tree_insert(n, &root);
        }       
    }  
    fclose(fp);
    //end1 = clock();      
    //print_nodes(0, ULONG_MAX);
    //printf("Tree-building time: %f \n", ((double)(end1-start1))/CLOCKS_PER_SEC);
      
    fp = fopen(qfile, "r");
    if (fp == NULL) {
        printf("File %s not found!\n", qfile);
        return 0;
    }
    unsigned long Total = 0;
    int qhits;
    /*
    int nQ;
    struct g_data* pQ = openBed(qfile, &nQ);    
    for(i=0;i<nQ;i++){
        qhits = 0;
        start = pQ[i].start;			
        end   = pQ[i].end;				
        qhits += search(start, end, &root);  
    }
    free(pQ);
    */
    while (fgets(line, LINE_LEN, fp)) {
        qhits = 0;
        s1 = strtok(line, "\t");
        s2 = strtok(NULL, "\t");
        s3 = strtok(NULL, "\t");
        if(strlen(s1)>5 || strlen(s1)<4 || strcmp(s1, "chrM")==0)
            ichr = -1;  
        else if(strcmp(s1, "chrX")==0)
            ichr = 22;
        else if(strcmp(s1, "chrY")==0)
            ichr = 23;         
        else{
            ichr = (int)(atoi(&s1[3])-1);
        }          
        if(ichr>=0){
            start = atol(s2) + gstart[ichr];			
            end   = atol(s3) + gstart[ichr] -1;//last base not included				
            qhits += search(start, end, &root);            
        }
        printf("%s\t%ld\t%ld\t%i\n", s1, atol(s2), atol(s3), qhits);          
        //printf("%s\t%s\t%s\t%i\n", s1, s2, s3, qhits);
        Total += qhits;
    }
    fclose(fp);
    //end2 = clock();
    //printf("Total: %lld\n", (long long)Total);
    //printf("Searching time: %f \n", ((double)(end2-end1))/CLOCKS_PER_SEC);      
    return 0;
}
