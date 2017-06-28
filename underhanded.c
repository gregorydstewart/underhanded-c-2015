/*
 * Underhanded C Contest, 2015
 * Gregory Stewart
 *
 * Compile:
 *   cc underhanded.c
 *
 * Usage:
 *   ./a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_TESTS   10
#define NUM_SAMPLES 4
#define THRESHOLD   0.2
#define POOL_SIZE   12

struct sample {
    double test;
    double reference;
    double threshold;
    struct sample *next;
};

struct sample *pool_head = NULL;

int
pool_create()
{
    struct sample *s, *prev, *head;
    int i, result;

    result = 1;
    prev = head = NULL;
    for (i = 0; i < POOL_SIZE; i++) {
        s = malloc(sizeof(struct sample));
        if (!s)
            goto error;
        memset(s, 0, sizeof(struct sample));

        if (prev)
            prev->next = s;
        else
            head = s;

        prev = s;
    }
    goto done;

error:
    result = 0;

done:
    pool_head = head;
    return result;
}

void
pool_destroy()
{
    struct sample *s, *head;

    head = pool_head;
    while (head) {
        s = head->next;
        free(head);
        head = s;
    }

    pool_head = NULL;
}

int
pool_acquire(struct sample **s)
{
    struct sample *p;
    int result;

    result = 1;
    if (!s)
        goto error;

    p = pool_head;
    if (!p) {
        p = malloc(sizeof(struct sample));
        if (!p)
            goto error;
        memset(p, 0, sizeof(struct sample));
        *s = p;
    } else {
        if (p->next) {
            pool_head = p->next;
            p->next = NULL;
        } else {
            pool_head = NULL;
        }
        *s = p;
        goto done;
    }

error:
    result = 0;

done:
    return result;
}

void
pool_release(struct sample **s)
{
    if (s) {
        if (*s) {
            (*s)->next = pool_head;
            pool_head = *s;
            *s = NULL;
        }
    }
}

struct sample *
sample_create(double test, double reference, double threshold)
{
    struct sample *s;

    if (pool_acquire(&s)) {
        s->test = test;
        s->reference = reference;
        s->threshold = threshold;
        s->next = NULL;
    }

    return s;
}

void
sample_destroy(struct sample **s)
{
    pool_release(s);
}

int
sample_match(struct sample *s)
{
    int result;

    result = 1;
    if (!s)
        goto error;

    while (s) {
        if (fabs(s->test - s->reference) > s->threshold)
            goto error;
        s = s->next;
    }
    goto done;

error:
    result = 0;

done:
    return result;
}

int
match(double *test, double *reference, int bins, double threshold)
{
    struct sample *s, *prev, *head;
    int i, result;

    s = prev = head = NULL;
    for (i = 0; i < bins; i++) {
        s = sample_create(test[i], reference[i], threshold);

        if (prev)
            prev->next = s;
        else
            head = s;

        prev = s;
    }
    
    result = sample_match(head);
    sample_destroy(&s);
    return result;
}

int
main(int argc, char **argv)
{
    struct test {
        double test[NUM_SAMPLES];
        double reference[NUM_SAMPLES];
        int bins;
        double threshold;
    } test[NUM_TESTS] = {
        /* first and second test (match = YES) */
        { { 1.0, 1.9, 0.3, 5.4 }, { 1.2, 2.0, 0.3, 5.3 }, NUM_SAMPLES, THRESHOLD },
        { { 1.0, 1.9, 0.3, 5.4 }, { 1.2, 2.0, 0.3, 5.3 }, NUM_SAMPLES, THRESHOLD },

        /* first and second test (match = NO) */
        { { 9.0, 0.7, 3.5, 7.1 }, { 4.0, 2.0, 8.1, 1.7 }, NUM_SAMPLES, THRESHOLD },
        { { 9.0, 0.7, 3.5, 7.1 }, { 4.0, 2.0, 8.1, 1.7 }, NUM_SAMPLES, THRESHOLD },

        /* first and second test (match should be NO, but is YES) */
        { { 2.0, 9.1, 8.8, 1.0 }, { 2.0, 3.6, 2.5, 7.2 }, NUM_SAMPLES, THRESHOLD },
        { { 2.0, 9.1, 8.8, 1.0 }, { 2.0, 3.6, 2.5, 7.2 }, NUM_SAMPLES, THRESHOLD },
        
        /* first and second test (match = NO) */
        { { 0.1, 4.3, 7.9, 2.0 }, { 8.9, 1.6, 2.5, 7.2 }, NUM_SAMPLES, THRESHOLD },
        { { 0.1, 4.3, 7.9, 2.0 }, { 8.9, 1.6, 2.5, 7.2 }, NUM_SAMPLES, THRESHOLD },

        /* first and second test (match = YES) */
        { { 9.0, 0.2, 6.7, 3.4 }, { 8.9, 0.1, 6.9, 3.5 }, NUM_SAMPLES, THRESHOLD },
        { { 9.0, 0.2, 6.7, 3.4 }, { 8.9, 0.1, 6.9, 3.5 }, NUM_SAMPLES, THRESHOLD }
    };
    int i, result;

    pool_create();
    for (i = 0; i < NUM_TESTS; i++) {
        result = match(test[i].test, test[i].reference, NUM_SAMPLES, THRESHOLD);
        printf("Test #%d: Match=%s\n", i, result? "YES" : "NO");
    }
    pool_destroy();

    return 0;
}

