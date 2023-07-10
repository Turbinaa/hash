#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <regex.h>
#include <pthread.h>
#include "../include/sha256.h"
#include "../include/main.h"

int num_threads = 1;
bool terminate = false;
size_t suma = 0;
typedef struct
{
    int thread_id;
    unsigned char *hash_to_guess;
    bool verbose;
    int maxlen;
    int urdata;
} GuessArgs;

pthread_t threads[32];

int main(int argc, char *argv[])
{
    bool verbose = false;
    char *text_argument = NULL;
    for (int k = 1; k < argc; k++)
    {
        if (strcmp(argv[k], "--threads") == 0 || strcmp(argv[k], "-t") == 0)
        {
            k++;
            if (k < argc)
            {
                num_threads = atoi(argv[k]);
                if (num_threads <= 0 || num_threads > 32)
                {
                    printf("Invalid number of threads: allowed range: 1-32\n");
                    return 1;
                }
            }
        }
        else if (strcmp(argv[k], "--verbose") == 0 || strcmp(argv[k], "-v") == 0)
        {
            verbose = true;
        }
        else
        {
            text_argument = argv[k];
        }
    }

    unsigned char *hash_to_guess = (unsigned char *)to_binary(text_argument);
    if (hash_to_guess == NULL)
    {
        printf("You have to specify SHA-256 hash you want to brute-force!\n");
        return 1;
    }
    int urandom_data = -1;
    // GET the data from /dev/urandom
    urandom_data = open("/dev/urandom", O_RDONLY);
    if (urandom_data < 0)
    {
        fprintf(stderr, "Failed to open /dev/urandom. Aborting\n");
        exit(1);
    }

    int i;
    GuessArgs guess_args[num_threads];
    for (i = 0; i < num_threads; ++i)
    {
        guess_args[i].thread_id = i;
        guess_args[i].hash_to_guess = hash_to_guess;
        guess_args[i].verbose = verbose;
        guess_args[i].urdata = urandom_data;
        guess_args[i].maxlen = MAXLEN;
        pthread_create(
            &threads[i],
            NULL,
            guess,
            (void *)&guess_args[i]);
    }
    for (i = 0; i < num_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    if (terminate)
        printf("Tries: %ld\n", suma);
    close(urandom_data);
    return 0;
}
unsigned char *to_binary(const char *text)
{
    unsigned char *sha256bin = (unsigned char *)malloc(SHA256_BLOCK_SIZE);
    int i;

    for (i = 0; i < SHA256_BLOCK_SIZE; i++)
    {
        sscanf(text + (i * 2), "%2hhx", sha256bin + i);
    }

    return sha256bin;
}

unsigned char *calc_hash(const unsigned char *input, size_t len)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, input, len);
    unsigned char *hash = (unsigned char *)malloc(SHA256_BLOCK_SIZE);
    if (hash == NULL)
        exit(1);

    sha256_final(&ctx, hash);
    return hash;
}

char *generate_string(int len, int urdata)
{
    char *string = (char *)malloc((len + 1) * sizeof(char));
    int i;
    if (urdata == -1)
    {
        fprintf(stderr, "/dev/urandom not initialized. Aborting\n");
    }
    ssize_t result = read(urdata, string, len);
    if (result < 0)
    {
        fprintf(stderr, "Failed to read /dev/urandom. Aborting\n");
        exit(1);
    }

    for (i = 0; i < len; ++i)
    {
        string[i] = 'a' + (string[i] % 26);
    }
    string[len] = '\0';
    return string;
}

void *guess(void *thread_args)
{
    GuessArgs *args = (GuessArgs *)thread_args;
    int thread_id = args->thread_id;
    unsigned char *hash_to_guess = args->hash_to_guess;
    bool verbose = args->verbose;
    int maxlen = args->maxlen;
    int urdata = args->urdata;
    static size_t i = 0;
    short n = MINLEN - 1;
    char *data;
    unsigned char *hash;
    printf("Begin brute-forcing on thread %d\n", thread_id);
    while (!terminate)
    {
        n++;
        data = generate_string(n, urdata);
        hash = calc_hash((unsigned char *)data, strlen(data));
        if (verbose)
        {
            // for (int n = 0; n < SHA256_BLOCK_SIZE; n++)
            // {
            //     printf("%02x", hash[n]);
            // }
            printf("%s\n", data);
        }
        if (strcmp((char *)hash_to_guess, (char *)hash) == 0 && !terminate)
        {
            terminate = true;
            printf("Successfully brute-forced your hash using %d threads, saved to hash-results.txt\n", num_threads);
            save_data(data, hash);
            break;
        }
        free(hash);
        free(data);
        i++;
        if (n > maxlen)
            n = MINLEN - 1;
    }
    suma += i;
    pthread_exit(&thread_id);
    return NULL;
}

void save_data(const char *text, const unsigned char *hash)
{
    time_t t = time(NULL);
    FILE *file = fopen("hash-results.txt", "w");
    if (file == NULL)
    {
        fprintf(stderr, "erm... we couldn't open results.txt");
        return;
    }
    fprintf(file, "###############SHA-256 results###############\n");
    fprintf(file, "Time: %s", ctime(&t));
    fprintf(file, "Hash: ");
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
    {
        fprintf(file, "%02x", hash[i]);
    }
    fprintf(file, "\n");
    fprintf(file, "Guessed word: %s\n", text);
    fclose(file);
}