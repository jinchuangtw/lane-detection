#include <iostream>
#include <limits>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/video.hpp>

using namespace std;
using namespace cv;

typedef struct Myline Myline;
struct Myline
{
    int x1;
    int y1;
    int x2;
    int y2;
    double slope;
};

bool CompareBySlope(Myline& la, Myline& lb)
{
    return la.slope < lb.slope;
}

// Read the video file and its info. ---------------------------------------------
char filename[] = "../res/UE_Simu1.mp4";
VideoCapture cap(filename);
Size videoSize = Size((int)cap.get(CAP_PROP_FRAME_WIDTH), (int)cap.get(CAP_PROP_FRAME_HEIGHT));
int totalFrameNum = (int)cap.get(CAP_PROP_FRAME_COUNT);
int FPS = (int)cap.get(CAP_PROP_FPS);

Mat src(videoSize, IMREAD_COLOR);

// declaring Mat for processing --------------------------------------------------
Mat dst_1, dst_2, dst_3, dst_4;

// declaring display information -------------------------------------------------
char displayWindowName[] = "Detected Lines (in red) - Probabilistic Hough Line";

// parameter settings ------------------------------------------------------------
int ts = 0;
const int max_ts = 100;

// function prototypes -----------------------------------------------------------

// this can find the hough lines
static void AdjustedHoughLinesP(int, void*);

// this can cluster all lines into 3 stacks
vector<Myline> slopeCluster(vector<Vec4i>);

// this can draw the result lines onto the original frame and show it
void DrawLinesAndShowResult(vector<Myline>);

int main()
{
    // print out the basic info. of the input video
    cout << "Video width  = " << videoSize.width << endl;
    cout << "Video height = " << videoSize.height << endl;
    cout << "Total frames = " << totalFrameNum << endl;
    cout << "Video FPS    = " << FPS << endl;

    namedWindow(displayWindowName, WINDOW_AUTOSIZE);
    createTrackbar("Threshold", displayWindowName, &ts, max_ts, AdjustedHoughLinesP);

    // displaying the video frame by frame
    for (int i = 0; i < totalFrameNum; i++)
    {
        cap >> src;
        // put the src image into our image processing pipeline

        // Step. 1 - Gaussian blur
        int GaussianBlurParam = 11;
        GaussianBlur(src, dst_1, Size(GaussianBlurParam, GaussianBlurParam), BORDER_DEFAULT);

        // Step. 2 - Edge detection
        Canny(dst_1, dst_2, 50, 150, 3);

        // Step. 3 - Hough line detection
        AdjustedHoughLinesP(0, 0);
        waitKey(1000 / FPS);
    }
    cap.release();
    system("read -p 'Press Enter to continue...' var");
    return 0;
}

static void AdjustedHoughLinesP(int, void*)
{
    // Probabilistic Hough Line Transform
    vector<Vec4i> linesP; // will hold the results of the detected lines, with 2 points of (x, y)
    HoughLinesP(dst_2, linesP, 1, CV_PI / 180, ts, 50, 10);
    cout << "Total numbers of lines detected: " << linesP.size() << endl;
    
    vector<Myline> result = slopeCluster(linesP);

    // drawing the lines onto the original frame and show it
    DrawLinesAndShowResult(result);
}

vector<Myline> slopeCluster(vector<Vec4i> inputLines)
{
    // calculate for the slopes of the lines
    vector<Myline> lines;
    // vector<double> slopes;
    for (int i = 0; i < inputLines.size(); i++)
    {
        double dis_x = inputLines[i][0] - inputLines[i][2];
        double dis_y = inputLines[i][1] - inputLines[i][3];
        double slope = dis_x / dis_y;
        Myline l = {inputLines[i][0], inputLines[i][1], inputLines[i][2], inputLines[i][3], slope};
        lines.push_back(l);
    }

    // sorting according to the magnitude of the slope
    sort(lines.begin(), lines.end(), CompareBySlope);

    // clustering, using k-means
    int numOfClusters = 3;
    double c_init_1 = lines[0].slope;
    double c_init_2 = lines[lines.size() / 2].slope;
    double c_init_3 = lines[lines.size()].slope;
    vector<Myline> group_1, group_2, group_3, groupResult;

    for (int i = 0; i < lines.size(); i++)
    {
        double min_dis = std::numeric_limits<int>::max();
        double dis_1 = abs(lines[i].slope - c_init_1);
        double dis_2 = abs(lines[i].slope - c_init_2);
        double dis_3 = abs(lines[i].slope - c_init_3);
        min_dis = dis_1;
        min_dis = (dis_2 < min_dis) ? dis_2 : min_dis;
        min_dis = (dis_3 < min_dis) ? dis_3 : min_dis;
        if (min_dis == dis_1)
        {
            group_1.push_back(lines[i]);
            groupResult.push_back(lines[i]);
        }
        else if (min_dis == dis_2)
        {
            group_2.push_back(lines[i]);
        }
        else
        {
            group_3.push_back(lines[i]);
            groupResult.push_back(lines[i]);
        }
    }
    return groupResult;
}

void DrawLinesAndShowResult(vector<Myline> lines)
{
    for (int i = 0; i < lines.size(); i++)
    {
        line(src, Point(lines[i].x1, lines[i].y1), Point(lines[i].x2, lines[i].y2), Scalar(0, 0, 255), 3, LINE_AA);
    }
    imshow(displayWindowName, src);
}