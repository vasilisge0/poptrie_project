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

            //if (i ==111433){
            //    printf("here\n");
            //}
            cidr = atoi(line);
            org_ip = ip;
            ip = ip & (0xffffffff << (32-cidr));
            //if (ip == 0x7a99ff00){
            //    printf("here\n");
            //}
            //insert_prefix(ip,cidr,(struct next_hop_info*)(key));
            //if (i == 107081) {
            //    printf("here\n");
            //}
            delete_prefix(trie, ip, cidr, NULL);
            
            struct next_hop_info *a = search(trie, org_ip);
            key = (uint32_t)(uint64_t)a;
            if (key != 0) {
                //printf("line %s, cidr %d key %d, org_ip %x\n", buf, cidr, key,org_ip );
            }
            if (prefix_exist(trie, ip, cidr)){
                printf("prefix exist ! error\n");
            }


            //hash_trie_insert(ip,cidr,(struct next_hop_info*)(key));

            //if (i ==111433){
            //    //printf("here\n");
	    //    break;
            //}
            //key ++;
        }
        else {
            //printf("line %s", line);
            ip = inet_network(line);
            strcpy(buf, line);
        }
        
        i++;
        //if (i == 8179) {
        //    printf("here\n");
        //}

    }
    printf("del routes %d\n", i/2 );
    rewind(fp);
    
    while((read = getline(&line, &len, fp)) != -1){
        if (i & 0x01) {

            //if (i ==111433){
            //    printf("here\n");
            //}
            cidr = atoi(line);
            org_ip = ip;
            ip = ip & (0xffffffff << (32-cidr));
            //if (ip == 0x7a99ff00){
            //    printf("here\n");
            //}
            //insert_prefix(ip,cidr,(struct next_hop_info*)(key));
            //if (org_ip == 0x77c25d00) {
            //    printf("here\n");
            //}
            //delete_prefix(ip, cidr);
            
            struct next_hop_info *a = search(trie, org_ip);
            key = (uint32_t)(uint64_t)a;
            if (key != 0) {
                printf("line %s, cidr %d key %d, org_ip %x\n", buf, cidr, key,org_ip );
            }
            //hash_trie_insert(ip,cidr,(struct next_hop_info*)(key));

            //if (i ==111433){
            //    //printf("here\n");
	    //    break;
            //}
            //key ++;
        }
        else {
            //printf("line %s", line);
            ip = inet_network(line);
            strcpy(buf, line);
        }
        
        i++;
        //if (i == 8179) {
        //    printf("here\n");
        //}

    }

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
            //printf("overlapped or error %s key %d ret %d\n", line, i, key);
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

int load_fib(struct lookup_trie *trie, FILE *fp)
{
    int i = 0;
    char *line = NULL;
    ssize_t read = 0;
    size_t len = 0;
    
    uint32_t ip = 0;
    uint32_t cidr;
    uint64_t key = 1;

    while((read = getline(&line, &len, fp)) != -1){
        uint32_t ip1,ip2,ip3,ip4,port;
        if((sscanf(line, "%d.%d.%d.%d/%d\t%d",&ip1, &ip2, &ip3, &ip4, &cidr, &port)) !=6 ) {
            printf("fib format error\n");
            exit(-1);
        }
        ip = (ip1<<24) + (ip2<<16) + (ip3<<8) + ip4;
        if (key < 10)
        {
            printf("ip: %u \ %u %u %u %u, cidr: %u  ", ip, ip1, ip2, ip3, ip4, cidr);
        }
        insert_prefix(trie, ip, cidr, (struct next_hop_info*)key);
        key++;
        i++;
    }

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

            //if (i ==111433){
            //    printf("here\n");
            //}
            cidr = atoi(line);
            ip = ip & (0xffffffff << (32-cidr));
            //if (key == 4835){
            //    printf("here\n");
            //}
            // printf("->(load-routes) line %s", line);
            insert_prefix(trie, ip, cidr,(struct next_hop_info*)(key));
            //hash_trie_insert(ip,cidr,(struct next_hop_info*)(key));

            //if (i ==111433){
            //    //printf("here\n");
	    //    break;
            //}
            key ++;
        }
        else {
            // printf("(load-routes) line %s", line);
            ip = inet_network(line);
            printf(", line: %s, ip: %lu\n", line, ip);
        }
        
        i++;
        //if (i == 8179) {
        //    printf("here\n");
        //}

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
        //if (nhi == NULL) {
        //    printf("error\n");
        //    printf("%d\n",i);
        //}
        i--;
    }
    clock_gettime(CLOCK_MONOTONIC, &tp_a);    

    printf("sec %ld, nano %ld\n", tp_b.tv_sec, tp_b.tv_nsec);
    printf("sec %ld, nano %ld\n", tp_a.tv_sec, tp_a.tv_nsec);
    printf("nano %ld\n", tp_a.tv_nsec - tp_b.tv_nsec);

}

