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
    Mat img = imread(image_path, IMREAD_GRAYSCALE);
    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }
    imshow("Display window", img);
    vector<Point2f> points;
    setMouseCallback("Display window", CallBackFunc, &points);
    waitKey(0);
    destroyWindow("Display window");
    vector<Point2f> pts_dst;

    pts_dst.push_back(Point2f(0, 0));
    pts_dst.push_back(Point2f(0, img.size().height - 1));
    pts_dst.push_back(Point2f(img.size().width - 1, img.size().height - 1));
    pts_dst.push_back(Point2f(img.size().width - 1, 0));

    Mat H = findHomography(points, pts_dst);

    cout << H << endl;
    Mat img_warp;
    
    // Rect br = boundingRect(outputCorners);
    warpPerspective(img, img_warp, H, img.size());
    // warpPerspective(img, newimg, H, Size(1000, 1000));
    imshow("Perspective Changed", newimg);
    imwrite("MyImage.jpg", newimg);
    waitKey(0);
    destroyWindow("Perspective Changed");
    cout << points[0].x << endl;
    //Cropping
    

    return 0;
}