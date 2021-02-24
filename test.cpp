// #include <opencv2/core.hpp>
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui.hpp>
// #include <iostream>
// using namespace std;
// using namespace cv;

// // Global points p1, p2, p3, p4
// int xc1, xc2, xc3, xc4, yc1, yc2, yc3, yc4, clickCount = 0;

// void CallBackFunc(int event, int x, int y, int flags, void *userdata)
// {
//     cout << x << " " << y << endl;
//     return;
//     if (event == EVENT_LBUTTONDOWN)
//     {
//         cout << x << " " << y << endl;
//         clickCount++;
//         if (clickCount == 1)
//         {
//             xc1 = x;
//             yc1 = y;
//         }
//         if (clickCount == 2)
//         {
//             xc2 = x;
//             yc2 = y;
//         }
//         if (clickCount == 3)
//         {
//             xc3 = x;
//             yc3 = y;
//         }
//         if (clickCount == 4)
//         {
//             xc4 = x;
//             yc4 = y;
//         }
//         else
//         {
//             cout << xc1 << x << endl;

//             return;
//         }
//     }
// }

// int main()
// {
//     string image_path = samples::findFile("./ani.jpeg");
//     Mat img = imread(image_path, IMREAD_COLOR);
//     if (img.empty())
//     {
//         cout << "Could not read the image: " << image_path << endl;
//         return 1;
//     }
//     imshow("Display window", img);
//     clickCount = 0;
//     setMouseCallback("Display window", CallBackFunc, NULL);
//     cout << xc1 << xc2 << xc3 << xc4;
//     xc1 = 50, yc1 = 50;
//     xc2 = 0, yc2 = 180;
//     xc3 = 250, yc3 = 150;
//     xc4 = 200, yc4 = 100;
//     int k = waitKey(0); // Wait for a keystroke in the window
//     if (k == 's')
//     {
//         imwrite("./ani.png", img);
//     }

//     return 0;
// }

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct userdata
{
    Mat im;
    vector<Point2f> points;
};

void mouseHandler(int event, int x, int y, int flags, void *data_ptr)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        userdata *data = ((userdata *)data_ptr);
        circle(data->im, Point(x, y), 3, Scalar(0, 0, 255), 5, cv::LINE_AA);
        imshow("Image", data->im);
        if (data->points.size() < 4)
        {
            data->points.push_back(Point2f(x, y));
        }
    }
}

int main(int argc, char **argv)
{

    // Read source image.
    Mat im_src = imread("./empty.jpg");

    // Destination image. The aspect ratio of the book is 3/4
    Size size(300, 400);
    Mat im_dst = Mat::zeros(size, CV_8UC3);

    // Create a vector of destination points.
    vector<Point2f> pts_dst;

    pts_dst.push_back(Point2f(0, 0));
    pts_dst.push_back(Point2f(size.width - 1, 0));
    pts_dst.push_back(Point2f(size.width - 1, size.height - 1));
    pts_dst.push_back(Point2f(0, size.height - 1));

    // Set data for mouse event
    Mat im_temp = im_src.clone();
    userdata data;
    data.im = im_temp;

    cout << "Click on the four corners of the book -- top left first and" << endl
         << "bottom left last -- and then hit ENTER" << endl;

    // Show image and wait for 4 clicks.
    imshow("Image", im_temp);
    // Set the callback function for any mouse event
    setMouseCallback("Image", mouseHandler, &data);
    waitKey(0);

    // Calculate the homography
    Mat h = findHomography(data.points, pts_dst);

    // Warp source image to destination
    warpPerspective(im_src, im_dst, h, size);

    // Show image
    imshow("Image", im_dst);
    waitKey(0);

    return 0;
}