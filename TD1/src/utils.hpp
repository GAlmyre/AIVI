#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/core/core.hpp>

//compute error image
extern void displayableErrorImage(const cv::Mat &im, const cv::Mat &imC, cv::Mat &imErr);

//im & imC are input images, imErr is the output image
extern void computeErrorImage(const cv::Mat &im, const cv::Mat &imC, cv::Mat &imErr);

//compute MSE between two images
extern double computeMSE(const cv::Mat &im, const cv::Mat &imC);

//compute PSNR between two images
extern double computePSNR(const cv::Mat &im, const cv::Mat &imC);

//compute PSNR from already computed MSE
extern double computePSNR(double MSE);

//compute entropy of an image
extern double computeEntropy(const cv::Mat &im);

#endif /* ! UTILS_HPP */
