// #ifdef TEST_CHECK
#include "bitmap_v6.h"
#include "bitmap_v4.h"
//#include "fast_lookup.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "time.h"

#include <sys/time.h>
#include <inttypes.h>
#include <x86intrin.h>
#include <x86intrin.h>

#define RAND_SIZE  1000000 

void print_nhi(struct next_hop_info *nhi)
{
    uint64_t key = (uint64_t)nhi;
    printf("%lu", key);
}

int del_routes(struct lookup_trie *trie, FILE *fp)
{
    int i = 0;
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    
    uint32_t ip = 0;
    uint32_t org_ip = 0;
    uint32_t cidr;
    char buf[128];

    uint32_t key = 1;
    rewind(fp);

    while((read = getline(&line, &len, fp)) != -1){
        if (i & 0x01) {

            cidr = atoi(line);
            org_ip = ip;
            ip = ip & (0xffffffff << (32-cidr));
            delete_prefix(trie, ip, cidr, NULL);
            
            struct next_hop_info *a = search(trie, org_ip);
            key = (uint32_t)(uint64_t)a;
            if (prefix_exist(trie, ip, cidr)){
                printf("prefix exist ! error\n");
            }
        }
        else {
            ip = inet_network(line);
            strcpy(buf, line);
        }
        
        i++;

    }
    printf("del routes %d\n", i/2 );
    rewind(fp);    

    return i/2 ;

}



int del_routes_v6(struct lookup_trie_v6 *trie, FILE *fp)
{
    struct ip_v6 ip;
    uint32_t cidr;
    char *slash;
    uint32_t i=1;

    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    int r;
   

    char ip_v6[INET6_ADDRSTRLEN]; 
    char prelen[4];

    struct in6_addr addr;
    
    while((read = getline(&line, &len, fp)) != -1){
        slash = strchr(line, '/');
        if(slash == NULL) {
            printf("wrong format line\n");
            continue;
        }

        memcpy(ip_v6, line, slash - line);
        ip_v6[slash - line] ='\0';

        r = inet_pton(AF_INET6, ip_v6, (void *)&addr);
        if ( r == 0 ){
            printf("wrong format\n");
            continue;
        }
        hton_ipv6(&addr);
        memcpy(&ip, &addr, sizeof(addr));

        memcpy(prelen, slash + 1, strlen(line) - (slash - line) -1);
        prelen[strlen(line) - (slash - line) - 1] ='\0';
        cidr = atoi(prelen);

        struct next_hop_info * ret = search_v6(trie, &ip);
        uint64_t key = (uint64_t)ret;
        if ( key != 0) {
            printf("overlapped or error %s key %d ret %d\n", line, i, key);
        }

        r = prefix_exist_v6(trie, ip, cidr);
        if( r != 1) {
            printf("prefix_exist error\n");
        }
        
        delete_prefix_v6(trie, ip, cidr, NULL); 
        r = prefix_exist_v6(trie, ip, cidr);
        if ( r == 1) {
            printf("prefix_exist error\n");
        }


        i++;
    }

    return 0;    
}

int load_fib(struct lookup_trie *trie, FILE *fp, struct Benchmark_Info* log)
{
    int i = 0;
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    uint32_t ip = 0;
    uint32_t cidr;
    uint64_t key = 1;
    double runtime = 0.0;

    while((read = getline(&line, &len, fp)) != -1){
        uint32_t ip1,ip2,ip3,ip4,port;
        if((sscanf(line, "%d.%d.%d.%d/%d\t%d",&ip1, &ip2, &ip3, &ip4, &cidr, &port)) !=6 ) {
            printf("fib format error\n");
            exit(-1);
        }
        ip = (ip1<<24) + (ip2<<16) + (ip3<<8) + ip4;
        clock_t start_time = clock();
        insert_prefix(trie, ip, cidr, (struct next_hop_info*)key);
        clock_t end_time = clock();
        runtime += (double)(end_time - start_time) / CLOCKS_PER_SEC;
        key++;
        i++;
    }
    log->build_runtime = runtime;
    return i ;
}

