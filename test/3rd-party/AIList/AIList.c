//=============================================================================
//Read .bed datasets, and then find all overlaps 
//by Jianglin Feng  09/05/2018
//Decomposition & simplication: 11/26/2018
//-----------------------------------------------------------------------------
#include "AIList.h"
#include <sys/stat.h>
#include <sys/types.h>
#define PROGRAM_NAME  "ailist"
#define MAJOR_VERSION "0"
#define MINOR_VERSION "1"
#define REVISION_VERSION "1"
#define BUILD_VERSION "0"
#define VERSION MAJOR_VERSION "." MINOR_VERSION "." REVISION_VERSION
//-----------------------------------------------------------------------------
int compare_uint32(const void *a, const void *b) {
    uint32_t pa = *(uint32_t*)a;
    uint32_t pb = *(uint32_t*)b;
    if(pa>pb)
        return 1;
    else if(pa<pb)
        return -1;
    else
        return 0;
}

int compare_rend(const void *a, const void *b)
{
    struct g_data *pa = (struct g_data *) a;
    struct g_data *pb = (struct g_data *) b;
    if(pa->r_end > pb->r_end)
        return 1;
    else if(pa->r_end < pb->r_end)
        return -1;
    else
        return 0;
    //return pa->r_end - pb->r_end;
}

int compare_rstart(const void *a, const void *b)
{
    struct g_data *pa = (struct g_data *) a;
    struct g_data *pb = (struct g_data *) b;
    if(pa->r_start > pb->r_start)
        return 1;
    else if(pa->r_start < pb->r_start)
        return -1;
    else
        return 0;    
    //return pa->r_start - pb->r_start;
}

//assume .end not inclusive:component
int bSearch(struct g_data* As, int idxS, int idxE, uint32_t qe)
{   //find the index of the first .s<qe start from right
    int tL=idxS, tR=idxE-1, tM, tE=-1;
    if(As[tR].r_start < qe)
        return tR;
    else if(As[tL].r_start >= qe)
        return -1;
    while(tL<tR-1){
        tM = (tL+tR)/2; 
        if(As[tM].r_start >= qe)
            tR = tM-1;
        else
            tL = tM;
    }
    if(As[tR].r_start < qe)
        tE = tR;
    else if(As[tL].r_start < qe)
        tE = tL;       
    return tE;   //tE: index of the first item satisfying .s<qe from right
}

