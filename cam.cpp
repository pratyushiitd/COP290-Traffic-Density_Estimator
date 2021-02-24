#include <iostream>
#include "opencv2/opencv.hpp"
#include <math.h>
using namespace std;
using namespace cv;

// Global points p1, p2, p3, p4
int xc1, xc2, xc3, xc4, yc1, yc2, yc3, yc4, clickCount = 0;

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

    destroyWindow("Display window");
    points = sort_points(points);
    vector<Point2f> pts_dst;
    // Compressing rectangle to the width of distant width and hight of left part

    // Forced Compression
    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));

    // Average Compression
    // pts_dst.push_back(Point2f((points[0].x + points[1].x) / 2, (points[0].y + points[3].y) / 2));
    // pts_dst.push_back(Point2f((points[0].x + points[1].x) / 2, (points[1].y + points[2].y) / 2));
    // pts_dst.push_back(Point2f((points[2].x + points[3].x) / 2, (points[1].y + points[2].y) / 2));
    // pts_dst.push_back(Point2f((points[2].x + points[3].x) / 2, (points[0].y + points[3].y) / 2));

    // Root mean square
    // pts_dst.push_back(Point2f(sqrt(pow(points[0].x, 2) + pow(points[1].x, 2)) / 2, sqrt(pow(points[0].y, 2) + pow(points[3].y, 2)) / 2));
    // pts_dst.push_back(Point2f(sqrt(pow(points[0].x, 2) + pow(points[1].x, 2)) / 2, sqrt(pow(points[1].y, 2) + pow(points[2].y, 2)) / 2));
    // pts_dst.push_back(Point2f(sqrt(pow(points[2].x, 2) + pow(points[3].x, 2)) / 2, sqrt(pow(points[1].y, 2) + pow(points[2].y, 2)) / 2));
    // pts_dst.push_back(Point2f(sqrt(pow(points[2].x, 2) + pow(points[3].x, 2)) / 2, sqrt(pow(points[0].y, 2) + pow(points[3].y, 2)) / 2));

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
    cout << "Projected images stored as: empty_projected.jpg and traffic_projected.jpg\n";
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
    cout << "Cropped images stored as : empty_cropped.jpg and traffic_cropped.jpg\n";
    waitKey(0);
    return 0;
}