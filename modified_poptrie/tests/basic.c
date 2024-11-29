/*_
 * Copyright (c) 2014-2016 Hirochika Asai <asai@jar.jp>
 * All rights reserved.
 */

#include "../poptrie.h"
#include "../buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<sys/time.h>

#define BUDDY_EOL 0xffffffffUL

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

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    /* Route update */
    nexthop = (void *)5678;
    ret = poptrie_route_update(poptrie, 0x1c001200, 24, nexthop);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    if ( nexthop != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    // printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    // printf("(benchmark-msg) poptrie->leavesz: %d\n", poptrie->leaves);

    /* Route delete */
    ret = poptrie_route_del(poptrie, 0x1c001200, 24);
    if ( ret < 0 ) {
        /* Failed to update */
        return -1;
    }

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    if ( NULL != poptrie_lookup(poptrie, 0x1c001203) ) {
        return -1;
    }
    TEST_PROGRESS();

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

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

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    /* Release */
    poptrie_release(poptrie);

    printf("(benchmark-msg) poptrie->nodesz: %d\n", poptrie->nodesz);
    printf("(benchmark-msg) poptrie->leafsz: %d\n", poptrie->leafsz);

    return 0;
}

static int
test_lookup_linx(char * filename)
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
        // printf("cnodes[0].level: %d\n", cnodes[0].level);
        int count = 0;
        poptrie_leaf_t* nodes = poptrie->leaves;
        for (i = 0 ; i < 10; i++) {
            // printf("cnodes[%d] == END: %d\n", i, (cnodes[i] == BUDDY_EOL));
            // if ((int)nodes[i].base1 != 0) {
                // printf("nodes[%d]: %d / %d\n", (int)i, (int)(nodes[i].leafvec), (int)(nodes[i].base1));
                // count += 1;
            // }
            printf("node[%d]: %d\n", i, (int)nodes[i]);
        }
        printf("count: %d\n", count);
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
        // printf(" i: %d\n", i);

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
    printf("compilation runtime: %lf\n", runtime);
    runtime = 0.0;

    int node_count = 0;
    u32* cnodes = poptrie->cnodes;
    // printf("cnodes[0].level: %d\n", cnodes[0].level);
    int count = 0;
    poptrie_node_t* nodes = poptrie->nodes;
    printf("test\n");
    printf("(1 << 19): %d\n", (1 << 19));
    for (i = 0 ; i < (1 << 19) ; i++) {
        // printf("cnodes[%d] == END: %d\n", i, (cnodes[i] == BUDDY_EOL));
        if (((int)nodes[i].base0 != 0) && ((int)nodes[i].base1 != 0)) {
            count += 1;
        }
    }
    printf("nodes-count: %d\n", count);
    node_count = count;
    int leaf_count = 0;
    {

    u32* cnodes = poptrie->cnodes;
    // printf("cnodes[0].level: %d\n", cnodes[0].level);
    int count = 0;
    poptrie_leaf_t* nodes = poptrie->leaves;
    printf("test\n");
    printf("(1 << 22): %d\n", (1 << 22));
    for (i = 0 ; i < (1 << 22) ; i++) {
        // printf("cnodes[%d] == END: %d\n", i, (cnodes[i] == BUDDY_EOL));
        // if (((int)nodes[i].base0 == 0) && ((int)nodes[i].base1 == 0)) {
            // count += 1;
        // }
        // printf("node[%d]: %d\n", i, (int)nodes[i]);
        if ((int)nodes[i] != 0) {
            count += 1;
        }
    }
    printf("leaves-count: %d\n", count);
    leaf_count = count;
    }

    start_time = clock();
    // uint64_t t0 = rdtsc(); 
    for ( i = 0; i < 0x100000000ULL; i++ ) {
        if ( 0 == i % 0x10000000ULL ) {
            TEST_PROGRESS();
        }
        poptrie_lookup(poptrie, i);
        // if (poptrie_lookup(poptrie, i)  != poptrie_rib_lookup(poptrie, i) ) {
            // return -1;
        // }
    }
    // uint64_t t1 = rdtsc() - t0;
    end_time = clock();
    runtime += (double)(end_time - start_time) / CLOCKS_PER_SEC;
    int memory_footprint = node_count * 24 + leaf_count*2;
    double num_lookups = (double)0x100000000ULL;
    printf("num_lookups: %lf\n", num_lookups);
    // printf("number of cycles: %d\n", (int) t1);
    printf("runtime: %lf\n", runtime);
    printf("memory_footprint: %d\n", memory_footprint);
    printf("rate: %lf (Mlps)\n", num_lookups / (runtime * 1e6));
    printf("0x100000000ULL: %llu\n", 0x100000000ULL);


    /* Release */
    poptrie_release(poptrie);

    /* Close */
    fclose(fp);

    return 0;
}

static int
test_lookup_linx_update(char * filename)
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
    // "tests/linx-rib.20141217.0000-p52.txt"
    /* Load from the linx file */
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
    fp = fopen(filename, "r");
    if ( NULL == fp ) {
        return -1;
    }

    clock_t start_time = clock();
    /* Load the full route */
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
                /* Ignore any errors */
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
    // sleep(1);
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <linx-rib-file>\n", argv[0]);
        return -1;
    }
    char * filename = (char *)argv[1];


    /* Run tests */
    // TEST_FUNC("init", test_init, ret);
    // TEST_FUNC("lookup", test_lookup, ret);
    // TEST_FUNC("lookup2", test_lookup2, ret);
    TEST_FUNC_REQUIRES_FILENAME("lookup_fullroute", test_lookup_linx, filename, ret);
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
