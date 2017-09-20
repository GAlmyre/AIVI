/*

  //OpenCV documentation is available here: http://docs.opencv.org/2.4.9/

  //1.1a - Display & save individual frames of the video as RGB
  // You will need to use classes & methods from the highgui module:
  // VideoCapture, imwrite, imshow, ...
  // You have an example of use of VideoCapture here:
  // http://docs.opencv.org/2.4.9/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture

  //1.1b - Display & save individual frames of the video as Y from YCrCb
  // You will need to use:
  //  cvtColor from the imgproc module
  //  split from the core module

  //1.2 - Compute the measures: MSE, PSNR, Entropy, Error Image
  // You will need to code these measures in utils.hpp/utils.cpp

  //1.3 - Trace the curves with gnuplot
  // You have examples of gnuplot commands are available in files example*.gplot

*/

#include <cstdlib>
#include <iostream>
#include "utils.hpp"
#include <opencv2/highgui/highgui.hpp> //VideoCapture, imshow, imwrite, ...
#include <opencv2/imgproc/imgproc.hpp> //cvtColor

int main(int argc, char **argv)
{
  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << " video-filename distance-between-two-frames-for-prediction" << std::endl;
    return EXIT_FAILURE;
  }

  const char *videoFilename = argv[1];

  const int interFramesDistance = atoi(argv[2]);
  if (interFramesDistance <= 0) {
    std::cerr<<"Error: distance-between-two-frames-for-prediction must be a strictly positive integer"<<std::endl;
    return EXIT_FAILURE;
  }

  cv::VideoCapture cap;
  cap.open(videoFilename);
  if ( ! cap.isOpened()) {
    std::cerr << "Error : Unable to open video file " << argv[1] << std::endl;
    return -1;
  }

  unsigned long frameNumber = 0;

  cv::namedWindow("MainWindow", 1);
  cv::Mat previousFrame;

  for ( ; ; ) {
    cv::Mat frameBGR;
    cv::Mat frameGray;

    cap >> frameBGR;
    if (frameBGR.empty()) {
      break;
    }

    cv::cvtColor(frameBGR, frameGray, CV_BGR2GRAY);

    if (frameNumber != 0) {
      std::cout << "MSE : " << computeMSE(frameGray, previousFrame) << std::endl;
    }

    cv::imshow("MainWindow", frameGray);

    //cv::imwrite("SavedFrame"+std::to_string(frameNumber)+".png", frameGray);

    cv::waitKey(1);
    ++frameNumber;
    previousFrame = frameGray;
  }

  return EXIT_SUCCESS;
}
