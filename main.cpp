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
#include <cstdlib>
#include <pthread.h>

using namespace std;
using namespace cv;
using namespace std::chrono;

vector<pair<int, int>> queue_y;
vector<pair<int, int>> dynamic_y;
int processf = 5;

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
int display_whiteratio_dynamic(Mat gry, Mat &frame){
    vector<Point> white_pixels_dyn;
    cv::findNonZero(gry, white_pixels_dyn);
    int white_ratio_dyn = ((white_pixels_dyn.size()) * 1000.0) / (gry.cols * gry.rows);
    stringstream sst;
    sst << white_ratio_dyn;
    return white_ratio_dyn;
}
int display_whiteratio_queue(Mat fgMask, Mat frame){
    //rectangle(frame, cv::Point(10, 2), cv::Point(100, 20), cv::Scalar(255, 255, 255), -1); //Display white ratio on top left corner
    stringstream ss;
    vector<Point> white_pixels;
    cv::findNonZero(fgMask, white_pixels);
    //ss << capture.get(CAP_PROP_POS_FRAMES);
    int white_ratio = ((white_pixels.size()) * 1000.0) / (fgMask.cols * fgMask.rows);
    ss << white_ratio;
    return white_ratio;
}
void write_out_queue(vector<pair<int, int>> queue_y, int NUM_THREADS){
    freopen("../outputs/queue.out", "w", stdout);
    //cout << "Hello" << endl;
    int arr_sz = queue_y.size() / NUM_THREADS;
    //cout << "Size is : " << arr_sz << endl;
    int arr[arr_sz+1] = {0};
    for (int i = 0; i < queue_y.size(); i++)
    {
        //cout << queue_y[i].first << "::" << queue_y[i].second << endl;
        arr[queue_y[i].first]+=queue_y[i].second;
    }
    for (int i = 1; i <= arr_sz; i++)
    {
        if (i == arr_sz)
        {
            cout << arr[i] / (NUM_THREADS * 1000.0);
            break;
        }

        cout << arr[i] / (NUM_THREADS * 1000.0) << ",";
    }
}
void write_out_dynamic(vector<pair<int, int>> dynamic_y, int NUM_THREADS){
    freopen("../outputs/dynamic.out", "w", stdout);
    //cout << "Hello1" << endl;
    int arr_sz = dynamic_y.size() / NUM_THREADS;
    //cout << "Size is : " << arr_sz << endl;
    int arr[arr_sz+1] = {0};
    for (int i = 0; i < dynamic_y.size(); i++)
    {
        //cout << dynamic_y[i].first << "::" << dynamic_y[i].second << endl;
        arr[dynamic_y[i].first]+=dynamic_y[i].second;
        //cout << "arr " << arr[dynamic_y[i].first] << endl;
    }
    for (int i = 1; i <= arr_sz; i++)
    {
        if (i == arr_sz)
        {
            cout << arr[i] / (NUM_THREADS * 1000.0);
            break;
        }

        cout << arr[i] / (NUM_THREADS * 1000.0) << ",";
    }
}
Mat evaluate_dense_opticalflow(Mat &next, Mat &prvs, Mat frame){
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
    normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    //threshold(temp, magn_norm, 0.4f, 1.0f, THRESH_BINARY);
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
    threshold(gr_bt, gry, 15, 255, THRESH_BINARY);
    return gry;
}
Mat evaluate_lucas_kanade_opticalflow(Mat &frame, vector<Point2f> &p0,vector<Point2f> &p1, vector<Point2f> &good_new, Mat &mask, Mat &old_gray, Mat &frame_gray, vector<Scalar> colors, vector<int> &sparse){
    //Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    vector<uchar> status;
    vector<float> err;
    TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
    //vector<Point2f> good_new;
    //We pass the previous frame, previous points and next frame. 
    //It returns next points along with some status numbers which 
    //has a value of 1 if next point is found, else zero
    double euclid = 0.0;
    for(uint i = 0; i < p0.size(); i++)
    {
        // Select good points
        if(status[i] == 1) {
            //status = 1 implies the the point is found
            good_new.push_back(p1[i]);
            // draw the tracks
            //cout <<< p1[i].x << " " << p1[i].y << " " << p0[i].x << " " << p0[i].y << endl;
            euclid+= sqrt((p1[i].x - p0[i].x) * (p1[i].x - p0[i].x) + (p1[i].y - p0[i].y) * (p1[i].y - p0[i].y)) ;
            line(mask,p1[i], p0[i], colors[i], 2);
            circle(frame, p1[i], 5, colors[i], -1);
        }
    }
    sparse.push_back(euclid / 10);
    Mat img;
    add(frame, mask, img);
    return img;
}

struct thread_data {
   Rect crop_region;
   Mat homo;
};

