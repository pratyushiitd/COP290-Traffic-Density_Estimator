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
    string image1_path = samples::findFile("../images/empty.jpg");
    string image2_path = samples::findFile("../images/traffic.jpg");

    Mat img = imread(image1_path, IMREAD_GRAYSCALE);
    Mat img2 = imread(image2_path, IMREAD_GRAYSCALE);
    if (img.empty())
    {
        cout << "Could not read the image: " << image1_path << endl;
        return 1;
    }
    imshow("Display window", img);
    vector<Point2f> points;
    while (points.size() < 4)
    {
        setMouseCallback("Display window", CallBackFunc, &points);
        waitKey(500);
    }
    destroyWindow("Display window");
    vector<Point2f> pts_dst;
    // Compressing rectangle to the width of distant width and hight of left part
    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));

    Mat H = findHomography(points, pts_dst);

    cout << H << endl;
    Mat img_warp;
    Mat img_warp2;
    warpPerspective(img, img_warp, H, img.size());
    warpPerspective(img2, img_warp2, H, img.size());

    // warpPerspective(img, newimg, H, Size(1000, 1000));
    imshow("Perspective Changed", img_warp);
    imshow("Perspective Changed", img_warp2);
    imwrite("../MyImage.jpg", img_warp);
    waitKey(0);
    destroyWindow("Perspective Changed");
    cout << points[0].x << endl;
    //Cropping

    //      cv::Rect crop_region(int a,int b, int c, int d);
    //      a,b : Coordinates of the top-left corner.
    //      c,d : Rectangle width and height.
    Rect crop_region(points[0].x, points[0].y, points[3].x - points[0].x, points[1].y - points[0].y);
    Mat cropped = img_warp2(crop_region);
    imshow("test", cropped);
    waitKey(0);
    return 0;
}