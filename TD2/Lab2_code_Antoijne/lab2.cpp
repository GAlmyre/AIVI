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

int main(int argc, char **argv){
  if(argc != 6) {
    std::cerr << "Usage: " << argv[0] << " video-filename distance-between-two-frames-for-prediction nbLevels blockSize windowSize" << std::endl;
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

  const int blockSize = atoi(argv[4]);//8
  const int windowSize = atoi(argv[5]);//32

  if(blockSize < 0){
    std::cerr<<"Error: blockSize must be a strictly positive integer"<<std::endl;
    return EXIT_FAILURE;
  }
  
  if(windowSize < 0){
    std::cerr<<"Error: windowSize must be a strictly positive integer"<<std::endl;
    return EXIT_FAILURE;
  }  
  //TODO: it would be better to pass these values as parameters DONE

  cv::VideoCapture cap;
  cap.open(videoFilename);
  if ( ! cap.isOpened()) {
    std::cerr << "Error : Unable to open video file " << argv[1] << std::endl;
    return -1;
  }

  unsigned long frameNumber = 0;

  const size_t deltaT = interFramesDistance;
  std::queue<cv::Mat> previousFrames;  

  cv::namedWindow("test",1);
  
  for ( ; ; ) {

    cv::Mat frameBGR;
    cap >> frameBGR;
    if (frameBGR.empty()) {
      break;
    }

    //save frame
    // std::stringstream ss;
    // ss<<"frame_"<<std::setfill('0')<<std::setw(6)<<frameNumber<<".png";
    // cv::imwrite(ss.str(), frameBGR);

    //convert from BGR to Y
    cv::Mat frameY;
    cv::cvtColor(frameBGR, frameY, CV_BGR2GRAY);

    if (previousFrames.size() >= deltaT) {
      cv::Mat prevY = previousFrames.front();
      previousFrames.pop();

      double MSE, PSNR, ENT, ENTe;
      
      if (nbLevels == 1) {

	cv::Mat motionVectors;
	blockMatchingMono(frameY, prevY, blockSize, windowSize, motionVectors);
	
	cv::Mat YC;
	computeCompensatedImage(motionVectors, prevY, YC);
	cv::imshow("test", YC);
	cv::waitKey(11);
	
	// drawMVi(frameY, motionVectors);
	// cv::imshow("test", frameY);
	// cv::waitKey(11);
	
	
	
	// MSE  = computeMSE(frameY, prevY);
	// PSNR = computePSNR(frameY, prevY);
	// ENT = computeEntropy(frameY);
	// cv::Mat errorImage;
	// computeErrorImage(frameY, prevY, errorImage); 
	//TODO: compute measures (& display images) ...

	
	
	// std::cout<<frameNumber<<" "<<MSE<<" "<<PSNR<<" "<<ENT<<" "<<ENTe<<"\n";
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
