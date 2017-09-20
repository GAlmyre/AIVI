#include "utils.hpp"
#include <iostream>

void
computeErrorImage(const cv::Mat &im, const cv::Mat &imC, cv::Mat &imErr)
{
  //TODO...
}

double computeMSE(const cv::Mat &im, const cv::Mat &imC)
{
  std::cerr<<im.type()<<" "<<CV_8UC1<<" "<<CV_8UC3<<"\n";
  assert(im.type() == CV_8UC1);
  double ret = 0.f;
  for (int i = 0; i < im.rows; i++) {
    for (int j = 0; j < im.cols; j++) {
      int diff = (im.at<unsigned char>(i,j) - imC.at<unsigned char>(i,j));
      ret += pow(diff, 2);
    }
  }

  return ret/(im.cols*im.rows);
}

double computePSNR(const cv::Mat &im, const cv::Mat &imC)
{
  //TODO...
}

double computePSNR(double MSE)
{
  //TODO...
}

double computeEntropy(const cv::Mat &im)
{
  //TODO...
}
