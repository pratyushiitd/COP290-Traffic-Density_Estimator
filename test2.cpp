#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

// Global points p1, p2, p3, p4
int xc1, xc2, xc3, xc4, yc1, yc2, yc3, yc4, clickCount = 0;

void CallBackFunc(int event, int x, int y, int flags, void *params)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        vector<Point2f> *points = (vector<Point2f> *)params;
        cout << x << " " << y << endl;
        if (points->size() < 4)
        {
            points->push_back(Point2f(x, y));
        }
    }
}

int main()
{
    string image_path = samples::findFile("./book1.jpg");
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }
    imshow("Display window", img);
    vector<Point2f> points;
    setMouseCallback("Display window", CallBackFunc, &points);
    waitKey(0);
    vector<Point2f> pts_dst;

    pts_dst.push_back(Point2f(0, 0));
    pts_dst.push_back(Point2f(img.size().width - 1, 0));
    pts_dst.push_back(Point2f(img.size().width - 1, img.size().height - 1));
    pts_dst.push_back(Point2f(0, img.size().height - 1));

    Mat H = findHomography(points, pts_dst);
    cout << H << endl;
    Mat newimg;
    warpPerspective(img, newimg, H, img.size());
    imshow("test", newimg);
    waitKey(0);
    cout << points[0].x << endl;

    Mat img_draw_matches;
    hconcat(img, newimg, img_draw_matches);
    for (size_t i = 0; i < points.size(); i++)
    {
        Mat pt1 = (Mat_<double>(3, 1) << points[i].x, points[i].y, 1);
        Mat pt2 = H * pt1;
        pt2 /= pt2.at<double>(2);
        Point end((int)(img.cols + pt2.at<double>(0)), (int)pt2.at<double>(1));
        line(img_draw_matches, points[i], end, Scalar(0, 0, 255), 2);
    }
    imshow("Draw matches", img_draw_matches);
    waitKey();

    return 0;
}