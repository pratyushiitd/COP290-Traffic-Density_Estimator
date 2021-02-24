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
    string image_path = samples::findFile("./empty.jpg");
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
    pts_dst.push_back(Point2f(0, img.size().height - 1));
    pts_dst.push_back(Point2f(img.size().width - 1, img.size().height - 1));

    pts_dst.push_back(Point2f(img.size().width - 1, 0));

    Mat H = findHomography(points, pts_dst);
    // Normalization to ensure that ||c1|| = 1
    // double norm = sqrt(H.at<double>(0, 0) * H.at<double>(0, 0) +
    //                    H.at<double>(1, 0) * H.at<double>(1, 0) +
    //                    H.at<double>(2, 0) * H.at<double>(2, 0));
    // H /= norm;
    // Mat c1 = H.col(0);
    // Mat c2 = H.col(1);
    // Mat c3 = c1.cross(c2);
    // Mat tvec = H.col(2);
    // Mat R(3, 3, CV_64F);
    // for (int i = 0; i < 3; i++)
    // {
    //     R.at<double>(i, 0) = c1.at<double>(i, 0);
    //     R.at<double>(i, 1) = c2.at<double>(i, 0);
    //     R.at<double>(i, 2) = c3.at<double>(i, 0);
    // }
    cout << H << endl;
    Mat newimg;
    for (size_t i = 0; i < points.size(); i++)
    {
        Mat pt1 = (Mat_<double>(3, 1) << points[i].x, points[i].y, 1);
        Mat pt2 = H * pt1;
    }
    // Rect br = boundingRect(outputCorners);
    warpPerspective(img, newimg, H, img.size());
    // warpPerspective(img, newimg, H, Size(1000, 1000));
    imshow("test", newimg);
    imwrite("MyImage.jpg", newimg);
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