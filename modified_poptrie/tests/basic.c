/*_
 * Copyright (c) 2014-2016 Hirochika Asai <asai@jar.jp>
 * All rights reserved.
 */

#include "../poptrie.h"
#include "../buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <x86intrin.h>
#include <x86intrin.h>

#define BUDDY_EOL 0xffffffffUL
#define BENCHMARK_ACTIVE 1

typedef uint32_t u32;

/* Macro for testing */
#define TEST_FUNC(str, func, ret)                \
    do {                                         \
        printf("%s: ", str);                     \
        if ( 0 == func() ) {                     \
            printf("passed");                    \
        } else {                                 \
            printf("failed");                    \
            ret = -1;                            \
        }                                        \
        printf("\n");                            \
    } while ( 0 )

#define TEST_FUNC_REQUIRES_FILENAME(str, func, param, ret)         \
    do {                                         \
        printf("%s: ", str);                     \
        if (0 == func(param)) {                  \
            printf("passed");                    \
        } else {                                 \
            printf("failed");                    \
            ret = -1;                            \
        }                                        \
        printf("\n");                            \
    } while (0)

#define TEST_FUNC_REQUIRES_FILENAME(str, func, param, ret)         \
    do {                                         \
        printf("%s: ", str);                     \
        if (0 == func(param)) {                  \
            printf("passed");                    \
        } else {                                 \
            printf("failed");                    \
            ret = -1;                            \
        }                                        \
        printf("\n");                            \
    } while (0)

#define TEST_PROGRESS()                              \
    do {                                             \
        printf(".");                                 \
        fflush(stdout);                              \
    } while ( 0 )


/*
 * Initialization test
 */
static int
test_init(void)
{
    struct poptrie *poptrie;

    /* Initialize */
    poptrie = poptrie_init(NULL, 19, 22);
    if ( NULL == poptrie ) {
        return -1;
    }

    TEST_PROGRESS();

    /* Release */
    poptrie_release(poptrie);

    return 0;
}

