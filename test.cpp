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

int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        cout << "Provide thread number as argument" << endl;
        return 0;
    }
    int numberOfThreads = argv[1];
    string image1_path = samples::findFile("../assets/empty.jpg");
    Mat img1 = imread(image1_path, IMREAD_GRAYSCALE);
    // UNCOMMENT FROM HERE AFTER SCRIPT IS OVER
    // namedWindow("Display window", WINDOW_NORMAL);
    // resizeWindow("Display window", 1000, 1000);
    // imshow("Display window", img1);
    vector<Point2f> points;
    // cout << "Selected points are: " << endl;
    // while (points.size() < 4)
    // {
    //     setMouseCallback("Display window", CallBackFunc, &points);
    //     waitKey(500);
    // }
    // destroyWindow("Display window");
    // points = sort_points(points);
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

    string vid_path = "../assets/trafficvideo.mp4";

    VideoCapture capture(samples::findFile(vid_path));
    capture.set(CAP_PROP_POS_FRAMES, 1000);
    Mat temp;
    capture >> temp;
    imshow("Optical Flow", temp);
    int keyboard = waitKey(0);
    return 0;
}