int load_routes(struct lookup_trie *trie, FILE *fp)
{
    int i = 0;
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    
    uint32_t ip = 0;
    uint32_t cidr;
    uint64_t key = 1;

    printf("in load routes\n");
    while((read = getline(&line, &len, fp)) != -1){
        if (i & 0x01) {

            cidr = atoi(line);
            ip = ip & (0xffffffff << (32-cidr));
            insert_prefix(trie, ip, cidr,(struct next_hop_info*)(key));
            key ++;
        }
        else {
            ip = inet_network(line);
            printf(", line: %s, ip: %lu\n", line, ip);
        }
        
        i++;
    }
    printf("load routes %d\n", i/2 );
    return i/2 ;
    

}

int load_routes_v6(struct lookup_trie_v6 *trie, FILE *fp)
{
    int i = 0;
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
   
    struct ip_v6 ip;
    uint32_t cidr;
    uint64_t key = 1;
    char *slash;
    

    char ip_v6[INET6_ADDRSTRLEN]; 
    char prelen[4];

    struct in6_addr addr;
    int ret;
    
    while((read = getline(&line, &len, fp)) != -1){
        slash = strchr(line, '/');
        if(slash == NULL) {
            printf("wrong format line\n");
            continue;
        }

        memcpy(ip_v6, line, slash - line);
        ip_v6[slash - line] ='\0';

        ret = inet_pton(AF_INET6, ip_v6, (void *)&addr);
        if (ret == 0) {
            printf("transform fail\n");
            continue;
        }
        hton_ipv6(&addr);
        memcpy(&ip, &addr, sizeof(addr));

        memcpy(prelen, slash + 1, strlen(line) - (slash - line) -1);
        prelen[strlen(line) - (slash - line) - 1] ='\0';
        cidr = atoi(prelen);
        
        insert_prefix_v6(trie, ip, cidr, (struct next_hop_info *)key);
        key ++;
        i++;
    }
    printf("load routes %d\n", i);
    return i ;
}
#if 0
void load_one_route(char *ip_str, uint32_t cidr)
{
   uint32_t ip = inet_network(ip_str);
   ip = ip & (0xffffffff << (32-cidr));
   insert_prefix(ip,cidr,(struct next_hop_info*)(1)); 
}

#endif

void test_lookup_valid_v6(struct lookup_trie_v6 *trie, FILE *fp)
{
    struct ip_v6 ip;
    uint32_t i=1;
    
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;

    int r;
    int match = 0;


    struct in6_addr addr;
    
    while((read = getline(&line, &len, fp)) != -1){
        line[strlen(line) - 1] = '\0';
        r = inet_pton(AF_INET6, line, (void *)&addr); // used to determine if the line in the file is an IPv6 address
        r = inet_pton(AF_INET6, line, (void *)&addr); // used to determine if the line in the file is an IPv6 address
        if ( r == 0 ){
            printf("wrong format\n");
            continue;
        }
        hton_ipv6(&addr);
        memcpy(&ip, &addr, sizeof(addr));

        struct next_hop_info * ret = search_v6(trie, &ip);
        uint64_t key = (uint64_t)ret;
        if ( key != i) {
            printf("overlapped or error %s key %d ret %lu\n", line, i, key);
        }
        else {
            match++;
        }
        i++;
    }

    printf("test %d, match %d\n", i -1 , match);
}

