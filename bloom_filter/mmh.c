#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 

#define HASH_NUM 8
#define FILTER_SIZE 250000

typedef struct {
    int* seeds;
    char** filters;
} bf_t; 

bf_t* create_bf();
void insert_bf(bf_t *b, char *s);
int is_element(bf_t *b, char *q);

typedef unsigned uint32_t;
typedef unsigned char uint8_t;
typedef unsigned long long uint64_t;

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    if (len > 3) {
        const uint32_t* key_x4 = (const uint32_t*) key;
        size_t i = len >> 2;
        do {
            uint32_t k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
        key = (const uint8_t*) key_x4;
    }
    if (len & 3) {
        size_t i = len & 3;
        uint32_t k = 0;
        key = &key[i - 1];
        do {
            k <<= 8;
            k |= *key--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

uint64_t fnv_1(const char *key) {
    uint64_t hash = 0xcbf29ce484222325;
    const char *k = key;
    while (*k != 0) {
        hash *= 0x100000001b3;
        hash ^= (uint64_t)*k++;
    }
    return hash;
}

size_t fibonacci_hash_24_bits(size_t hash) {
    //return (hash * 11400714819323198485llu) >> 40;
    return (hash * 11400714819323198485llu);
}

bf_t *create_bf() {
    int i;
    srand(time(0));
    bf_t *bf = malloc(sizeof(bf_t));
    bf->seeds = malloc(HASH_NUM * sizeof(int));
    bf->filters = malloc(HASH_NUM * sizeof(char *));
    for (i = 0; i < HASH_NUM; i++) {
        bf->seeds[i] = rand();
        bf->filters[i] = malloc(FILTER_SIZE * sizeof(char));
        memset(bf->filters[i], 0, FILTER_SIZE);
    }
    return bf;
}

void insert_bf(bf_t *b, char *s) {
    int i; 
    unsigned hash, index, offset;
    unsigned long long tmp;
    for (i = 0; i < HASH_NUM; i++) {
        hash = murmur3_32(s, (unsigned)strlen(s), b->seeds[i]);
        tmp = fibonacci_hash_24_bits(hash);
        index = tmp % (FILTER_SIZE * 8);
        offset = index % 8;
        b->filters[i][index / 8] |= 1 << offset;
        //printf("%d, %d, %d\n", i, index, offset);
    }
}

int is_element(bf_t *b, char *q) {
    int i; 
    unsigned hash, index, offset;
    unsigned long long tmp;
    for (i = 0; i < HASH_NUM; i++) {
        hash = murmur3_32(q, (unsigned)strlen(q), b->seeds[i]);
        tmp = fibonacci_hash_24_bits(hash);
        index = tmp % (FILTER_SIZE * 8);
        offset = index % 8;
        //printf("%d, %d, %d\n", i, index, offset);
        if ((int)(b->filters[i][index / 8] & (1 << offset)) == 0) {
            return 0;
        }
    }
    return 1;
}

void sample_string_A(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = 'b';
   s[5] = (char)(1 + ((i*(i-3)) %217));
   s[6] = (char)(1 + ((17*i+129)%233 ));
   s[7] = '\0';
}
void sample_string_B(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = (char)(1 + ((i*(i-3)) %217));
   s[5] = (char)(1 + ((17*i+129)%233 ));
   s[6] = '\0';
}
void sample_string_C(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = 'a';
   s[3] = (char)(1 + ((i*(i-3)) %217));
   s[4] = (char)(1 + ((17*i+129)%233 ));
   s[5] = '\0';
}
void sample_string_D(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'b';
   s[4] = 'b';
   s[5] = (char)(1 + ((i*(i-3)) %217));
   s[6] = (char)(1 + ((17*i+129)%233 ));
   s[7] = '\0';
}
void sample_string_E(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = (char)(2 + ((i*(i-3)) %217));
   s[5] = (char)(1 + ((17*i+129)%233 ));
   s[6] = '\0';
}

int main()
{  long i,j; 
   bf_t * bloom;
   bloom = create_bf();
   printf("Created Filter\n");
   for( i= 0; i< 1450000; i++ )
   {  char s[8];
      sample_string_A(s,i);
      insert_bf( bloom, s );
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_B(s,i);
      insert_bf( bloom, s );
   }
   for( i= 0; i< 50000; i++ )
   {  char s[6];
      sample_string_C(s,i);
      insert_bf( bloom, s );
   }
   printf("inserted 2,000,000 strings of length 8,7,6.\n");
   
   for( i= 0; i< 1450000; i++ )
   {  char s[8];
      sample_string_A(s,i);
      if( is_element( bloom, s ) != 1 )
    {  printf("found negative error (1)\n"); exit(0); }
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_B(s,i);
      if( is_element( bloom, s ) != 1 )
    {  printf("found negative error (2)\n"); exit(0); }
   }
   for( i= 0; i< 50000; i++ )
   {  char s[6];
      sample_string_C(s,i);
      if( is_element( bloom, s ) != 1 )
    {  printf("found negative error (3)\n"); exit(0); }
   }
   j = 0;
   for( i= 0; i< 500000; i++ )
   {  char s[8];
      sample_string_D(s,i);
      if( is_element( bloom, s ) != 0 )
        j+=1;
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_E(s,i);
      if( is_element( bloom, s ) != 0 ) {
        j+=1;
      }
   }
   printf("Found %d positive errors out of 1,000,000 tests.\n",j);
   printf("Positive error rate %f\%.\n", (float)j/10000.0);
    
   /*
    int c = 0;
    int k;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < FILTER_SIZE; j++) {
            char tmp = bloom->filters[i][j];
            for (k = 0; k < 8; k++) {
                if ((int)(tmp & 1 << k) != 0) {
                    c++;
                }
            }
        }
    }
    printf("not zero is %d\n", c);
    */
}
