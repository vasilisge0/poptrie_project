#include "Fib.h"
#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <math.h>
#include <string.h>
#include <time.h>
//#include <conio.h>
#include <chrono>
#include <x86intrin.h>
#include <chrono>
#include <x86intrin.h>

#define IP_LEN		32

char * ribFile					= "rib.txt";				//original Rib file
char * updateFile				= "updates.txt"; 			//update file in IP format

char * oldPortfile1				= "oldport1.txt";
char * oldPortfile_bin1			= "oldport_bin1.txt";
char * oldPortfile2				= "oldport2.txt";
char * oldPortfile_bin2			= "oldport_bin2.txt";
char * oldPortfile3				= "oldport3.txt";
char * oldPortfile_bin3			= "oldport_bin3.txt";

char * newPortfile1				= "newport1.txt";
char * newPortfile_bin1			= "newport_bin1.txt";
char * newPortfile2				= "newport2.txt";
char * newPortfile_bin2			= "newport_bin2.txt";
char * newPortfile3				= "newport3.txt";
char * newPortfile_bin3			= "newport_bin3.txt";

char * trace_path				= "trace(100000).integer";
char * ribfileName				= "rib.txt.port";
char ret[IP_LEN+1];

#define UpdateFileCount		6
#define UPDATE_ALG	_MINI_REDUANDANCY_TWOTRAS

struct Benchmark_Info
{
    double build_runtime;
    double runtime;
    long long unsigned num_lookups;
    long long unsigned num_cycles;
    int node_count;
    int max_nodes;
    int leaf_count;
    int max_leaves;
    int memory_usage;
    double lookup_rate;
    double lookup_runtime;
};

int FindFibTrieNextHop(FibTrie * m_trie, char * insert_C)
{
	int nextHop = -1;//init the return value
	FibTrie *insertNode = m_trie;

	if (insertNode->oldPort != 0) {
		nextHop = insertNode->oldPort;
	}

	int len=(int) strlen(insert_C);

	for (int i=0; i < (len + 1);i++)
	{		
		if ('0' == insert_C[i])
		{//if 0, turn left
			if (NULL != insertNode->lchild)	
			{
				insertNode = insertNode->lchild;
			}
			else {
				break;
			}
		}
		else
		{//if 1, turn right
			if (NULL != insertNode->rchild) {
				insertNode = insertNode->rchild;
			}
			else {
				break;
			}
		}

		if (insertNode->newPort != 0)	{
			nextHop = insertNode->newPort;
		}
	}

	return	nextHop;
}



//given a ip in binary---str and its length---len, return the next ip in binary
char * GetStringIP(char *str, int len)
{
	memset(ret,0,sizeof(ret));
	memcpy(ret,str,IP_LEN);
	int i;
	for (i=0;i<len;i++)
	{
		if ('0'==ret[i])
		{
			ret[i]='1';
			break;
		}
		else if ('1'==ret[i])
		{
			ret[i]='0';
		}
	}
	//printf("%s*\n",ret);
	return ret;
}

unsigned int btod(char *bstr)
{
	unsigned int d = 0;
	unsigned int len = (unsigned int)strlen(bstr);
	if (len > 32)
	{
		printf("too long\n");
		return -1; 
	}
	len--;
	for (unsigned int i = 0; i <= len; i++)
	{
		d += (bstr[i] - '0') * (1 << (len - i));
	}
	return d;
}

void sailDetectForFullIp(CFib *tFib) {
	printf("in sailDetectForFullIp\n");
	int nonRouteStatic=0;

	int hop1=0;
	int hop2=0;

	char strIP00[IP_LEN + 1];
	memset(strIP00, 0, sizeof(strIP00));
	
	for (int tmp=0; tmp < IP_LEN; tmp++)
	{
		strIP00[tmp]='0';
	}

	int len88 = (int)strlen(strIP00);

	char new_tmp[IP_LEN + 1];
	char old_tmp[IP_LEN + 1];

	memset(new_tmp, 0, sizeof(new_tmp));
	memset(new_tmp, 0, sizeof(new_tmp));
	memcpy(new_tmp, strIP00, IP_LEN);

	double zhishuI = pow((double)2,(double)IP_LEN);

	bool ifhalved = false;
	printf("\t\ttotal\t%.0f\t\n", zhishuI);
	printf("\t\tlength\tcycles\t\tpercent\tnexthop\n");

	printf("zhishuI: %lf\n", zhishuI);
	for (long long k=0; k < zhishuI; k++)
	{
		memcpy(old_tmp, new_tmp, IP_LEN);
		memcpy(new_tmp, GetStringIP(old_tmp, IP_LEN), IP_LEN);
		
		hop1 = FindFibTrieNextHop(tFib->m_pTrie, new_tmp);
	
		unsigned int IPInteger = btod(new_tmp);
		hop2 = tFib->sailLookup(IPInteger);

		if (hop1== -1 && hop2 != hop1)
		{
			nonRouteStatic++;
			continue;
		}

		double ratio=0;
		
		if (hop2 != hop1)
		{
			printf("%d:%d", hop1, hop2);
			printf("\n\n\t\tNot Equal!!!\n");
			//_getch();
		}
		else 
		{
			if (k%100000 == 0)
			{
				ratio=k/(double)(zhishuI/100);
				printf("\r\t\t%d\t%lld\t%.2f%%\t%d             ", IP_LEN, k, ratio, hop1);
			}
		}
	}

	printf("\n\t\tTotal number of garbage roaming route��%d",nonRouteStatic);
	printf("\n\t\tEqual!!!!\n");
}