void rm_test_v6(FILE *fp, struct lookup_trie_v6 * trie)
{
    struct ip_v6 ip;
    int i=0;
    
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;

    int r;


    struct in6_addr addr;
    struct ip_v6 *set = (struct ip_v6*)calloc(1, 1000000*sizeof(*set));
    struct next_hop_info * nhi = NULL;

    if (set == NULL) {
        printf("error\n");
        return;
    }

    while((read = getline(&line, &len, fp)) != -1){
        line[strlen(line) - 1] = '\0';
        r = inet_pton(AF_INET6, line, (void *)&addr);
        if ( r == 0 ){
            printf("wrong format\n");
            continue;
        }
        hton_ipv6(&addr);
        memcpy(&ip, &addr, sizeof(addr));
        set[i] = ip;
        i++;
    }
    printf("load %d\n", i);


    struct timespec tp_b;
    struct timespec tp_a;
    i--;

    clock_gettime(CLOCK_MONOTONIC, &tp_b);

    while (i>=0){
        nhi = search_v6(trie, set + i);
        if (nhi == NULL) {
           printf("error\n");
           printf("%d\n",i);
        }
        i--;
    }
    clock_gettime(CLOCK_MONOTONIC, &tp_a);    

    printf("sec %ld, nano %ld\n", tp_b.tv_sec, tp_b.tv_nsec);
    printf("sec %ld, nano %ld\n", tp_a.tv_sec, tp_a.tv_nsec);
    printf("nano %ld\n", tp_a.tv_nsec - tp_b.tv_nsec);

}

