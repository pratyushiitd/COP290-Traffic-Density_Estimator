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

    string vid_path = "../assets/mock.mp4";

    VideoCapture capture(samples::findFile(vid_path));
    int noOfFrames = capture.get(CAP_PROP_FRAME_COUNT);

    if (!capture.isOpened())
    {
        //error in opening the video input
        cout << "Could not open file: " << vid_path << endl;
        return 0;
    }
    Ptr<BackgroundSubtractor> obj_back;                       //create Background Subtractor object
    obj_back = createBackgroundSubtractorMOG2(0, 350, false); // (History, threshold, detech_shadows = false)
    //obj_back = createBackgroundSubtractorKNN();

    int framec = 0;

    Mat frame, fgMask, prvs;
    Mat frame1;
    vector<int> queue_y;
    vector<int> dynamic_y;
    freopen("../outputs/frames.out", "w", stdout);

    capture >> frame1;
    warpPerspective(frame1, frame1, H, frame.size());
    frame1 = frame1(crop_region);
    cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    warpPerspective(img1, img1, H, img1.size());
    frame1 = img1;
    // frame = img1(crop_region);
    // obj_back->apply(frame, frame, 0);
    double fps = capture.get(CAP_PROP_FPS);
    // VideoWriter videoout("outcpp.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, frame.size());

    auto start = high_resolution_clock::now();
    while (true)
    {
        // DEBUGGER

        if (framec == 200000)
        {
            break;
        }
        //

        framec++;
        if (framec < capture.get(CAP_PROP_FRAME_COUNT) - 1)
        {
            cout << framec << ",";
        }
        else if (framec == capture.get(CAP_PROP_FRAME_COUNT) - 1)
        {
            cout << framec;
        }

        // Capture frame-by-frame
        // if (framec % 1000 == 0){cout << framec << endl;}
        capture.read(frame);
        if (frame.empty())
            break;

        warpPerspective(frame, frame, H, frame.size());
        frame = frame(crop_region);
        // Display the resulting frame
        // imshow("Frame", frame);
        fgMask = img1;
        obj_back->apply(frame, fgMask, 0); //Learning rate set to 0

        //====================================================================================
        //Display white ratio in white box on top left corner for masked frames
        rectangle(frame, cv::Point(10, 2), cv::Point(100, 20), cv::Scalar(255, 255, 255), -1); //Display white ratio on top left corner
        stringstream ss;
        //ss << capture.get(CAP_PROP_POS_FRAMES); //0-based index of the frame to be decoded/captured next.
        vector<Point> white_pixels;
        cv::findNonZero(fgMask, white_pixels);
        //ss << capture.get(CAP_PROP_POS_FRAMES);
        int white_ratio = ((white_pixels.size()) * 1000.0) / (fgMask.cols * fgMask.rows);
        ss << white_ratio;
        queue_y.push_back(white_ratio);
        white_pixels.clear();
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(45, 15),
                FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

        //==================================================================================
        //Optical Flow Evaluation
        Mat next;
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
        normalize(magnitude, temp, 0.0f, 1.0f, NORM_MINMAX);
        threshold(temp, magn_norm, 0.4f, 1.0f, THRESH_BINARY);
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
        threshold(gr_bt, gry, 5, 255, THRESH_BINARY);
        //imshow("Optical Flow", hsv8);

        //====================================================================================
        //Display white ratio in white box on top left corner for optical flow frame
        vector<Point> white_pixels_dyn;
        cv::findNonZero(gry, white_pixels_dyn);

        int white_ratio_dyn = ((white_pixels_dyn.size()) * 1000.0) / (gry.cols * gry.rows);
        stringstream sst;
        sst << white_ratio_dyn;
        dynamic_y.push_back(white_ratio_dyn);
        white_pixels_dyn.clear();
        rectangle(frame, cv::Point(10, 30), cv::Point(100, 48), cv::Scalar(255, 255, 255), -1); //Display white ratio on top left corner

        frameNumberString = sst.str();
        putText(frame, frameNumberString.c_str(), cv::Point(45, 43),
                FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        //=====================================================================================

        imshow("Optical Flow", gry);
        imshow("Original Frame", frame);
        imshow("Foreground Mask", fgMask);
        // videoout.write(frame);

        int keyboard = waitKey(1);
        if (keyboard == 27)
            break;
        prvs = next;
    }
    auto stop = high_resolution_clock::now();
    // When everything done, release the video capture object
    capture.release();
    auto duration = duration_cast<microseconds>(stop - start);
    // cout << "Time taken by function: "
    //      << duration.count() / 1000000.0 << " seconds" << endl;
    // Closes all the frames

    //Now we have vector<int> queue_y and vector<int> dynamic_y, Plot the coordinates.
    freopen("../outputs/static.out", "w", stdout);
    for (int i = 0; i < queue_y.size(); i++)
    {
        if (i == queue_y.size() - 1)
        {
            cout << queue_y[i] / 1000.0;
            break;
        }

        cout << queue_y[i] / 1000.0 << ",";
    }
    freopen("../outputs/dynamic.out", "w", stdout);
    for (int i = 0; i < dynamic_y.size(); i++)
    {
        if (i == dynamic_y.size() - 1)
        {
            cout << dynamic_y[i] / 1000.0;
            break;
        }
        cout << dynamic_y[i] / 1000.0 << ",";
    }

    destroyAllWindows();
    return 0;
}