void help () {
	printf ("#######################################\n");
	printf ("##  *-*-*-*-OH algorithm-*-*-*-*-*   ##\n");
	printf ("#   {para} = [trace name] [rib name]  #\n");
	printf ("##       trace_path   ribfileName    ##\n");
	printf ("#######################################\n");
	system ("pause");
}

unsigned int depth(FibTrie* root){
	if(root == NULL){
		return 0;
	}
	return max(depth(root->lchild), depth(root->rchild))+1;
}

// Levelpushing Trie Update
unsigned int BFLevelPushingTrieUpdate(string sFileName,CFib *tFib)
{
	unsigned int		iEntryCount = 0;					//the number of items from file
	char				sPrefix[20];						//prefix from rib file
	unsigned long		lPrefix;							//the value of Prefix
	unsigned int		iPrefixLen;							//the length of PREFIX
	int					iNextHop;							//to store NEXTHOP in RIB file

	char				operate_type_read;
	int 				operate_type;
	int					readlines = 0;
	unsigned long long	updatetimeused = 0;
	unsigned long long	rdtsc_time = 0;

	long				yearmonthday=0;						//an integer to record year, month, day
	long				hourminsec=0;						//an integer to record hour, minute, second
	long				yearmonthday_old=0;					//an integer to record year, month, day
	long				hourminsec_old=0;					//an integer to record hour, minute, second
	
	long				outputCount=0;
	long				insertNum_old=0;
	long				DelNum_old=0;
	long				readlines_old=0;

	//int64_t frequence,privious,privious1;
	//if(!QueryPerformanceFrequency(&frequence)) return 0;
	//auto frequence = std::chrono::high_resolution_clock::now();

	//for (int jjj = 1; jjj <= UpdateFileCount; jjj++)
	for (int jjj = 1; jjj <= 1; jjj++)
	{
		char strName[20];
		memset(strName, 0, sizeof(strName));
		//sprintf(strName, "updates%d.txt", jjj);
		sprintf(strName, sFileName.c_str(), jjj);

		ifstream fin(strName);
		if (!fin)
		{
			//printf("!!!error!!!!  no file named:%s\n",strName);
			continue;
		}

		printf("\n    Parsing %s\n", strName);

		while (!fin.eof() ) {			// Add by Qiaobin Fu 2014-4-22
		
			lPrefix = 0;
			iPrefixLen = 0;
			iNextHop = -9;

			memset(sPrefix,0,sizeof(sPrefix));
			
			//read data from rib file, iNextHop attention !!!
			fin >> yearmonthday >> hourminsec >> operate_type_read >> sPrefix;	//>> iNextHop;

			if('W' == operate_type_read) {
				operate_type = _DELETE;
			}
			else if ('A' == operate_type_read)
			{
				fin >> iNextHop;
				operate_type = _NOT_DELETE;
			}
			else
			{
				printf("\tFormat of update file Error, quit....\n");
				//getchar();
				return 0;
			}

			int iStart = 0;								//the end point of IP
			int iEnd = 0;								//the end point of IP
			int iFieldIndex = 3;		
			int iLen = (int)strlen(sPrefix);			//the length of Prefix
		
			if(iLen > 0)
			{
				readlines++;

				for ( int i=0; i<iLen; i++ )
				{
					//extract the first 3 sub-part
					if ( sPrefix[i] == '.' )
					{
						iEnd = i;
						string strVal(sPrefix + iStart, iEnd - iStart);
						lPrefix += atol(strVal.c_str()) << (8 * iFieldIndex); 
						iFieldIndex--;
						iStart = i + 1;
						i++;
					}

					if ( sPrefix[i] == '/' ) {
						//extract the 4th sub-part
						iEnd = i;
						string strVal(sPrefix + iStart, iEnd - iStart);
						lPrefix += atol(strVal.c_str());
						iStart = i + 1;

						//extract the length of prefix
						i++;
						strVal = string(sPrefix + iStart, iLen - 1);
						iPrefixLen = atoi(strVal.c_str());
					}
				}

				char insert_C[50];
				memset(insert_C,0,sizeof(insert_C));
				//insert the current node into Trie tree
				for (unsigned int yi = 0; yi < iPrefixLen; yi++)
				{
					//turn right
					if(((lPrefix << yi) & HIGHTBIT) == HIGHTBIT) insert_C[yi]='1';
					else insert_C[yi]='0';
				}
				//printf("%s\/%d\t%d\n",insert_C,iPrefixLen,iNextHop);

				if (iPrefixLen < 8) {
					//printf("%d-%d; ", iPrefixLen, iNextHop);
				}
				else
				{
					//QueryPerformanceCounter(&privious); 
					auto privious = std::chrono::high_resolution_clock::now();
					unsigned long long start = __rdtsc();
					//printf("\t\t%d-%d; ", iPrefixLen, iNextHop);
					tFib->Update(iNextHop, insert_C, operate_type, sPrefix);

					// QueryPerformanceCounter(&privious1);
					unsigned long long end = __rdtsc();
					auto privious1 = std::chrono::high_resolution_clock::now();
					// updatetimeused += 1000000*(privious1.QuadPart-privious.QuadPart)/frequence.QuadPart;
					updatetimeused += std::chrono::duration_cast<std::chrono::microseconds>(privious1-privious).count();
					rdtsc_time += end-start;
				}
			}
		}
		fin.close();
	}

	printf("\tupdate performance: readline=%u\ttime=%uus\n\tspeed=%.7f Mups\n",readlines,updatetimeused, readlines/(updatetimeused+0.0));
	printf("\tCycles per update (RDTSC): %u\n", rdtsc_time/readlines);
	printf("\tCycles per update (Time): %u\n", updatetimeused/sysconf(_SC_CLK_TCK));
	printf("\tDepth: %u\n", depth(tFib->m_pTrie));
	// printf("\tCycles per update (RDTSC): %u\n", rdtsc_time/readlines);
	// printf("\tCycles per update (Time): %u\n", updatetimeused/sysconf(_SC_CLK_TCK));
	// printf("\tDepth: %u\n", depth(tFib->m_pTrie));
	return readlines;
}

