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
int NUM_THREADS;
vector<pair<int, int>>
    queue_y;
vector<pair<int, int>> dynamic_y;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
void CallBackFunc(int event, int x, int y, int flags, void *params)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        vector<Point2f> *points = (vector<Point2f> *)params;
        if (points->size() < 4)
        {
            points->push_back(Point2f(x, y));
        }
    }
}

int get_quad(const Point &p1)
{
    if (p1.x > 0 && p1.y < 0)
        return 4;
    else if (p1.x < 0 && p1.y < 0)
        return 1;
    else if (p1.x < 0 && p1.y > 0)
        return 2;
    return 3;
}
bool compare_points(const Point &a, const Point &b)
{
    return (get_quad(a) < get_quad(b));
}
vector<Point2f> sort_points(vector<Point2f> points)
{
    int Mx = (points[0].x + points[1].x + points[2].x + points[3].x) / 4;
    int My = (points[0].y + points[1].y + points[2].y + points[3].y) / 4;
    for (int a = 0; a < 4; a++)
    {
        points[a].x -= Mx;
        points[a].y -= My;
    }
    std::sort(points.begin(), points.end(), compare_points);
    for (int a = 0; a < 4; a++)
    {
        points[a].x += Mx;
        points[a].y += My;
        cout << "(" << points[a].x << ", " << points[a].y << ")" << endl;
    }
    return points;
}
int display_whiteratio_dynamic(Mat &gry, Mat &frame, int pastValue)
{
    if (pastValue == -1)
    {
        vector<Point> white_pixels_dyn;
        cv::findNonZero(gry, white_pixels_dyn);
        int white_ratio_dyn = ((white_pixels_dyn.size()) * 1000.0) / (gry.cols * gry.rows);
        stringstream sst;
        sst << white_ratio_dyn;
        white_pixels_dyn.clear();
        return white_ratio_dyn;
    }
    else
    {
        return pastValue;
    }
}
int display_whiteratio_queue(Mat &fgMask, Mat &frame, int pastValue)
{
    if (pastValue == -1)
    {
        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20), cv::Scalar(255, 255, 255), -1); //Display white ratio on top left corner
        stringstream ss;
        //ss << capture.get(CAP_PROP_POS_FRAMES); //0-based index of the frame to be decoded/captured next.
        vector<Point> white_pixels;
        cv::findNonZero(fgMask, white_pixels);
        //ss << capture.get(CAP_PROP_POS_FRAMES);
        int white_ratio = ((white_pixels.size()) * 1000.0) / (fgMask.cols * fgMask.rows);
        ss << white_ratio;
        white_pixels.clear();
        return white_ratio;
    }
    else
    {
        return pastValue;
    }
}
void write_out_queue(vector<pair<int, int>> queue_y)
{
    freopen("../outputs/static.out", "w", stdout);
    for (int i = 0; i < queue_y.size(); i++)
    {
        if (i == queue_y.size() - 1)
        {
            cout << queue_y[i].second / 1000.0;
            break;
        }

        cout << queue_y[i].second / 1000.0 << ",";
    }
}
void write_out_dynamic(vector<pair<int, int>> dynamic_y)
{
    freopen("../outputs/dynamic.out", "w", stdout);
    for (int i = 0; i < dynamic_y.size(); i++)
    {
        if (i == dynamic_y.size() - 1)
        {
            cout << dynamic_y[i].second / 1000.0;
            break;
        }
        cout << dynamic_y[i].second / 1000.0 << ",";
    }
}
Mat evaluate_dense_opticalflow(Mat &next, Mat &prvs, Mat frame)
{
    cvtColor(frame, next, COLOR_BGR2GRAY);
    Mat flow(prvs.size(), CV_32FC2);
    calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 7, 1.5, 0);
    // visualization
    Mat flow_parts[2];
    split(flow, flow_parts);

    //flow parts[0] has x coordinates of pixel displacement vectors
    //flow parts[1] has y coordinates of pixel displacement vectors
    Mat magnitude, angle, magn_norm, gr_bt, temp;

    //magnitude and angle calculation using x and y coordinate vectors
    //angle in degrees is set to true
    cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);

    //normalize the magnitude Matrix and output into magn_norm Matrix
    // Min = 0 and Max = 1
    normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    //threshold(temp, magn_norm, 0.4f, 1.0f, THRESH_BINARY);
    //threshold(gr_bt, magn_norm,0.8,1.0, THRESH_BINARY);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    //build hsv image
    Mat _hsv[3], hsv, hsv8, bgr, gry;
    _hsv[0] = angle;
    _hsv[1] = Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    cvtColor(bgr, gr_bt, COLOR_BGR2GRAY);
    threshold(gr_bt, gry, 15, 255, THRESH_BINARY);
    return gry;
}
struct thread_data
{
    int thread_id;
    VideoCapture capture;
    int framec;
};
void helper(int threadNumber, VideoCapture capture)
{
    int framec = -1;
    int totalFrames = capture.get(CAP_PROP_FRAME_COUNT); //Getting the total number of frames//
    // cout << totalFrames << endl;
    string image1_path = samples::findFile("../assets/empty.jpg");
    Mat img1 = imread(image1_path, IMREAD_GRAYSCALE);
    vector<Point2f> points;
    points.push_back(Point2f(948, 270));
    points.push_back(Point2f(205, 1062));
    points.push_back(Point2f(1551, 1064));
    points.push_back(Point2f(1296, 249));
    vector<Point2f> pts_dst;

    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));

    Mat H = findHomography(points, pts_dst);
    Rect crop_region(points[0].x, points[0].y, points[3].x - points[0].x, points[1].y - points[0].y);

    cout << "Success1" << endl;
    Ptr<BackgroundSubtractor> obj_back;                       //create Background Subtractor object
    obj_back = createBackgroundSubtractorMOG2(1, 350, false); // (History, threshold, detech_shadows = false)
    //obj_back = createBackgroundSubtractorKNN();

    Mat frame, fgMask, prvs, frame1, old_gray;
    // vector<pair<int, int>> queue_y;
    // vector<pair<int, int>> dynamic_y;

    capture >> frame1;
    warpPerspective(frame1, frame1, H, frame.size());
    frame1 = frame1(crop_region);
    cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    warpPerspective(img1, img1, H, img1.size());
    // frame1 = img1;
    frame = img1(crop_region);
    //obj_back->apply(frame, fgMask, 1);
    int processf = 5;
    int qPastValue = 0;
    int dPastValue = 0;
    //==================
    // Create some random colors
    //======================
    while (true)
    {
        // if (framec % 100 == 0)
        //     cout << framec << " " << threadNumber << endl;

        framec++;
        if (totalFrames < framec + 2)
        {
            break;
        }
        capture.read(frame);

        if ((framec / processf) % NUM_THREADS != threadNumber)
        {
            continue;
        }

        if (framec % processf != 0)
        {
            pthread_mutex_lock(&mutex1);
            queue_y.push_back(make_pair(framec, qPastValue));
            dynamic_y.push_back(make_pair(framec, dPastValue));
            pthread_mutex_unlock(&mutex1);
            continue;
        }
        // capture.set(CAP_PROP_POS_FRAMES, framec);
        // cout << "Frame Number: " << framec << " is proccessed by Thread Number: " << threadNumber << endl;
        // capture.read(frame);
        if (frame.empty())
            break;

        warpPerspective(frame, frame, H, frame.size());
        frame = frame(crop_region);
        fgMask = img1;
        obj_back->apply(frame, fgMask, 0); //Learning rate set to 0
        qPastValue = display_whiteratio_queue(fgMask, frame, -1);
        pthread_mutex_lock(&mutex1);
        queue_y.push_back(make_pair(framec, qPastValue));
        pthread_mutex_unlock(&mutex1);
        //==================================================================================
        //Optical Flow Evaluation
        Mat next;
        Mat gry = evaluate_dense_opticalflow(next, prvs, frame);
        //Display white ratio in white box on top left corner for optical flow frame
        dPastValue = display_whiteratio_dynamic(gry, frame, -1);
        pthread_mutex_lock(&mutex1);
        dynamic_y.push_back(make_pair(framec, dPastValue));
        pthread_mutex_unlock(&mutex1);

        // int keyboard = waitKey(1);
        // if (keyboard == 27)
        //     break;
        prvs = next;
    }
    capture.release();
}
void *processVideo(void *threadarg)
{
    struct thread_data *my_data;
    my_data = (struct thread_data *)threadarg;
    int threadNumber = my_data->thread_id;
    VideoCapture capture = my_data->capture;
    helper(threadNumber, capture);
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        cout << "Provide thread number as argument" << endl;
        return 0;
    }

    auto start = high_resolution_clock::now();

    NUM_THREADS = atoi(argv[1]);
    pthread_t threads[NUM_THREADS];
    struct thread_data td[NUM_THREADS];
    string vid_path = "../assets/trafficvideo.mp4";
    VideoCapture captures[NUM_THREADS];

    int rc;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        td[i].thread_id = i;
        VideoCapture capture(samples::findFile(vid_path));
        td[i].capture = capture;

        rc = pthread_create(&threads[i], NULL, processVideo, (void *)&td[i]);

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
         << duration.count() / 1000000.0 << " seconds" << endl;
    freopen("../outputs/timetaken.out", "w", stdout);
    cout << duration.count() / 1000000.0 << endl;
    for (int i = 0; i < dynamic_y.size(); i++)
    {
        // "first" and "second" are used to access
        // 1st and 2nd element of pair respectively
        cout << dynamic_y[i].first << " "
             << dynamic_y[i].second << endl;
    }
    sort(dynamic_y.begin(), dynamic_y.end());
    sort(queue_y.begin(), queue_y.end());
    write_out_queue(queue_y);
    write_out_dynamic(dynamic_y);
    destroyAllWindows();
    return 0;
}