void test_lookup_valid(struct lookup_trie *trie, char * filename, struct Benchmark_Info* log)
{
    FILE *fp = fopen(filename,"r"); //ret_5
    if (fp == NULL)
        exit(-1);

    int i = 0;

    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;

    uint32_t *ips = (uint32_t *)malloc(1000000 * sizeof(uint32_t));
    uint32_t ip;
    uint32_t key = 1;

    int counter = 0;
    while((read = getline(&line, &len, fp)) != -1){

        counter += 1;
        uint32_t ip1,ip2,ip3,ip4,port,cidr;
        if((sscanf(line, "%d.%d.%d.%d/%d\t%d",&ip1, &ip2, &ip3, &ip4, &cidr, &port)) !=6 ) {
            printf("fib format error\n");
            exit(-1);
        }

        uint32_t ip_tmp;
        // if (i & 0x01) {
        // }
        // else {
        ip = (ip1<<24) + (ip2<<16) + (ip3<<8) + ip4;
        ips[i] = ip;
        // }
        i++;
    }
    // int ip_cnt = i/2;
    int ip_cnt = i;

    printf("ip_cnt: %d\n", ip_cnt);
    printf("counter: %d\n", counter);

    struct timespec tp_b;
    struct timespec tp_a;

    //int j;
    //for (j=0;j<10;j++){

    // clock_gettime(CLOCK_MONOTONIC, &tp_a);

    clock_t start_time = clock();
    unsigned long long cycles = 0;
    int cnt=0;
    for (i = 0; i< ip_cnt ;i++){

    	unsigned long long start_cycle = __rdtsc();
        struct next_hop_info *a = search(trie, ips[i]);
    	unsigned long long end_cycle = __rdtsc();
	    cycles += (end_cycle-start_cycle);

        uint32_t b = (uint32_t)(uint64_t)a;
        if ( b == key ) {
            cnt ++;
        }

        key ++;
    }
    clock_t end_time = clock();
    printf("runtime2: %lf\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    log->lookup_runtime = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    log->num_lookups = ip_cnt; 
    log->num_cycles = cycles;

    // clock_gettime(CLOCK_MONOTONIC, &tp_b);
    // double runtime = (tp_b.tv_nsec - tp_a.tv_nsec);
    // printf("runtime: %lf\n", runtime);
    printf("match %d / %d\n", cnt, ip_cnt);
    fclose(fp);
    free(ips);
    //mem_usage(&trie->mm);
}

void ipv6_test(char * filename)
{
    printf("in ipv6 test\n");

    FILE *fp = fopen(filename,"r");
    if (fp == NULL)
        exit(-1);
    
    struct lookup_trie_v6 trie;
    struct mem_stats_v6 mem_trie;
    memset(&trie, 0, sizeof(struct lookup_trie_v6));

    printf("ipv6 test\n");
    init_lookup_trie_v6(&trie);


    load_routes_v6(&trie, fp);
    //print_prefix_v6(&trie, print_nhi);

    FILE *fp2 = fopen(filename, "r"); // ret_5 // fib_rm
    if (fp2 != NULL)
        test_lookup_valid_v6(&trie,fp2);
    fclose(fp2);

    mem_trie = mem_trie_v6(&trie);
    printf("ms.mem %d\n", mem_trie.mem);
    mem_alloc_stat_v6();


    FILE *fp3;
    fp3 = fopen(filename, "r");
    if (fp3 != NULL)
        rm_test_v6(fp3, &trie);
    fclose(fp3);

    //rewind(fp);
    //del_routes_v6(&trie,fp);

    mem_trie = mem_trie_v6(&trie);
    printf("ms.mem %d\n", mem_trie.mem);
    destroy_trie_v6(&trie, NULL);

    //print_prefix(&trie, NULL);

    //mem_op();
    ////load_one_route("1.22.0.0", 23);

    ////load_fib();
    //test_lookup_valid(&trie);

    //FILE *fp1 = fopen("update_list","r");

    //del_routes(&trie, fp);
    //print_prefix(&trie, NULL);
    //mem_op();
    //mem_usage(&(trie.mm));

    printf("\n");

}

void test_random_ips(struct lookup_trie *trie, char * filename)
{
    FILE *fp = fopen(filename,"r"); //ret_5
    if (fp == NULL)
        exit(-1);

    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    int cnt = 0;
    int i = 0;
    uint32_t ip;

    uint32_t *random_ips = (uint32_t *)malloc(RAND_SIZE * sizeof(uint32_t));
    uint32_t *ips = (uint32_t *)malloc(RAND_SIZE * sizeof(uint32_t));

    while((read = getline(&line, &len, fp)) != -1){
        if (i & 0x01) {
            // printf("cidr %d, key %d\n",array[i/2].cidr, array[i/2].key);
        }
        else {
            ip = inet_network(line);
            ips[i/2] = ip;
        }
        i++;
    }
    cnt = i/2;


    for (i=0;i<RAND_SIZE;i++){
        random_ips[i] = ips[random()%cnt];
    }
    free(ips);

    struct timespec tp_b;
    struct timespec tp_a;

    //int j;
    //for (j=0;j<10;j++){

    clock_gettime(CLOCK_MONOTONIC, &tp_b);

    for (i=0;i<RAND_SIZE;i++){
        search(trie, random_ips[i]);
    }
 
    clock_gettime(CLOCK_MONOTONIC, &tp_a);
    long nano = (tp_a.tv_nsec > tp_b.tv_nsec) ? (tp_a.tv_nsec -tp_b.tv_nsec) : (tp_a.tv_nsec - tp_b.tv_nsec + 1000000000ULL);
    printf("sec %ld, nano %ld\n", tp_b.tv_sec, tp_b.tv_nsec);
    printf("sec %ld, nano %ld\n", tp_a.tv_sec, tp_a.tv_nsec);
    printf("nano %ld\n", nano);
    //}
    free(random_ips);
    fclose(fp);

}

void ipv4_test(char* filename, char* filename_out)
{
    FILE *fp = fopen(filename,"r"); //"rrc00(2013080808).txt.port"
    struct Benchmark_Info log;

    //FILE *fp = fopen("ret_5","r");
    if (fp == NULL)
        exit(-1);
    
    struct lookup_trie trie;
    memset(&trie, 0, sizeof(struct lookup_trie));

    clock_t start_time = clock();
    init_lookup_trie(&trie, &log);
    clock_t end_time = clock();
    log.build_runtime = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // load_routes(&trie, fp);
    load_fib(&trie, fp, &log);

    level_memory(&trie);
    test_lookup_valid(&trie, filename, &log);
    //mem_alloc_stat_v6();

    //rewind(fp);
    // del_routes(&trie,fp);

    //print_all_prefix(&trie, print_nhi);
    //test_random_ips(&trie);
    destroy_trie(&trie, NULL);
    //mc_profile(&trie.mm);
    
    printf("\n                       build runtime: %lf\n", log.build_runtime);
    printf("                      lookup runtime: %lf\n", log.lookup_runtime);
    printf("                         num_lookups: %ld\n", (long int)log.num_lookups);
    printf("                    number of cycles: %llu\n", log.num_cycles);
    printf("                   cycles per lookup: %ld\n", log.num_cycles / (long int)log.num_lookups);
    printf("                     number of nodes: %d / %d (%2.1lf\%)\n", log.node_count, log.max_nodes, 100 * (double)log.node_count / (double)log.max_nodes);
    printf("                    number of leaves: %d / %d (%2.1lf\%)\n", log.leaf_count, log.max_leaves, 100 * (double)log.leaf_count / (double)log.max_leaves);
    printf("    memory footprint (based on tree): %d\n", log.memory_footprint);
    printf("                  lookup rate (Mlps): %lf\n", log.num_lookups / (log.lookup_runtime * 1e6));
    printf("                              output: %s\n", filename_out);

    /* Write output to file. */
    if (filename_out == NULL) {
        perror("output filename is NULL\n");
        exit(-1);
    }

    FILE* fp_out = fopen(filename_out, "w");
    if (fp_out == NULL) {
        perror("Error in opening output file.");
        exit(-1);
    }

    fprintf(fp_out, "poptrie\n%ld\n%lf\n%lf\n%lf\n%d\n%d\n%d\n%d\n%d\n%llu\n", log.num_lookups, log.lookup_rate, log.lookup_runtime, log.build_runtime,
        log.node_count, log.leaf_count, log.max_nodes, log.max_leaves, log.memory_footprint, log.num_cycles);
    fclose(fp_out);


    /* Close */
    fclose(fp);
}

void test_one_prefix(char * filename)
{
    FILE *fp = fopen(filename,"r"); // ret_5
    if (fp == NULL)
        exit(-1);

    struct Benchmark_Info* log;
    struct lookup_trie trie;
    memset(&trie, 0, sizeof(struct lookup_trie));

    init_lookup_trie(&trie, &log);

    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;

    read = getline(&line, &len, fp);

    int cidr;
    uint64_t key = 1;
    uint32_t ip = 0;


    cidr = atoi(line);
    ip = ip & (0xffffffff << (32 - cidr));

    insert_prefix(&trie, ip, cidr, (struct next_hop_info*)(key));

    ip = inet_network(line);
    
    struct timespec tp_b;
    struct timespec tp_a;

    clock_gettime(CLOCK_MONOTONIC, &tp_b);
    int i;

    for (i=0;i<RAND_SIZE;i++){
        search(&trie, ip);
    }
 
    clock_gettime(CLOCK_MONOTONIC, &tp_a);

    long nano = (tp_a.tv_nsec > tp_b.tv_nsec) ? (tp_a.tv_nsec -tp_b.tv_nsec) : (tp_a.tv_nsec - tp_b.tv_nsec + 1000000000ULL);
    printf("sec %ld, nano %ld\n", tp_b.tv_sec, tp_b.tv_nsec);
    printf("sec %ld, nano %ld\n", tp_a.tv_sec, tp_a.tv_nsec);
    printf("nano %ld\n", nano);


}

int main(int argc, char **argv)
{
    //fast_table_init();
    if (argc < 3) {
        printf("Usage: %s <v4_filename> <v6_filename>\n", argv[0]);
        printf("We need specific files for these tests being run.\n");
        printf("Take a look at how IPv4 (Line 215) and IPv6 (Line 166) addresses are verified.\n");
        exit(-1);
    }
    char * v4_filename = argv[1];
    char * filename_out = argv[2];
    ipv4_test(v4_filename, filename_out);

    // char * v6_filename = argv[2];
    // ipv6_test(v6_filename);
    //test_one_prefix();

    return 0;
}