#include "blockmatching.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include "utils.hpp"



void blockMatchingMono(const cv::Mat &m1, const cv::Mat &m2,
		       int blockSize,
		       int windowSize,
		       cv::Mat &motionVectors)
{
  assert(m1.size() == m2.size()
	 && m1.type() == m2.type()
	 && m1.type() == CV_8UC1);
  assert(blockSize > 0);
  assert(windowSize > 0);

  //Here we suppose that m1.cols & m1.rows are multiple of blockSize
  const int blockXCount = m1.cols / blockSize;
  const int blockYCount = m1.rows / blockSize;

  motionVectors.create(blockYCount, blockXCount, CV_32SC2);

	// initialise useful values
	cv::Mat block1;
	block1.create(blockSize, blockSize, CV_32SC2);
	cv::Mat block2;
	block2.create(blockSize, blockSize, CV_32SC2);
	int bestI = 0, bestJ = 0;

  //We want to traverse all blocks from the m1 image.
  //For each block b1 in m1, we search the block b2 in m2 image
  //such as computeMSE(b1, b2) is minimal.

  //To call computeMSE we will need to create a cv::Mat corresponding to each block.
  //You can for example use the rowRange()/colRange() methods of cv::Mat.

  //Suppose b1 is at position (x1, y1)
  // and the best b2 (that is the block that minimizes computeMSE(b1, b2)) is at (x2Best, y2Best)
  // then the motion vector is (x2Best-x1, y2Best-y1).
  //We can fill the motionVectors matrix with:
  // motionVectors.at<cv::Vec2i>(i, j) = cv::Vec2i(x2Best-x1, y2Best-y1);
	for (int i = 0; i < m1.rows-blockSize-1; i+=blockSize) {
		for (int j = 0; j < m1.cols-blockSize-1; j+=blockSize) {

			block1 = m1(cv::Rect(j,i,blockSize,blockSize));
			double bestMSE = DBL_MAX;

			for (int k = i-windowSize/2+blockSize/2; k < i+windowSize/2-blockSize-1+blockSize/2; k++) {
				for (int l = j-windowSize/2+blockSize/2; l < j+windowSize/2-blockSize-1+blockSize/2; l++) {
					// handles the edges
					if (k >= 0 && k < m2.rows - blockSize-1 && l>= 0 && l < m2.cols - blockSize-1) {
						block2 = m2(cv::Rect(l,k,blockSize,blockSize));
						double currentMSE = computeMSE(block1, block2);

						if (currentMSE < bestMSE) {
							bestMSE = currentMSE;
							bestI = k;
							bestJ = l;
						}
					}
				}
			}
			motionVectors.at<cv::Vec2i>(i/blockSize, j/blockSize) = cv::Vec2i(bestJ-j, bestI-i);
		}
	}
}


//Helper function for blockMatchingMulti
//Here motion vectors are used both as an input and an output.
//If they are non empty, then they are used to initialize block b2 search window position.
void blockMatchingMono2(const cv::Mat &m1, const cv::Mat &m2,
			int blockSize,
			int windowSize,
			cv::Mat &motionVectors)
{
  assert(m1.size() == m2.size()
	 && m1.type() == m2.type()
	 && m1.type() == CV_8UC1);
  assert(blockSize > 0);
  assert(windowSize > 0);

  //Here we suppose that m1.cols & m1.rows are multiple of blockSize
  const int blockXCount = m1.cols / blockSize;
  const int blockYCount = m1.rows / blockSize;

	cv::Mat block1;
	block1.create(blockSize, blockSize, CV_32SC2);
	cv::Mat block2;
	block2.create(blockSize, blockSize, CV_32SC2);
	int bestI = 0, bestJ = 0;

	// standard matching if vectors are not computed
	if (motionVectors.empty()) {
		blockMatchingMono(m1,m2,blockSize,windowSize,motionVectors);
	}
	else {
		for (int i = 0; i < m1.rows-blockSize-1; i+=blockSize) {
			for (int j = 0; j < m1.cols-blockSize-1; j+=blockSize) {

				block1 = m1(cv::Rect(j,i,blockSize,blockSize));
				double bestMSE = DBL_MAX;

				for (int k = i-windowSize/2+blockSize/2; k < i+windowSize/2-blockSize-1+blockSize/2; k++) {
					for (int l = j-windowSize/2+blockSize/2; l < j+windowSize/2-blockSize-1+blockSize/2; l++) {
						// handles the edges
						cv::Vec2i currentVector = motionVectors.at<cv::Vec2i>(i/blockSize, j/blockSize);
						int kVec = k+currentVector(1);
						int lVec = l+currentVector(0);
						if (kVec >= 0 && kVec < m2.rows - blockSize-1 && lVec>= 0 && lVec < m2.cols - blockSize-1) {
							block2 = m2(cv::Rect(l+currentVector(0),k+currentVector(1),blockSize,blockSize));
							double currentMSE = computeMSE(block1, block2);

							if (currentMSE < bestMSE) {
								bestMSE = currentMSE;
								bestI = kVec;
								bestJ = lVec;
							}
						}
					}
				}
				motionVectors.at<cv::Vec2i>(i/blockSize, j/blockSize) = cv::Vec2i(bestJ-j, bestI-i);
			}
		}
	}

}




void up(const cv::Mat &m, cv::Mat &m2)
{
  assert(m.type() == CV_32SC2);

  m2 = 2 * m;
}


void blockMatchingMulti(const cv::Mat &m1, const cv::Mat &m2,
			int blockSize,
			int windowSize,
			int nbLevels,
			std::vector<cv::Mat> &levels1,
			std::vector<cv::Mat> &levels2,
			std::vector<cv::Mat> &motionVectors)
{
  assert(m1.size() == m2.size()
	 && m1.type() == m2.type()
	 && m1.type() == CV_8UC1);
  assert(blockSize > 0);
  assert(windowSize > 0);
  assert(nbLevels > 0);

  motionVectors.resize(nbLevels);
  levels1.resize(nbLevels);
  levels2.resize(nbLevels);

  std::vector<int> blockSizes(nbLevels);
  levels1[0] = m1;
  levels2[0] = m2;
  blockSizes[0] = blockSize;
  for (int i=1; i<nbLevels; ++i) { //start from 1
    cv::pyrDown(levels1[i-1], levels1[i], cv::Size(levels1[i-1].cols/2, levels1[i-1].rows/2));
    cv::pyrDown(levels2[i-1], levels2[i], cv::Size(levels2[i-1].cols/2, levels2[i-1].rows/2));
    blockSizes[i] = blockSizes[i-1]/2;
  }

  for (int i=nbLevels-1; i>=0; i--) {

    blockMatchingMono2(levels1[i], levels2[i], blockSizes[i], windowSize, motionVectors[i]);

    if (i > 0) {
      up(motionVectors[i], motionVectors[i-1]);
    }
  }

}
