#include "utils.hpp"
#include <opencv2/imgproc/imgproc.hpp> //cvtColor
#include <iostream>
#include <vector>

void
computeErrorImage(const cv::Mat &im, const cv::Mat &imC, cv::Mat &imErr)
{
  imErr = im-imC;
}

void displayableErrorImage(const cv::Mat &im, const cv::Mat &imC, cv::Mat &imErr) {
  imErr = cv::min(255, cv::max(0,im-imC+128));
}

double computeMSE(const cv::Mat &im, const cv::Mat &imC)
{
  //std::cerr<<im.type()<<" "<<CV_8UC1<<" "<<CV_8UC3<<"\n";
  assert(im.type() == CV_8UC1);
  double ret = 0.f;
  assert(im.size() == imC.size());
  for (int i = 0; i < im.rows; i++) {
    for (int j = 0; j < im.cols; j++) {
      int diff = ((int)im.at<unsigned char>(i,j) - (int)imC.at<unsigned char>(i,j));
      ret += pow(diff, 2);
    }
  }

  return ret/(im.cols*im.rows);
}

double computePSNR(const cv::Mat &im, const cv::Mat &imC)
{
  return 10*log((255*255)/computeMSE(im, imC));
}

double computePSNR(double MSE)
{
  return 10*log((255*255)/MSE);
}

double computeEntropy(const cv::Mat &im)
{
  // computes the histogram
  cv::Mat hist;
  const int histSize = 256;
  float range[] = {0,255};
  const float* histRange = {range};
  cv::calcHist(&im, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
  hist /= im.total();

  cv::Mat logHist;
  cv::log(hist, logHist);

  return -1*cv::sum(hist.mul(logHist)).val[0];
}