static int
test_lookup(void)
{
    struct poptrie *poptrie;
    int ret;
    void *nexthop;

    /* Initialize */
    poptrie = poptrie_init(NULL, 19, 22);
    if ( NULL == poptrie ) {
        return -1;
    }

    /* No route must be found */
    if ( NULL != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }

    /* Route add */
    nexthop = (void *)1234;
    ret = poptrie_route_add(poptrie, 0x1c001200, 24, nexthop);
    if ( ret < 0 ) {
        /* Failed to add */
        return -1;
    }
    if ( nexthop != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Route update */
    nexthop = (void *)5678;
    ret = poptrie_route_update(poptrie, 0x1c001200, 24, nexthop);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }

    if ( nexthop != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Route delete */
    ret = poptrie_route_del(poptrie, 0x1c001200, 24);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }

    if ( NULL != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Release */
    poptrie_release(poptrie);

    return 0;
}

static int
test_lookup2(void)
{
    struct poptrie *poptrie;
    int ret;
    void *nexthop;

    /* Initialize */
    poptrie = poptrie_init(NULL, 19, 22);
    if ( NULL == poptrie ) {
        return -1;
    }

    /* No route must be found */
    if ( NULL != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }

    /* Route add */
    nexthop = (void *)1234;
    ret = poptrie_route_add(poptrie, 0x1c001203, 32, nexthop);
    if ( ret < 0 ) {
        /* Failed to add */
        return -1;
    }
    if ( nexthop != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Route update */
    nexthop = (void *)5678;
    ret = poptrie_route_update(poptrie, 0x1c001203, 32, nexthop);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }
    if ( nexthop != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Route delete */
    ret = poptrie_route_del(poptrie, 0x1c001203, 32);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }
    if ( NULL != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    /* Release */
    poptrie_release(poptrie);

    return 0;
}

static int
test_lookup_linx(char * filename, char* filename_out)
{
    struct poptrie *poptrie;
    FILE *fp;
    char buf[4096];
    int prefix[4];
    int prefixlen;
    int nexthop[4];
    int ret;
    u32 addr1;
    u32 addr2;
    u64 i;

    /* Load from the linx file */
    fp = fopen(filename, "r");
    if ( NULL == fp ) {
        return -1;
    }

    /* Initialize */
    poptrie = poptrie_init(NULL, 19, 22);
    if ( NULL == poptrie ) {
        return -1;
    }
    {
        u32* cnodes = poptrie->cnodes;
        int count = 0;
        poptrie_leaf_t* nodes = poptrie->leaves;
    }

    /* Load the full route */
    i = 0;

    double runtime = 0.0;
    clock_t start_time;
    clock_t end_time;

    while ( !feof(fp) ) {
        if ( !fgets(buf, sizeof(buf), fp) ) {
            continue;
        }
        ret = sscanf(buf, "%d.%d.%d.%d/%d %d.%d.%d.%d", &prefix[0], &prefix[1],
                     &prefix[2], &prefix[3], &prefixlen, &nexthop[0],
                     &nexthop[1], &nexthop[2], &nexthop[3]);
        if ( ret < 0 ) {
            return -1;
        }

        /* Convert to u32 */
        addr1 = ((u32)prefix[0] << 24) + ((u32)prefix[1] << 16)
            + ((u32)prefix[2] << 8) + (u32)prefix[3];
        addr2 = ((u32)nexthop[0] << 24) + ((u32)nexthop[1] << 16)
            + ((u32)nexthop[2] << 8) + (u32)nexthop[3];

        /* Add an entry */
        start_time = clock();
        ret = poptrie_route_add(poptrie, addr1, prefixlen, (void *)(u64)addr2);
        end_time = clock();
        runtime += (double)(end_time - start_time) / CLOCKS_PER_SEC;

        if ( ret < 0 ) {
            return -1;
        }
        if ( 0 == i % 10000 ) {
            TEST_PROGRESS();
        }
        i++;
    }
    double build_runtime = runtime;
    runtime = 0.0;

    int node_count = 0;
    u32* cnodes = poptrie->cnodes;
    int count = 0;
    poptrie_node_t* nodes = poptrie->nodes;
    for (i = 0 ; i < (1 << 19) ; i++) {
        if (((int)nodes[i].base0 != 0) && ((int)nodes[i].base1 != 0)) {
            count += 1;
        }
    }
    node_count = count;
    int leaf_count = 0;
    {

    u32* cnodes = poptrie->cnodes;
    int count = 0;
    poptrie_leaf_t* nodes = poptrie->leaves;
    for (i = 0 ; i < (1 << 22) ; i++) {
        if ((int)nodes[i] != 0) {
            count += 1;
        }
    }
    leaf_count = count;
    }

    start_time = clock();
    unsigned long long t0 = __rdtsc(); 
    for ( i = 0; i < 0x100000000ULL; i++ ) {
        if ( 0 == i % 0x10000000ULL ) {
            TEST_PROGRESS();
        }
        poptrie_lookup(poptrie, i);

#if BENCHMARK_ACTIVE != 1
        if (poptrie_lookup(poptrie, i)  != poptrie_rib_lookup(poptrie, i) ) {
            return -1;
        }
#endif
    }
    unsigned long long t1 = __rdtsc() - t0;
    end_time = clock();
    runtime += (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double lookup_runtime = runtime;
    int memory_footprint = node_count * 24 + leaf_count*2;
    double num_lookups = (double)0x100000000ULL;
    double lookup_rate = num_lookups / (runtime * 1e6);
    int max_nodes = (1 << 19);
    int max_leaves = (1 << 22);

    printf("\n                       build runtime: %lf\n", build_runtime);
    printf("                      lookup runtime: %lf\n", runtime);
    printf("                         num_lookups: %ld\n", (long int)num_lookups);
    printf("                    number of cycles: %llu\n", t1);
    printf("                     number of nodes: %d / %d (%2.1lf\%)\n", node_count, max_nodes, 100 * (double)node_count / (double)max_nodes);
    printf("                    number of leaves: %d / %d (%2.1lf\%)\n", leaf_count, max_leaves, 100 * (double)leaf_count / (double)max_leaves);
    printf("    memory footprint (based on tree): %d\n", memory_footprint);
    printf("                  lookup rate (Mlps): %lf\n", num_lookups / (runtime * 1e6));
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

    fprintf(fp_out, "poptrie\n%llu\n%lf\n%lf\n%lf\n%d\n%d\n%d\n%d\n%d\n%llu\n", 0x100000000ULL, lookup_rate, lookup_runtime, build_runtime,
        node_count, leaf_count, max_nodes, max_leaves, memory_footprint, t1);
    fclose(fp_out);

    /* Release */
    poptrie_release(poptrie);

    /* Close */
    fclose(fp);

    return 0;
}

static int
test_lookup_linx_update(char* filename, char* filename_update, char * filename_out)
{
    struct poptrie *poptrie;
    FILE *fp;
    char buf[4096];
    int prefix[4];
    int prefixlen;
    int nexthop[4];
    int ret;
    u32 addr1;
    u32 addr2;
    u64 i;
    int tm;
    char type;

    /* Initialize */
    poptrie = poptrie_init(NULL, 19, 22);
    if ( NULL == poptrie ) {
        return -1;
    }

    /* Load from the linx file */
    /* Example: "tests/linx-rib.20141217.0000-p52.txt" */
    fp = fopen(filename, "r");
    if ( NULL == fp ) {
        return -1;
    }

    /* Load the full route */
    i = 0;
    while ( !feof(fp) ) {
        if ( !fgets(buf, sizeof(buf), fp) ) {
            continue;
        }

        ret = sscanf(buf, "%d.%d.%d.%d/%d %d.%d.%d.%d", &prefix[0], &prefix[1],
                     &prefix[2], &prefix[3], &prefixlen, &nexthop[0],
                     &nexthop[1], &nexthop[2], &nexthop[3]);
        if ( ret < 0 ) {
            return -1;
        }

        /* Convert to u32 */
        addr1 = ((u32)prefix[0] << 24) + ((u32)prefix[1] << 16)
            + ((u32)prefix[2] << 8) + (u32)prefix[3];
        addr2 = ((u32)nexthop[0] << 24) + ((u32)nexthop[1] << 16)
            + ((u32)nexthop[2] << 8) + (u32)nexthop[3];

        /* Add an entry */
        ret = poptrie_route_add(poptrie, addr1, prefixlen, (void *)(u64)addr2);
        if ( ret < 0 ) {
            return -1;
        }
        if ( 0 == i % 10000 ) {
            TEST_PROGRESS();
        }
        i++;
    }

    /* Close */
    fclose(fp);

    /* Load from the update file */
    fp = fopen(filename_update, "r");
    if ( NULL == fp ) {
        return -1;
    }

    /* Load the full route */
    clock_t start_time = clock();
    i = 0;
    while ( !feof(fp) ) {
        if ( !fgets(buf, sizeof(buf), fp) ) {
            continue;
        }
        ret = sscanf(buf, "%d %c %d.%d.%d.%d/%d %d.%d.%d.%d", &tm, &type,
                     &prefix[0], &prefix[1], &prefix[2], &prefix[3], &prefixlen,
                     &nexthop[0], &nexthop[1], &nexthop[2], &nexthop[3]);
        if ( ret < 0 ) {
            return -1;
        }

        /* Convert to u32 */
        addr1 = ((u32)prefix[0] << 24) + ((u32)prefix[1] << 16)
            + ((u32)prefix[2] << 8) + (u32)prefix[3];
        addr2 = ((u32)nexthop[0] << 24) + ((u32)nexthop[1] << 16)
            + ((u32)nexthop[2] << 8) + (u32)nexthop[3];

        if ( 'a' == type ) {
            /* Add an entry (use update) */
            ret = poptrie_route_update(poptrie, addr1, prefixlen,
                                       (void *)(u64)addr2);
            if ( ret < 0 ) {
                return -1;
            }
        } else if ( 'w' == type ) {
            /* Delete an entry */
            ret = poptrie_route_del(poptrie, addr1, prefixlen);
            if ( ret < 0 ) {
                /* Ignore any errors */ // Originally it was not handled;
                return -1;              // Changed this to exit instead.
            }
        }
        if ( 0 == i % 1000 ) {
            TEST_PROGRESS();
        }
        i++;
    }
    clock_t end_time = clock();
    double runtime = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("update runtime: %lf\n", runtime);

    /* Close */
    fclose(fp);

    for ( i = 0; i < 0x100000000ULL; i++ ) {
        if ( 0 == i % 0x10000000ULL ) {
            TEST_PROGRESS();
        }
        if ( poptrie_lookup(poptrie, i) != poptrie_rib_lookup(poptrie, i) ) {
            return -1;
        }
    }

    /* Release */
    poptrie_release(poptrie);


    return 0;
}

/*
 * Main routine for the basic test
 */
int
main(int argc, const char *const argv[])
{
    int ret;
    ret = 0;

    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <linx-rib-file>\n", argv[0]);
        return -1;
    }

    /* Lookup test. */
    if (argc == 3) {
        char* filename = (char *)argv[1];
        char* filename_out = (char *)argv[2];
        int status = test_lookup_linx(filename, filename_out);
        if (status == 0) {
            printf("passed\n");
        }
        else if (status == -1) {
            printf("failed\n");
        }
    }
    /* Update test. */
    else if (argc == 4) {
        char* filename = (char *)argv[1];
        char* filename_update = (char *)argv[2];
        char* filename_out = (char *)argv[3];
        int status = test_lookup_linx_update(filename, filename_update, filename_out);
        if (status == 0) {
            printf("passed\n");
        }
        else if (status == -1) {
            printf("failed\n");
        }
    }

    /* Other tests */
    // TEST_FUNC("init", test_init, ret);
    // TEST_FUNC("lookup", test_lookup, ret);
    // TEST_FUNC("lookup2", test_lookup2, ret);
    // TEST_FUNC_REQUIRES_FILENAME("lookup_fullroute", test_lookup_linx, filename, ret);
    // TEST_FUNC_REQUIRES_FILENAME("lookup_fullroute_update", test_lookup_linx_update, filename, ret);

    return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
