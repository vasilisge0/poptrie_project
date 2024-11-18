/*_
 * Copyright (c) 2016,2018 Hirochika Asai <asai@jar.jp>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../radix.h"
#include <stdio.h>
#include <sys/time.h>
#include <inttypes.h>

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
 * Xorshift
 */
static __inline__ uint32_t
xor128(void)
{
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x<<11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w>>19)) ^ (t ^ (t >> 8));
}

static __inline__ double
getmicrotime(void)
{
    struct timeval tv;
    double microsec;

    if ( 0 != gettimeofday(&tv, NULL) ) {
        return 0.0;
    }

    microsec = (double)tv.tv_sec + (1.0 * tv.tv_usec / 1000000);

    return microsec;
}

/*
 * Initialization test
 */
static int
test_init(void)
{
    struct radix_tree *radix;

    /* Initialize */
    radix = radix_tree_init(NULL);
    if ( NULL == radix ) {
        return -1;
    }

    TEST_PROGRESS();

    /* Release */
    radix_tree_release(radix);

    return 0;
}

/*
 * Lookup test
 */
static int
test_lookup(void)
{
    struct radix_tree *radix;
    uint8_t key0[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45};
    uint8_t key1[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    int ret;
    void *val;

    /* Initialize */
    radix = radix_tree_init(NULL);
    if ( NULL == radix ) {
        return -1;
    }

    /* No entry */
    val = radix_tree_lookup(radix, key0);
    if ( NULL != val ) {
        /* Failed */
        return -1;
    }

    TEST_PROGRESS();

    /* Insert */
    ret = radix_tree_add(radix, key1, 64, key1);
    if ( ret < 0 ) {
        /* Failed to insert */
        return -1;
    }

    /* Lookup */
    val = radix_tree_lookup(radix, key1);
    if ( val != key1 ) {
        /* Failed */
        return -1;
    }

    /* Lookup */
    val = radix_tree_delete(radix, key1, 64);
    if ( val != key1 ) {
        /* Failed */
        return -1;
    }

    /* Lookup */
    val = radix_tree_lookup(radix, key1);
    if ( NULL != val ) {
        /* Failed */
        return -1;
    }

    /* Release */
    radix_tree_release(radix);

    return 0;
}

static int
test_lookup_linx_performance(char * filename)
{
    struct radix_tree *rt;
    FILE *fp;
    char buf[4096];
    int prefix[4];
    int prefixlen;
    int nexthop[4];
    int ret;
    uint8_t addr1[4];
    uint64_t addr2;
    ssize_t i;
    uint64_t res;
    double t0;
    double t1;
    uint32_t a;

    /* Load from the linx file */
    fp = fopen(filename, "r");
    if ( NULL == fp ) {
        return -1;
    }

    /* Initialize */
    rt = radix_tree_init(NULL);
    if ( NULL == rt ) {
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
        addr1[0] = prefix[0];
        addr1[1] = prefix[1];
        addr1[2] = prefix[2];
        addr1[3] = prefix[3];
        addr2 = ((uint32_t)nexthop[0] << 24) + ((uint32_t)nexthop[1] << 16)
            + ((uint32_t)nexthop[2] << 8) + (uint32_t)nexthop[3];

        /* Add an entry */
        ret = radix_tree_add(rt, addr1, prefixlen, (void *)(uint64_t)addr2);
        if ( ret < 0 ) {
            return -1;
        }
        if ( 0 == i % 10000 ) {
            TEST_PROGRESS();
        }
        i++;
    }

    t0 = getmicrotime();

    res = 0;
    for ( i = 0; i < 0x100000000LL; i++ ) {
        if ( 0 == i % 0x10000000ULL ) {
            TEST_PROGRESS();
        }
        a = xor128();
        res ^= (uint64_t)radix_tree_lookup(rt, (uint8_t *)&a);
    }
    t1 = getmicrotime();

    printf("RESULT: %" PRIu64 "\n", res);

    printf("Result[0]: %lf ns/lookup\n", (t1 - t0)/i * 1000000000);
    printf("Result[1]: %lf Mlps\n", 1.0 * i / (t1 - t0) / 1000000);

    /* Release */
    radix_tree_release(rt);

    /* Close */
    fclose(fp);

    return 0;
}

/*
 * Main routine for the basic test
 */
int
main(int argc, const char *const argv[])
{
    int ret;

    /* Reset */
    ret = 0;
    if (2 > argc) {
        fprintf(stderr, "Usage: %s <linx file>\n", argv[0]);
        return -1;
    }
    const char *filename = argv[1];

    /* Run tests */
    TEST_FUNC("init", test_init, ret);
    TEST_FUNC("lookup", test_lookup, ret);
    TEST_FUNC_REQUIRES_FILENAME("performance", test_lookup_linx_performance, filename, ret);

    return 0;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