void *process_splitted_frame(void *threadarg)
{
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    Rect crop_region = my_data->crop_region;
    Mat H = my_data->homo;
    string image1_path = samples::findFile("../assets/empty.jpg");
    Mat img1 = imread(image1_path, IMREAD_GRAYSCALE);
    //Rect crop_region(points[0].x, points[0].y, points[3].x - points[0].x, points[1].y - points[0].y);
    string vid_path = "../assets/trafficvideo.mp4";

    VideoCapture capture(samples::findFile(vid_path));
    int noOfFrames = capture.get(CAP_PROP_FRAME_COUNT);

    if (!capture.isOpened())
    {
        cout << "Could not open file: " << vid_path << endl;
        return 0;
    }
    Ptr<BackgroundSubtractor> obj_back;
    obj_back = createBackgroundSubtractorMOG2(1, 350, false);

    int framec = 0;

    Mat frame, fgMask, prvs, frame0, old_gray;
    vector<int> sparse_y;
    vector<Point2f> p0, p1;
    // freopen("../outputs/frames.out", "w", stdout);

    capture >> frame0;
    warpPerspective(frame0, frame0, H, frame.size());
    Mat frame1 = frame0(crop_region);
    cvtColor(frame1, prvs, COLOR_BGR2GRAY);
    warpPerspective(img1, img1, H, img1.size());
    // frame1 = img1;
    frame = img1(crop_region);
    //obj_back->apply(frame, fgMask, 1);
    double fps = capture.get(CAP_PROP_FPS);
    // VideoWriter videoout("outcpp.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, frame.size());
    double timeOfVid = noOfFrames / fps;
    // cout << timeOfVid << endl;
    //======================
    while (true)
    {
        capture.read(frame);
        if (frame.empty()) break;
        framec++;
        if (framec % processf != 0) continue;
        if (framec % 100 == 0) cout << framec << endl;
        //if (framec > 200) break;
        warpPerspective(frame, frame, H, frame.size());
        frame = frame(crop_region);
        obj_back->apply(frame, fgMask, 0); //Learning rate set to 0
        //==================================================================================
        //Display white ratio in white box on top left corner for masked frames
        //pthread_mutex_lock(&mutex1);
        int wr_queue = display_whiteratio_queue(fgMask, frame);
        queue_y.push_back(make_pair(framec / processf, wr_queue));
        //pthread_mutex_unlock(&mutex1);
        //Optical Flow Evaluation
        Mat next;
        Mat gry = evaluate_dense_opticalflow(next, prvs, frame);
        //Display white ratio in white box on top left corner for optical flow frame
        //pthread_mutex_lock(&mutex1);
        int rw_dynam = display_whiteratio_dynamic(gry, frame);
        dynamic_y.push_back(make_pair(framec / processf, rw_dynam));
        //pthread_mutex_unlock(&mutex1);
        // imshow("Optical Flow", gry);
        // imshow("Original Frame", frame);
        // imshow("Foreground Mask", fgMask);
        // int keyboard = waitKey(1);
        // if (keyboard == 27)
        //     break;
        //cout << queue_y.size() << " " << dynamic_y.size() << endl;
        prvs = next;
    }
    capture.release();
    pthread_exit(NULL);
}


int main(int argc, char const *argv[]){
    int NUM_THREADS;
    if (argc < 2){
        NUM_THREADS = 6;
    }
    else NUM_THREADS = atoi(argv[1]);

    vector<Point2f> points;
    auto start = high_resolution_clock::now();
    points.push_back(Point2f(948, 270));
    points.push_back(Point2f(205, 1062));
    points.push_back(Point2f(1551, 1064));
    points.push_back(Point2f(1296, 249));

    vector<Point2f> pts_dst;

    pts_dst.push_back(Point2f(points[0].x, points[0].y));
    pts_dst.push_back(Point2f(points[0].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[1].y));
    pts_dst.push_back(Point2f(points[3].x, points[0].y));

    Mat H = findHomography(points, pts_dst);
    int height_crop = (( points[1].y - points[0].y ) / NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    struct thread_data td[NUM_THREADS];
    for(int it = 0; it < NUM_THREADS; it++ ) {
        Rect splitted_crop_region(points[0].x, (points[0].y + height_crop * it), (points[3].x - points[0].x), height_crop);
        td[it].crop_region = splitted_crop_region;
        td[it].homo = H;
        cout << "main(): Creating thread " << it+1 << endl;
        int rec_thread = pthread_create(&threads[it], NULL, process_splitted_frame , (void *)&td[it]);
        if (!rec_thread) {
            cout << "Successfully created thread" << it+1 << endl;
        }
        else{
            cout << "Error in creating thread: " << rec_thread << endl;
            exit(-1);
        }
    }
    for(int it = 0; it < NUM_THREADS; it++) {
        pthread_join(threads[it], NULL);
        cout << "Successfully joined thread: " << it+1 << endl;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function with " << NUM_THREADS << " threads is: "
         << duration.count() / 1000000.0 << " seconds" << endl;
    freopen("../outputs/timetaken.out", "w", stdout);
    cout << duration.count() / 1000000.0 << endl;
    //cout << queue_y.size() << " " << dynamic_y.size() << endl;
    write_out_queue(queue_y, NUM_THREADS);
    write_out_dynamic(dynamic_y, NUM_THREADS);
    //destroyAllWindows();
    return 0;
}
