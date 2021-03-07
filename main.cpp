#include <iostream>
#include "opencv2/opencv.hpp"
#include <math.h>
#include <chrono>
using namespace std;
using namespace cv;
using namespace std::chrono;
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

int main(int argc, char const *argv[])
{

    string image1_path = samples::findFile("../assets/empty.jpg");

    Mat img1 = imread(image1_path, IMREAD_GRAYSCALE);
    namedWindow("Display window", WINDOW_NORMAL);
    resizeWindow("Display window", 1000, 1000);
    imshow("Display window", img1);
    vector<Point2f> points;
    cout << "Selected points are: " << endl;
    while (points.size() < 4)
    {
        setMouseCallback("Display window", CallBackFunc, &points);
        waitKey(500);
    }
    destroyWindow("Display window");
    points = sort_points(points);
    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));

    Mat H = findHomography(points, pts_dst);
    Rect crop_region(points[0].x, points[0].y, points[3].x - points[0].x, points[1].y - points[0].y);

    // Check if camera opened successfully
    VideoCapture cap("../assets/trafficvideo.mp4");

    if (!cap.isOpened())
    {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }
    int framec = 0;
    auto start = high_resolution_clock::now();

    while (1)
    {

        Mat frame;
        framec++;
        // Capture frame-by-frame
        if (framec % 1000 == 0)
        {
            cout << framec << endl;
        }
        cap >> frame;
        if (frame.empty())
            break;
        warpPerspective(frame, frame, H, frame.size());
        frame = frame(crop_region);
        // If the frame is empty, break immediately

        // Display the resulting frame
        // imshow("Frame", frame);

        // // Press  ESC on keyboard to exit
        // char c = (char)waitKey(20);
        // if (c == 27)
        //     break;
    }
    auto stop = high_resolution_clock::now();
    // When everything done, release the video capture object
    cap.release();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    // Closes all the frames
    destroyAllWindows();
    return 0;
}