void AIListIntersect(char* fQuery, struct g_data** B, int* nB, int cLen)
{   //Decomposite B recursively
    //bed end not inclusive
    clock_t start1, end1, start2, end2;
    start1 = clock();     
    int cLen1=cLen/2, j1;     
    cLen += cLen1;      
    int lenT, len, iter, i, i1, j, k, k0, t, tS, tE; 
    uint32_t tt; 
    int idxC[24][10], lenC[24][10];      
    struct g_data** aiL = malloc(24*sizeof(struct g_data*));  
    struct g_data* aiT; 
    uint32_t** maxE = malloc(24*sizeof(uint32_t*)); 
    int numC[24], maxC=10, minL = MAX(64, cLen);  //max number of components, minL>cLen      
    
    //int maxIter=0;//test 
    
    for(i=0;i<24;i++){
        qsort(B[i], nB[i], sizeof(struct g_data), compare_rstart); 
        
        //for(k=0; k<nB[i]-1; k++){
        //    if(B[i][k+1].r_end < B[i][k].r_end)
        //        dtotal++;
        //}
                       
        aiL[i] = malloc(nB[i]*sizeof(struct g_data));
        maxE[i] = malloc(nB[i]*sizeof(uint32_t));        
        if(nB[i]<=minL){        
            memcpy(aiL[i], B[i], nB[i]*sizeof(struct g_data));
            numC[i]=1;
            lenC[i][0] = nB[i];
            idxC[i][0] = 0;                
        }
        else{   //B[i] will be updated for each cycle
            aiT = malloc(nB[i]*sizeof(struct g_data));    
            iter = 0;
            lenT = nB[i];
            k = 0;
            k0 = 0;
            while(iter<maxC && lenT>minL){   
                len = 0;            
                for(t=0; t<lenT-cLen; t++){
                    tt = B[i][t].r_end;
                    j=1;    j1=1;
                    while(j<cLen && j1<cLen1){
                        if(B[i][j+t].r_end>=tt)  
                            j1++;
                        j++;
                    }
                    if(j1<cLen1){
                        memcpy(&aiT[len], &B[i][t], sizeof(struct g_data));
                        len++;
                    }
                    else{
                        memcpy(&aiL[i][k], &B[i][t], sizeof(struct g_data));
                        k++;
                    }                    
                } 
                memcpy(&aiL[i][k], &B[i][lenT-cLen], cLen*sizeof(struct g_data));   
                k += cLen;
                lenT = len;                
                idxC[i][iter] = k0;
                lenC[i][iter] = k-k0;
                k0 = k;
                if(lenT<=minL || iter==maxC-2){//exit: add aiT to the end
                    iter++;
                    if(lenT>0){
                        memcpy(&aiL[i][k], aiT, lenT*sizeof(struct g_data));
                        idxC[i][iter] = k;
                        lenC[i][iter] = lenT;
                        iter++;
                        numC[i] = iter;
                    }
                    else
                        numC[i]=iter;                   
                }
                else{
                    memcpy(B[i], aiT, lenT*sizeof(struct g_data));
                    iter++;
                }
            }
            free(aiT);  
	    //if(iter>maxIter)
	//	maxIter = iter;     
        }
        if(nB[i]>0)
            free(B[i]);
        //---------------------------------------------------------------------
        //printf("i=%i: %i %i\n", i, nB[i], cLen);
        for(j=0; j<numC[i]; j++){ 
            k0 = idxC[i][j];
            k = k0 + lenC[i][j];
            tt = aiL[i][k0].r_end;
            maxE[i][k0]=tt;
            for(t=k0+1; t<k; t++){
                if(aiL[i][t].r_end > tt)
                    tt = aiL[i][t].r_end;
                maxE[i][t] = tt;  
            }             
        }      
    } 
    
    //printf("The maximum number of subs: %i\n", maxIter);
    
    //-------------------------------------------------------------------------    
    end1 = clock();    
    //printf("Constructing time: %f\n", ((double)(end1-start1))/CLOCKS_PER_SEC);    
    //-------------------------------------------------------------------------
    int bufsize = 1024;   
    int* OlsC = malloc(bufsize*sizeof(int));
    int* OlsI = malloc(bufsize*sizeof(int));
    char buf[1024], s10[128];
    FILE* fd = fopen(fQuery, "r");
    int ichr=-1, lens, t1, t2, ichr0=-1;
    uint32_t qs, qe, rs, re, gs, qhits;
    uint64_t Total=0;
    char *s1, *s2, *s3;   
    strcpy(s10, fQuery);   
    //int tmpi, tmpj;   //test
    while (fgets(buf, 1024, fd)) {
        s1 = strtok(buf, "\t");
        s2 = strtok(NULL, "\t");
        s3 = strtok(NULL, "\t");  
        if(strcmp(s1, s10)==0){
            ichr = ichr0;
        } 
        else{
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
            ichr0 = ichr;
            strcpy(s10, s1); 
        }         
        if(ichr>=0){
            qhits = 0;
            qs = atol(s2);
            qe = atol(s3);   
            //-----------------------------------------------------------------  
            //BSearch takes very little time!!! 2M bSearch(50M)~0.5s!!!     
            for(k=0; k<numC[ichr]; k++){
                rs = idxC[ichr][k];
                re = rs+lenC[ichr][k];
                if(lenC[ichr][k]>15){
                    t = bSearch(aiL[ichr], rs, re, qe); //inline not better 
                    while(t>=rs && maxE[ichr][t]>qs){
                        if(aiL[ichr][t].r_end>qs){
                            if(qhits<bufsize){
                                OlsC[qhits] = ichr;
                                OlsI[qhits] = t;                
                            }
                            qhits++;
                        } 
                        t--;
                    }
                }
                else{
                    for(t=rs; t<re; t++){
                        if(aiL[ichr][t].r_start<qe && aiL[ichr][t].r_end>qs){
                            if(qhits<bufsize){
                                OlsC[qhits] = ichr;
                                OlsI[qhits] = t;                
                            }
                            qhits++;
                        }                
                    }               
                }
            }
            //if(qhits>0)
            printf("%s\t%u\t%u\t%i\n", s1, qs, qe, qhits);                        
            Total += qhits;                                  
        }   
    }    
    fclose(fd);       
    end2 = clock();
    //printf("Searching time: %f\n",((double)(end2-end1))/CLOCKS_PER_SEC);      
    //printf("Total:%lld\n", (long long)Total);
    for(i=0;i<24;i++){
        free(aiL[i]);
        free(maxE[i]);
    }
    free(aiL);
    free(maxE);
    free(OlsC);
    free(OlsI);
}

