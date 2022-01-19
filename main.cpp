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
 
// Declare
Mat grey_frame;
Mat blurred_frame;
Mat mask;
Mat edge_frame;
Rect2d rois;
const char* window_name = "Video";

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int kernel_size = 3;

static void Canny_threshold(int ,void*) {
    Canny(mask, edge_frame, lowThreshold, lowThreshold * 3, kernel_size);
    imshow(window_name, edge_frame);
}


int  main() {
    cout << "Video width = " << video_size.width << endl;
    cout << "Video height = " <<  video_size.height << endl;

    // Frame operation
    int number_of_frames = (int)cap.get(CAP_PROP_FRAME_COUNT);
    cout << "Total frames = " << number_of_frames << endl;
    int FPS = (int)cap.get(CAP_PROP_FPS); // Get FPS
    cout << "Video FPS = " << FPS << endl;

    namedWindow(window_name, WINDOW_AUTOSIZE);
    // createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, Canny_threshold );

    // Display the video frame by frame
    for (int i = 0; i < number_of_frames; i++) {
        cap >> src;
        // Edge detection
        Mat dst, cdst, cdstP;
        Canny(src, dst, 250, 200, 3);
        // Copy edges to the images that will display the results in BGR
        cvtColor(dst, cdst, COLOR_GRAY2BGR);
        cdstP = cdst.clone();
        // Standard Hough Line Transform
        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(dst, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( cdst, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
        }
        // Probabilistic Line Transform
        vector<Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(dst, linesP, 1, CV_PI/180, 50, 50, 10 ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        }
        // Show results
        imshow("Source", src);
        imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
        imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
        // cvtColor(src, grey_frame, COLOR_RGB2GRAY); // Turn the source frame into greyscale
        // GaussianBlur(grey_frame, blurred_frame, Size(3, 3), 0);
        // mask = Mat::zeros(grey_frame.size(),CV_8UC1);
        // if (i == 0) {
        //     imshow(window_name, grey_frame);
        //     rois = selectROI(grey_frame);
        // }
        // else { }
        // Mat srcROI = grey_frame(rois);
        // imshow("test", srcROI);
        // srcROI.copyTo(mask);
        // Canny_threshold(0, 0);
        // imwrite(to_string(i) + ".png", src);
        waitKey(1000 / FPS);
    }
    cap.release();
    system("read -p 'Press Enter to continue...' var");
    return 0;
}