void test_lookup_valid(struct lookup_trie *trie, char * filename)
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

        if (i & 0x01) {
        }
        else {
            ip = (ip1<<24) + (ip2<<16) + (ip3<<8) + ip4;
            //printf("line %s", line);
            // ip = inet_network(line);
            // ip = inet_network("195.66.224.39");
            // printf("line: %s -> %u\n", line, ip);
            // ips[i/2] = ip;
            ips[i] = ip;
        }
        i++;
    }
    // int ip_cnt = i/2;
    int ip_cnt = i;
    printf("ip_cnt: %d\n", ip_cnt);
    printf("counter: %d\n", counter);



    int cnt=0;
    for (i = 0; i< ip_cnt ;i++){

        //if (ips[i] == 0x7a99ff00) {
        //    printf("here\n");
        //}
        // struct next_hop_info *a = hash_trie_search(ips[i]);
        if (i < 10)
            printf("ips[%d]: %u / %lu\n", i, ips[i], ips[i]);
        struct next_hop_info *a = search(trie, ips[i]);
        uint32_t b = (uint32_t)(uint64_t)a;
        // printf("b: %u, a: %u, key: %u\n", b, a, key);
        // printf("key: %u\n", key);
        // printf("b: %d, key: %d, i: %d, ips[i]: %d\n", b, key, i, (int)ips[i]);
        if (key < 10)
        {
            printf("key: %u, b: %u\n", key, b);
        }
        if ( b == key ) {
            cnt ++;
        }
        else {
            printf("**** b: %d, key: %d\n", b, key);
            //struct in_addr addr;
            //addr.s_addr = htonl(array[i].test_ip);


            // printf("search(0x%x); result %d; i  %d\n", ips[i], b ,i);
            //printf("the truth is ip_test %s  key %d ip %x\n", inet_ntoa(addr),array[i].key*2, array[i].ip);
        }
        //printf("search(0x%x);\n", array[i].ip);

        //printf("search(0x%x); reulst %x\n", array[i].ip, b);
        key ++;
    }

    printf("match %d\n", cnt);
    fclose(fp);
    free(ips);
    //mem_usage(&trie->mm);
}

void ipv6_test(char * filename)
{
    //init
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
            //if (i == 201863){
            //    printf("here\n");
            //}
            
            //printf("cidr %d, key %d\n",array[i/2].cidr, array[i/2].key);
        }
        else {
            ip = inet_network(line);
            ips[i/2] = ip;
        }
        //printf("line %s", line);
        
        i++;
        //if (i == 8179) {
        //    printf("here\n");
        //}
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
        //hash_trie_search(random_ips[i]);
        //compact_search(random_ips[i]);
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

void ipv4_test(char * filename)
{
    FILE *fp = fopen(filename,"r"); //"rrc00(2013080808).txt.port"
    //FILE *fp = fopen("ret_5","r");
    if (fp == NULL)
        exit(-1);
    
    struct lookup_trie trie;
    memset(&trie, 0, sizeof(struct lookup_trie));

    printf("ipv4 test\n");

    init_lookup_trie(&trie);

    // load_routes(&trie, fp);
    load_fib(&trie, fp);

    level_memory(&trie);
    test_lookup_valid(&trie, filename);
    //mem_alloc_stat_v6();

    //rewind(fp);
    // del_routes(&trie,fp);

    //print_all_prefix(&trie, print_nhi);
    //test_random_ips(&trie);
    destroy_trie(&trie, NULL);
    //mc_profile(&trie.mm);
    
    printf("\n");

}

void test_one_prefix(char * filename)
{
    FILE *fp = fopen(filename,"r"); // ret_5
    if (fp == NULL)
        exit(-1);

    struct lookup_trie trie;
    memset(&trie, 0, sizeof(struct lookup_trie));

    init_lookup_trie(&trie);

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
        //hash_trie_search(random_ips[i]);
        //compact_search(random_ips[i]);
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
    printf("test\n");
    printf("argc: %d\n", argc);
    if (argc < 2) {
        printf("Usage: %s <v4_filename>\n", argv[0]);
        // printf("Usage: %s <v4_filename> <v6_filename>\n", argv[0]);
        // exit(-1);
    }
    printf("We need specific files for these tests being run.\n");
    printf("Take a look at how IPv4 (Line 215) and IPv6 (Line 166) addresses are verified.\n");
    char * v4_filename = argv[1];
    // char * v6_filename = argv[2];
    // char * v6_filename = argv[1];
    printf("here\n");

    double runtime = 0.0;
    clock_t start_time;
    clock_t end_time;
    start_time = clock();
    printf("before ipv4\n");
    ipv4_test(v4_filename);
    end_time = clock();
    runtime += (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("runtime: %lf\n", runtime);
    printf("rate: %lf (Mlps)\n", 518230 / (runtime * 1e6));
    // ipv6_test(v6_filename);
    // test_one_prefix(v4_filename);

    return 0;
}

// #endif




