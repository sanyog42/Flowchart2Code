#pragma once

#include "CodeGenerate.h"
#include <sstream>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class img {
public:
    img(wxString filepath, wxString codepath, int lang_sel);

    node* start = nullptr;

private:
	CodeGenerate* generate = nullptr;

	int counter = 1;
	float confThreshold = 0.5, nmsThreshold = 0.4;   // confidence and nms threshold for relevance
	std::string classes[7] = { "Text", "Arrow", "Connection", "Process", "Decision", "Data", "Terminator" };
	std::vector<int> status;
	std::vector<int> status1;

	struct node* traverse(struct node* temp, int id1);
	void preprocess(const cv::Mat& frame, cv::dnn::Net& net, cv::Size inpSize, bool swapRB);
	void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& out, cv::dnn::Net& net);
	void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame, cv::Rect box, int boxId);
	void sortdown(std::vector<cv::Rect>& boxes, std::vector<int>& classIds, std::vector<float>& confidences);
	void sortright(std::vector<cv::Rect>& boxes, std::vector<int>& classIds, std::vector<int>& objectIds);
	void createnodes(std::vector<int> classIds, std::vector<cv::Rect> boxes, std::vector<int> objectIds, std::vector<int> classIds1, std::vector<cv::Rect> boxes1, std::vector<int> objectIds1, int idx, int idx1);
	void findloops(std::vector<int> classIds, std::vector<cv::Rect> boxes, std::vector<int> objectIds, std::vector<int> classIds1, std::vector<cv::Rect> boxes1, std::vector<int> objectIds1);
	void inserttext(std::string text, int objectId);
	//void print(struct node* temp);   // for debugging
	int findbox(std::vector<cv::Rect> boxes, int objectId);
	int idxright(int i, std::vector<int> objectIds);
	bool check(float cent_x, float cent_y, cv::Rect box);
	bool checkdown(float cent_x, float cent_y, cv::Rect box);
	bool checkright(float cent_x, float cent_y, cv::Rect box);
	bool arrowbw(std::vector<cv::Rect> boxes, std::vector<int> classIds, int flag, int i, int idx);
	int arrowloop(int j, std::vector<int> objectIds, std::vector<cv::Rect> boxes, std::vector<int> classIds);
	cv::Mat resizeKeepAspectRatio(cv::Mat input, cv::Size dstSize, cv::Scalar bgcolor);
};
