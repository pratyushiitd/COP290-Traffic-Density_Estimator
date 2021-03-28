#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>

using namespace std;
using namespace std::chrono;
#define NUM_THREADS 2

void *PrintHello(void *threadid)
{
    long x = 10000;
    long y = 900000000;
    while (y > 0)
    {
        x = x * y;
        x = x / y;
        y--;
    }
    long tid;
    tid = (long)threadid;
    cout << "Hello World! Thread ID, " << tid << endl;
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int rc;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < NUM_THREADS; i++)
    {
        cout << "main() : creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, PrintHello, (void *)i);

        if (rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() / 1000000 << " seconds" << endl;
    pthread_exit(NULL);
}