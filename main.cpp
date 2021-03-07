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
//#include "matplotlibcpp.h"

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

// int calcPercentage(Mat msk){
// 	int height = msk::rows;
//     int width = msk::cols;
// 	int num_pixels = height * width;
// 	int count_white = cv2::countNonZero(msk);
// 	int percent_white = (count_white*100/num_pixels);

// 	return percent_white;
// }

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
    string image1_path = samples::findFile("empty.jpg");

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

    string vid_path = "trafficvideo.mp4";

    VideoCapture capture(samples::findFile(vid_path));

    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Could not open file: " << vid_path << endl;
        return 0;
    }
    Ptr<BackgroundSubtractor> obj_back; //create Background Subtractor object

    obj_back = createBackgroundSubtractorMOG2(1, 100.0, false); // (History, threshold, detech_shadows = false)
    //obj_back = createBackgroundSubtractorKNN();

    int framec = 0;

    Mat frame, frame1, fgMask, prvs;
    capture >> frame1;
    warpPerspective(frame1, frame1, H, frame.size());
    frame1 = frame1(crop_region);
    cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    vector<double> queue_y;
    warpPerspective(img1, img1, H, img1.size());
    frame = img1(crop_region);
    auto start = high_resolution_clock::now();
    while (true)
    {
        framec++;
        // Capture frame-by-frame
        // if (framec % 1000 == 0)
        // {
        //     cout << framec << endl;
        // }
        capture >> frame;
        if (frame.empty())
            break;
        warpPerspective(frame, frame, H, frame.size());
        frame = frame(crop_region);
        // Display the resulting frame
        // imshow("Frame", frame);

        // capture >> frame;
        // if (frame.empty()){
        //     break;
        // }
        obj_back->apply(frame, fgMask, 0); //Learning rate set to 0

        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20), cv::Scalar(255, 255, 255), -1); //Display white ratio on top left corner
        stringstream ss;
        //ss << capture.get(CAP_PROP_POS_FRAMES); //0-based index of the frame to be decoded/captured next.
        vector<Point> white_pixels;
        cv::findNonZero(fgMask, white_pixels);
        //ss << capture.get(CAP_PROP_POS_FRAMES);
        int white_ratio = ((white_pixels.size()) * 1000) / (fgMask.cols * fgMask.rows);
        ss << white_ratio;
        queue_y.push_back(white_ratio);
        white_pixels.clear();
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(45, 15),
                FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        // cv::Mat diff;
        // cv::absdiff(frame, img1, diff);

        imshow("Original Frame", frame);
        imshow("Foreground Mask", fgMask);

        //Mat frame2, next;
        Mat next;
        // capture >> frame2;
        // if (frame2.empty())
        //     break;
        cvtColor(frame, next, COLOR_BGR2GRAY);
        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        // visualization
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        imshow("Optical Flow", bgr);
        //imshow("Optical Flow", hsv8);

        int keyboard = waitKey(30);
        if (keyboard == 27)
            break;
    }
    auto stop = high_resolution_clock::now();
    // When everything done, release the video capture object
    capture.release();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: "
         << duration.count() / 1000000.0 << " seconds" << endl;
    // Closes all the frames
    destroyAllWindows();
    return 0;
}