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

int get_quad(const Point &p1)
{
    if (p1.x > 0 && p1.y < 0)
        return 3;
    else if (p1.x < 0 && p1.y < 0)
        return 4;
    else if (p1.x < 0 && p1.y > 0)
        return 1;
    return 2;
}

bool compare_points(const Point &a, const Point &b)
{
    return (get_quad(a) > get_quad(b));
}

int main()
{
    string image1_path = samples::findFile("../images/empty.jpg");
    string image2_path = samples::findFile("../images/traffic.jpg");

    Mat img1 = imread(image1_path, IMREAD_GRAYSCALE);
    Mat img2 = imread(image2_path, IMREAD_GRAYSCALE);
    if (img1.empty())
    {
        cout << "Could not read the image: " << image1_path << endl;
        return 1;
    }
    if (img2.empty())
    {
        cout << "Could not read the image: " << image2_path << endl;
        return 1;
    }
    imshow("Display window", img1);
    vector<Point2f> points;
    cout << "Selected points are: " << endl;
    while (points.size() < 4)
    {
        setMouseCallback("Display window", CallBackFunc, &points);
        waitKey(500);
    }

    // some function to make input points can be taken in any order

    //

    destroyWindow("Display window");
    vector<Point2f> pts_dst;
    // Compressing rectangle to the width of distant width and hight of left part
    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));
    // pts_dst.push_back(Point2f((points[0].x + points[1].x) / 2, (points[0].y + points[3].y) / 2));
    // pts_dst.push_back(Point2f((points[0].x + points[1].x) / 2, (points[1].y + points[2].y) / 2));
    // pts_dst.push_back(Point2f((points[2].x + points[3].x) / 2, (points[1].y + points[2].y) / 2));
    // pts_dst.push_back(Point2f((points[2].x + points[3].x) / 2, (points[0].y + points[3].y) / 2));

    Mat H = findHomography(points, pts_dst);

    // cout << H << endl;
    Mat img1_projected;
    Mat img2_projected;
    warpPerspective(img1, img1_projected, H, img1.size());
    warpPerspective(img2, img2_projected, H, img2.size());

    // warpPerspective(img, newimg, H, Size(1000, 1000));
    // imshow("Perspective Changed", img_warp);
    imshow("Perspective Changed", img1_projected);
    imwrite("../empty_projected.jpg", img1_projected);
    imwrite("../traffic_projected.jpg", img2_projected);
    cout << "Projected images stored \n";
    waitKey(0);
    destroyWindow("Perspective Changed");
    // cout << points[0].x << endl;
    // Cropping
    //      cv::Rect crop_region(int a,int b, int c, int d);
    //      a,b : Coordinates of the top-left corner.
    //      c,d : Rectangle width and height.
    Rect crop_region(points[0].x, points[0].y, points[3].x - points[0].x, points[1].y - points[0].y);
    Mat img1_cropped = img1_projected(crop_region);
    Mat img2_cropped = img2_projected(crop_region);
    imshow("Cropped", img1_cropped);
    imwrite("../empty_cropped.jpg", img1_cropped);
    imwrite("../traffic_cropped.jpg", img2_cropped);
    cout << "Cropped images stored \n";
    waitKey(0);
    return 0;
}
