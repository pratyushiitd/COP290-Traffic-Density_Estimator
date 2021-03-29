#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <math.h>
#include <chrono>
#include <pthread.h>
using namespace std;
using namespace cv;
using namespace std::chrono;
// void complex()
// {
//     int a = 0;
//     for (int i = 0; i < 600000000; i++)
//     {
//         a = i * i;
//     }
//     cout << "DONE!" << endl;
// }
// void *PrintHello(void *threadid)
// {
//     int j = 0;
//     for (int i = 0; i < 20000000; i++)
//     {
//         j++;
//     }
//     cout << j << endl;
//     pthread_exit(NULL);
// }

// int main()
// {
//     pthread_t threads[NUM_THREADS];
//     int rc;

//     auto start = high_resolution_clock::now();
//     for (int i = 0; i < 5; i++)
//     {

//         for (int i = 0; i < NUM_THREADS; i++)
//         {
//             cout << "main() : creating thread, " << i << endl;
//             rc = pthread_create(&threads[i], NULL, PrintHello, (void *)i);
//             if (rc)
//             {
//                 cout << "Error:unable to create thread," << rc << endl;
//                 exit(-1);
//             }
//         }
//     }
//     for (int i = 0; i < NUM_THREADS; i++)
//     {
//         pthread_join(threads[i], NULL);
//     }
//     auto stop = high_resolution_clock::now();

//     auto duration = duration_cast<microseconds>(stop - start);

//     cout << "Time taken by function: "
//          << duration.count() / 1000000 << " seconds" << endl;
//     pthread_exit(NULL);
// }
int main(int argc, char const *argv[])
{
    // auto start = high_resolution_clock::now();
    // VideoCapture cap("../assets/trafficvideo.mp4");
    // Mat frame;
    // int index = 0;
    // while (cap.read(frame))
    // {
    //     // cout << "the current frame: " << index << endl;
    //     index++;
    // }
    // auto stop = high_resolution_clock::now();

    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << "Time taken by function: "
    //      << duration.count() / 1000000.0 << " seconds" << endl;

    auto start2 = high_resolution_clock::now();
    VideoCapture capture("../assets/trafficvideo.mp4");
    Mat frame2;
    int framec = 0;
    int totalFrames = capture.get(CAP_PROP_FRAME_COUNT); //Getting the total number of frames//

    while (true)
    {
        if (totalFrames < framec + 2)
        {
            break;
        }
        if (framec % 1000 != 0)
        {
            framec++;
            continue;
        }
        capture.set(CAP_PROP_POS_FRAMES, framec + 1);
        cout << "Frame Number: " << framec << endl;
        capture.read(frame2);
        // cout << "the current frame: " << index << endl;
        framec++;
        if (frame2.empty())
            break;
    }
    auto stop2 = high_resolution_clock::now();

    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout << "Time taken by function: "
         << duration2.count() / 1000000.0 << " seconds" << endl;

    return 0;
}
