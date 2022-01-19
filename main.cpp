#include <iostream>
#include <string>
#include <cmath>
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

int main()
{
    cout << "Video width = " << video_size.width << endl;
    cout << "Video height = " <<  video_size.height << endl;

    // Frame operation
    int number_of_frames = (int)cap.get(CAP_PROP_FRAME_COUNT);
    cout << "Total frames = " << number_of_frames << endl;
    int FPS = (int)cap.get(CAP_PROP_FPS); // Get FPS
    cout << "Video FPS = " << FPS << endl;

    namedWindow("Video", WINDOW_AUTOSIZE);

    // Display the video frame by frame
    for (int i = 0; i < number_of_frames; i++) {
        cap >> src;

        Mat src_blurred;
        GaussianBlur(src, src_blurred, Size(11, 11), BORDER_DEFAULT);

        // Edge detection
        Mat dst, cdst, cdstP;
        Canny(src_blurred, dst, 250, 200, 3);

        // Copy edges to the images that will display the results in BGR
        cvtColor(dst, cdst, COLOR_GRAY2BGR);
        cdstP = cdst.clone();

        // Standard Hough Line Transform
        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(dst, lines, 1, CV_PI / 180, 150, 0, 0); // runs the actual detection

        // Draw the lines
        for(size_t i = 0; i < lines.size(); i++)
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        }

        // Probabilistic Line Transform
        vector<Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(dst, linesP, 1, CV_PI / 180, 50, 50, 10); // runs the actual detection

        // Draw the lines
        for(size_t i = 0; i < linesP.size(); i++)
        {
            Vec4i l = linesP[i];
            line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
        }
        // Show results
        imshow("Source", src);
        imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
        imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);

        waitKey(1000 / FPS);
    }
    cap.release();
    system("read -p 'Press Enter to continue...' var");
    return 0;
}