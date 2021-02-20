#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_RBUTTONDOWN)
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MBUTTONDOWN)
    {
        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MOUSEMOVE)
    {
        cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
    }
}

int main()
{
    string image_path = samples::findFile("./ani.jpeg");
    Mat img = imread(image_path, IMREAD_COLOR);
    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }
    imshow("Display window", img);
    setMouseCallback("Display window", CallBackFunc, NULL);
    int k = waitKey(0); // Wait for a keystroke in the window
    if (k == 's')
    {
        imwrite("./ani.png", img);
    }
    return 0;
}