#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include <stdlib.h> //for using the function sleep
#include <stdio.h>
#include <condition_variable> // std::condition_variable
#include <unistd.h>

using namespace std;
using namespace std::chrono;
#define NUM_THREADS 5
void complex()
{
    int a = 0;
    for (int i = 0; i < 600000000; i++)
    {
        a = i * i;
    }
    cout << "DONE!" << endl;
}
void *PrintHello(void *threadid)
{
    int j = 0;
    for (int i = 0; i < 20000000; i++)
    {
        j++;
    }
    cout << j << endl;
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int rc;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 5; i++)
    {

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