void amination()
{
	//system("color 1D");
	system("mode con cols=75 lines=40 &color 3f");
	int sleeptime=200;
	printf("\t\t    |-*-*-*-*-*-*-     -*-*-*-*-*-*-*-|\n");
	printf("\t\t    |-*-*-*-                 -*-*-*-*-|\n");
	printf("\t\t    |          S                      |");
	sleep(sleeptime);
	printf("\r\t\t    |          SA                     |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAI                    |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL                  |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL A                |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Al               |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Alg              |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Algo             |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Algor            |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Algori           |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Algorit          |");
	sleep(sleeptime);
	printf("\r\t\t    |          SAIL Algoritm         |\n");


	printf("\t\t    |by Tong Yang & Qiaobin Fu        |");
	sleep(sleeptime);
	printf("\r\t\t    |by Tong Yang & Qiaobin Fu        |");
	sleep(sleeptime);
	printf("\r\t\t    | by Tong Yang & Qiaobin Fu       |");
	sleep(sleeptime);
	printf("\r\t\t    |   by Tong Yang & Qiaobin Fu     |");
	sleep(sleeptime);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |");

	sleep(sleeptime/2);
	printf("\r\t\t    |                                 |");
	sleep(sleeptime/2);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |");
	sleep(sleeptime/2);
	printf("\r\t\t    |                                 |");
	sleep(sleeptime/2);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |");
	sleep(sleeptime/2);
	printf("\r\t\t    |                                 |");
	sleep(sleeptime/2);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |");
	sleep(sleeptime/2);
	printf("\r\t\t    |                                 |");
	sleep(sleeptime/2);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |");
	sleep(sleeptime/2);
	printf("\r\t\t    |                                 |");
	sleep(sleeptime/2);
	printf("\r\t\t    |    by Tong Yang & Qiaobin Fu    |\n");
	sleep(sleeptime/2);
	printf("\r\t\t    |  Directed by Prof. Gaogang Xie  |\n");
	sleep(sleeptime);
	printf("\r\t\t    |         In Sigcomm 2014         |\n");
	sleep(sleeptime);
	printf("\r\t\t    |          ICT,CAS,China          |\n");
	sleep(sleeptime);
	printf("\t\t    |-*-*-*-                 -*-*-*-*-|\n");
	sleep(sleeptime);
	printf("\t\t    |-*-*-*-*-*-*-     -*-*-*-*-*-*-*-|\n");
}
void sailPerformanceTest(char *traffic_file, char* fib_file)
{
	printf("\t\t\t********************************************\n");
	printf("\t\t\t*-*-*         sail algorithm        *-*-*-*\n");
	printf("\t\t\t*-*-*            ICT, CAS            *-*-*-*\n");
	printf("\t\t\t********************************************\n");

	//amination();

	printf("\n\nsail algorithm starts...\n\n");
	CFib tFib = CFib();
	tFib.BuildFibFromFile(fib_file);
	// unsigned int *traffic=tFib.TrafficRead(traffic_file);

	register unsigned char LPMPort=0;

	struct timespec frequence,privious,privious1;
	// if(!QueryPerformanceFrequency(&frequence))return;
	// QueryPerformanceCounter(&privious);
	// printf("\tfrequency=%u\n",frequence.QuadPart);//2825683


	unsigned long long start = __rdtsc();
	auto t0 = std::chrono::high_resolution_clock::now();
	printf("TRACE_READ: %d", TRACE_READ);
	for (register int j=0;j<30000;j++)
	{
		// for (register int i=0;i<TRACE_READ;i++)
		// {
			// LPMPort=tFib.sailLookup(traffic[i]);
		// }
		LPMPort=tFib.sailLookup(j);
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	double runtime = (double)std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
	unsigned long long end = __rdtsc();
	unsigned long long rdtsc_time = end - start;


	printf("	runtime: %lf\n", runtime);
 	printf("	Cycles per update (RDTSC): %llu\n", rdtsc_time);

	// QueryPerformanceCounter(&privious1);
	// long long Lookuptime=1000000*(privious1.QuadPart-privious.QuadPart)/frequence.QuadPart;
	// printf("\tLMPport=%d\n\tLookup time=%u\n\tThroughput is:\t %.3f Mpps\n",LPMPort,Lookuptime, 10000.0*TRACE_READ/Lookuptime);

	// int updateEntryCount = BFLevelPushingTrieUpdate(updateFile, &tFib);
}




void test(int argc, char** argv)
{
	Benchmark_Info log;
	char* filename_out;
	if (argc >= 4) {
		filename_out = argv[3];
	}

	/******************************************Stage 1******************************************/
	printf("\t\tStage One: The Initial Trie Construction\n");
	//build FibTrie
	CFib tFib = CFib();

	tFib.ytGetNodeCounts();
	printf("\nThe total number of Trie node is :\t%u.\n", tFib.allNodeCount);
	printf("The total number of solid Trie node is :\t%d.\n", tFib.solidNodeCount);

	/*tFib.outputPortMapping(portMapFile1);
	tFib.OutputTrie(tFib.m_pTrie, newPortfile1, oldPortfile1);*/
	
	printf("\n***********************Trie Correct Test***********************\n");
	
	if (!tFib.isCorrectTrie(tFib.m_pTrie)) {
		printf("The trie structure is incorrect!!!\n");
	}
	else {
		printf("The trie structure is correct!\n");
	}

	tFib.ytTriePortTest(tFib.m_pTrie);
	printf("******************************End******************************\n");

	//system("pause");

	/******************************************Stage 2******************************************/
	printf("\n\n\t\tStage Two: The First Round Update\n");
	unsigned int iEntryCount = 0;
	unsigned int updateEntryCount = 0;

	//printf("%d\n", tFib.nodes);
	//printf("\tDepth: %u\n", depth(tFib.m_pTrie));
	
	auto one = std::chrono::high_resolution_clock::now();
	iEntryCount = tFib.BuildFibFromFile(argv[2]);
	auto two = std::chrono::high_resolution_clock::now();

	//printf("%d\n", tFib.nodes);
	printf("\tCompilation time: %uus\n", std::chrono::duration_cast<std::chrono::microseconds>(two-one).count());
	log.build_runtime = std::chrono::duration_cast<std::chrono::microseconds>(two-one).count() / 1e6;
	printf("\tDepth: %u\n", depth(tFib.m_pTrie));

	tFib.ytGetNodeCounts();
	printf("\tBytes Used: %u\n", tFib.allNodeCount * sizeof(struct FibTrie));
	log.max_nodes = tFib.allNodeCount;
	log.node_count = tFib.solidNodeCount;
	log.memory_usage = tFib.allNodeCount * sizeof(struct FibTrie);
	
	printf("\nThe total number of Trie node is :\t%u.\n",tFib.allNodeCount);
	printf("The total number of solid Trie node is :\t%d.\n", tFib.solidNodeCount);
	printf("The total number of routing items in FRib file is :\t%u.\n", iEntryCount);

	/*tFib.outputPortMapping(portMapFile2);
	tFib.OutputTrie(tFib.m_pTrie, newPortfile2, oldPortfile2);*/

	printf("\n***********************Trie Correct Test***********************\n");
	
	if (!tFib.isCorrectTrie(tFib.m_pTrie)) {
		printf("The trie structure is incorrect!!!\n");
	}
	else {
		printf("The trie structure is correct!\n");
	}

	tFib.ytTriePortTest(tFib.m_pTrie);
	printf("******************************End******************************\n");

	//tFib.checkTable(tFib.m_pTrie, 0);

	/*printf("\n************************sail Lookup Correct Test************************\n");
	sailDetectForFullIp(&tFib);
	printf("***********************************End***********************************\n");*/

	//system("pause");

	// /******************************************Stage 3******************************************/
	// printf("\n\n\t\tStage Three: The Second Round Update\n");
	// //update FibTrie stage
	// updateEntryCount = BFLevelPushingTrieUpdate(argv[1], &tFib);
	// tFib.ytGetNodeCounts();
	// printf("\nThe total memory access is :\t%llu.\n", tFib.memory_access);
	// printf("The total number of Trie node is :\t%d.\n", tFib.allNodeCount);
	// printf("The total number of solid Trie node is :\t%u.\n", tFib.solidNodeCount);
	// printf("The total number of updated routing items is :\t%u.\n", updateEntryCount);
	
	// /*tFib.outputPortMapping(portMapFile3);
	// tFib.OutputTrie(tFib.m_pTrie, newPortfile3, oldPortfile3);*/

	// printf("\n************************Trie Correct Test************************\n");
	
	// if (!tFib.isCorrectTrie(tFib.m_pTrie)) {
	// 	printf("The trie structure is incorrect!!!\n");
	// }
	// else {
	// 	printf("The trie structure is correct!\n");
	// }

	// tFib.ytTriePortTest(tFib.m_pTrie);
	// printf("*******************************End*******************************\n");

	// printf("\n\n\t\tUpdate Statistics\n");
	// printf("\nThe total number of true update items is :\t%u.\n", tFib.trueUpdateNum);
	// printf("The total number of invalid update items is :\t%u.\n", tFib.invalid);
	// printf("The detailed invalid items:\n\tinvalid0 = %u\tinvalid1 = %u\tinvalid2 = %u\n", tFib.invalid0, tFib.invalid1, tFib.invalid2);

	// // system("pause");
	// tFib.checkTable(tFib.m_pTrie, 0);

	printf("\n\n************************sail Lookup Correct Test************************\n");
	//sailDetectForFullIp(&tFib);
	printf("***********************************End***********************************\n");

	printf("\nMission Complete, Press any key to continue...\n");
	//system("pause");

	printf("\n                       build runtime: %lf\n", log.build_runtime);
    printf("                      lookup runtime: %lf\n", log.runtime);
    printf("                         num_lookups: %ld\n", (long int)log.num_lookups);
    printf("                    number of cycles: %llu\n", log.num_cycles);
    // printf("                 cycles / per lookup: %llu\n", log.num_cycles / log.num_lookups);
    printf("                     number of nodes: %d\n", log.node_count);
    printf("        memory usage (based on tree): %d\n", log.memory_usage);
    // printf("                  lookup rate (Mlps): %lf\n", log.num_lookups / (log.runtime * 1e6));
    printf("                              output: %s\n", filename_out);
}



int main (int argc, char** argv) {

	//amination();

	if (argc<3) {
		printf("Usage: %s [traffic file <updates>] [fib file <rrc01-12.1.1.txt>]\n", argv[0]);
	}
	else if (argc == 4)
	{
		printf("before performance test\n");
		// sailPerformanceTest(argv[1], argv[2]);
		test(argc, argv);
	}
	return 0;
	
}