struct g_data** openBed(char* bFile, int* nD)
{   //open a .bed file and construct g_data  
    char buf[1024], s10[128];
    int i, k, ichr, ichr0, lens;    
    char *s1, *s2, *s3;   
    FILE* fd = fopen(bFile, "r"); 
    strcpy(s10, bFile);      
    //uint64_t avgsize = 0, ntotal = 0;
    while(fgets(buf, 1024, fd)!=NULL){
        s1 = strtok(buf, "\t");
        if(strcmp(s1, s10)==0){
            ichr = ichr0;
        }
        else{          
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
                ichr = atoi(&s1[3])-1;
            }
            strcpy(s10, s1);
            ichr0 = ichr;             
        }    
        if(ichr>=0)
            nD[ichr]++;
    }	
    fseek(fd, 0, SEEK_SET);
    //-------------------------------------------------------------------------   
    struct g_data** gD = malloc(24*sizeof(struct g_data*));
    for(i=0;i<24;i++){
        gD[i] = NULL;
        if(nD[i]>0)
            gD[i] = malloc(nD[i]*sizeof(struct g_data));
            
        //ntotal += nD[i];    
        nD[i]=0;
    }
    while (fgets(buf, 1024, fd)) {
        s1 = strtok(buf, "\t");
        s2 = strtok(NULL, "\t");
        s3 = strtok(NULL, "\t"); 
        if(strcmp(s1, s10)==0){
            ichr = ichr0;
        }
        else{           
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
                ichr = atoi(&s1[3])-1;
            } 
            strcpy(s10, s1);
            ichr0 = ichr;
        }         
        if(ichr>=0){
            k = nD[ichr];
            gD[ichr][k].r_start  = atol(s2);
            gD[ichr][k].r_end  = atol(s3);
            nD[ichr]++;
            //avgsize += atol(s3)-atol(s2);
        } 
    } 
    //printf("total intervals, avg size: %lld, %lld\n", (long long )ntotal, (long long) (avgsize/ntotal));
    fclose(fd);
    return gD;  
} 


int ailist_help(int argc, char **argv, int exit_code);

int main(int argc, char **argv)
{
    int cLen = 20;
    if(argc == 5 && strcmp(argv[3], "-L")==0){
        cLen = atoi(argv[4]);
    }
    else if(argc!=3){
        //fprintf(stderr, "Unknown command\n");
        return ailist_help(argc, argv, 0);//EX_USAGE);
    }

    char *qfName = argv[1];
    char *dfName = argv[2];      
    struct stat st = {0}; 
    char ftmp[128];    

    FILE* fd;  
    fd = fopen(qfName, "r");       
    if(fd==NULL){
        printf("File %s not found!\n", qfName);
        return 0;
    } 
    fclose(fd);
    fd = fopen(dfName, "r");
    if(fd==NULL){
        printf("File %s not found!\n", dfName);
        return 0;
    } 
    fclose(fd);      

    clock_t start, end;
    start = clock();     //chr25: all other minors
    uint32_t* nD24 = calloc(24, sizeof(uint32_t));
    struct g_data** pD24 = openBed(dfName, nD24);
    end = clock();    
    //printf("loading time: %f \n", ((double)(end-start))/CLOCKS_PER_SEC);           
    AIListIntersect(qfName, pD24, nD24, cLen);    //decomposition
    free(pD24);        
    free(nD24);
    return 0;
}

int ailist_help(int argc, char **argv, int exit_code)
{
    fprintf(stderr,"%s, v%s\n" "usage:   %s query-file(.bed) database-file(.bed) [-L coverage-length] \n",
            PROGRAM_NAME, VERSION, PROGRAM_NAME);
    return exit_code;
}

