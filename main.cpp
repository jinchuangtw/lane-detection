#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/video.hpp>
 
using namespace std;
using namespace cv;

// Read the video file and do the basic settings
char filename[] = "../res/UE_Simu1.mp4";
VideoCapture cap(filename);
Size video_size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),  (int)cap.get(CAP_PROP_FRAME_HEIGHT));
Mat src(video_size, IMREAD_COLOR); // Get the numbers of the frames
Mat src_blurred, dst, cdstP;

char displayWindowName[] = "Detected Lines (in red) - Probabilistic Line Transform";

int ts = 0;
const int max_ts = 100;

vector<Vec4i> slopeFilter(vector<Vec4i> linesP)
{
    vector<double> slopes;
    vector<Vec4i> linesFiltered;
    double slope_delta = 0.2;
    double delta_x = linesP[0][0] - linesP[0][2];
    double delta_y = linesP[0][1] - linesP[0][3];
    double slope = delta_x / delta_y;

    for (int i = 0; i < linesP.size(); i++)
    {
        double delta_x = linesP[i][0] - linesP[i][2];
        double delta_y = linesP[i][1] - linesP[i][3];
        double slope = delta_x / delta_y;

        for (int i = 0; i < slopes.size(); i++)
        {        
            double slope_up = slope + slope_delta;
            double slope_low = slope - slope_delta;
            if (slope > slope_up || slope < slope_low)
            {
                slopes.push_back(slope);
            }
        }
    }
}

vector<Vec4i> lineFilter(vector<Vec4i> linesP)
{
    vector<Vec4i> linesFiltered;
    double slope_min = 999, slope_max = -999;
    int slope_min_idx = 0, slope_max_idx = 0;

    for (int i = 0; i < linesP.size(); i++)
    {
        double delta_x = linesP[i][0] - linesP[i][2];
        double delta_y = linesP[i][1] - linesP[i][3];
        double slope = delta_x / delta_y;
        if (slope_min > slope)
        {
            slope_min = slope;
            slope_min_idx = i;
        }
        if (slope_max < slope)
        {
            slope_max = slope;
            slope_max_idx = i;
        }
    }
    linesFiltered.push_back(linesP[slope_min_idx]);
    linesFiltered.push_back(linesP[slope_max_idx]);
    return linesFiltered;
}

static void HoughLine(int, void*)
{
    // Probabilistic Line Transform
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI / 180, ts, 50, 10); // runs the actual detection
    cout << "Lines detected: " << linesP.size() << endl;
    vector<Vec4i> lf = lineFilter(linesP);
    // Draw the lines
    for(size_t i = 0; i < lf.size(); i++)
    {
        Vec4i l = lf[i];
        line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
    }
    imshow(displayWindowName, cdstP);
}

int main()
{
    cout << "Video width = " << video_size.width << endl;
    cout << "Video height = " <<  video_size.height << endl;

    // Frame operation
    int number_of_frames = (int)cap.get(CAP_PROP_FRAME_COUNT);
    cout << "Total frames = " << number_of_frames << endl;
    int FPS = (int)cap.get(CAP_PROP_FPS); // Get FPS
    cout << "Video FPS = " << FPS << endl;

    namedWindow(displayWindowName, WINDOW_AUTOSIZE);
    createTrackbar("Threshold", displayWindowName, &ts, max_ts, HoughLine);

    // Display the video frame by frame
    for (int i = 0; i < number_of_frames; i++)
    {
        cap >> src;
        GaussianBlur(src, src_blurred, Size(11, 11), BORDER_DEFAULT);

        // Edge detection
        Canny(src_blurred, dst, 250, 200, 3);

        // Copy edges to the images that will display the results in BGR
        cvtColor(dst, cdstP, COLOR_GRAY2BGR);

        // Show results
        HoughLine(0, 0);
        waitKey(1000 / FPS);
    }
    cap.release();
    system("read -p 'Press Enter to continue...' var");
    return 0;
}