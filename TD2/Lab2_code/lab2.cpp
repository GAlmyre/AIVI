/*

  //OpenCV documentation is available here: http://docs.opencv.org/2.4.9/

  //Block matching in mono and multi-resolution

*/

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <queue>
#include <sstream>


#include <opencv2/highgui/highgui.hpp> //VideoCapture, imshow, imwrite, ...
#include <opencv2/imgproc/imgproc.hpp> //cvtColor

#include "utils.hpp"
#include "blockmatching.hpp"

int
main(int argc, char **argv)
{
  if(argc != 4) {
    std::cerr << "Usage: " << argv[0] << " video-filename distance-between-two-frames-for-prediction nbLevels" << std::endl;
    return EXIT_FAILURE;
  }

  const char *videoFilename = argv[1];

  const int interFramesDistance = atoi(argv[2]);
  if (interFramesDistance <= 0) {
    std::cerr<<"Error: distance-between-two-frames-for-prediction must be a strictly positive integer"<<std::endl;
    return EXIT_FAILURE;
  }

  const int nbLevels = atoi(argv[3]);
  if (nbLevels <= 0 || nbLevels>4) {
    std::cerr<<"Error: nbLevels must be a strictly positive integer"<<std::endl;
    return EXIT_FAILURE;
  }

  const int blockSize = 8;
  const int windowSize = 32;
  //TODO: it would be better to pass these values as parameters

  cv::VideoCapture cap;
  cap.open(videoFilename);
  if ( ! cap.isOpened()) {
    std::cerr << "Error : Unable to open video file " << argv[1] << std::endl;
    return -1;
  }

  unsigned long frameNumber = 0;

  const size_t deltaT = interFramesDistance;
  std::queue<cv::Mat> previousFrames;


  for ( ; ; ) {

    cv::Mat frameBGR;
    cap >> frameBGR;
    if (frameBGR.empty()) {
      break;
    }

    //save frame
    std::stringstream ss;
    ss<<"frame_"<<std::setfill('0')<<std::setw(6)<<frameNumber<<".png";
    cv::imwrite(ss.str(), frameBGR);

    //convert from BGR to Y
    cv::Mat frameY;
    cv::Mat errorFrame;
    cv::cvtColor(frameBGR, frameY, CV_BGR2GRAY);

    if (previousFrames.size() >= deltaT) {
      cv::Mat prevY = previousFrames.front();
      previousFrames.pop();

      double MSE, PSNR, ENT, ENTe;

      if (nbLevels == 1) {

      	cv::Mat motionVectors;
      	blockMatchingMono(frameY, prevY, blockSize, windowSize, motionVectors);
        cv::Mat frameYCopy = frameY.clone();
        drawMVi(frameYCopy, motionVectors);
      	cv::Mat YC;
      	computeCompensatedImage(motionVectors, prevY, YC);

        cv::imshow("vectors", frameYCopy);
        cv::imshow("compensated", YC);
        cv::waitKey(11);

        MSE = computeMSE(frameYCopy, YC);
        PSNR = computePSNR(MSE);
        ENT = computeEntropy(YC);
        computeErrorImage(frameYCopy, YC, errorFrame);
        ENTe = computeEntropy(errorFrame);
      	std::cout<<frameNumber<<" "<<MSE<<" "<<PSNR<<" "<<ENT<<" "<<ENTe<<"\n";
      }
      else {

      	std::vector<cv::Mat> levelsY;
      	std::vector<cv::Mat> levelsPrevY;
      	std::vector<cv::Mat> motionVectorsP;
      	blockMatchingMulti(frameY, prevY, blockSize, windowSize, nbLevels, levelsY, levelsPrevY, motionVectorsP);

      	std::cout<<frameNumber;
      	for (int i=nbLevels-1; i>=0; --i) {

      	  //TODO : compute measures  (& display images) ...


      	  std::cout<<" "<<MSE<<" "<<PSNR<<" "<<ENT<<" "<<ENTe;
      	}
      	std::cout<<"\n";
      }


    }

    previousFrames.push(frameY);

    ++frameNumber;
  }

  return EXIT_SUCCESS;
}
