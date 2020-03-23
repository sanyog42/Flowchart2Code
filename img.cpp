#include "img.h"

node* start = nullptr;

struct node* traverse(struct node* temp, int id1);

img::img(wxString filepath, wxString codepath, int lang_sel) {
	std::string modelPath = "model/flowchart.pb";
	std::string configPath = "model/flowchart.pbtxt";

	cv::dnn::Net net = cv::dnn::readNet(modelPath, configPath);
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	std::vector<cv::String> outNames = net.getUnconnectedOutLayersNames();

	static const std::string kWinName = "Flowchart2Code";
	cv::namedWindow(kWinName, cv::WINDOW_AUTOSIZE);

	cv::VideoCapture cap;
	std::string path = std::string(filepath.mb_str());
	std::ifstream file(path);

	cap.open(path);

	cv::Mat frame, blob;
	
	while (cv::waitKey(1) < 0)
	{
		cap >> frame;

		if (frame.empty())
		{
			cv::waitKey();
			break;
		}

		frame = resizeKeepAspectRatio(frame, cv::Size(1024, 1024), cv::Scalar(255, 255, 255));

		preprocess(frame, net, cv::Size(frame.cols, frame.rows), true);

		std::vector<cv::Mat> outs;
		net.forward(outs, outNames);

		postprocess(frame, outs, net);

		// Efficiency information
		std::vector<double> layersTimes;
		double freq = cv::getTickFrequency() / 1000;
		double t = net.getPerfProfile(layersTimes) / freq;
		std::string label = cv::format("Inference time: %.2f ms", t);
		putText(frame, label, cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));

		imshow(kWinName, frame);

		//print(start); // for debugging
		if (start != NULL) {
			generate = new CodeGenerate(codepath, start, lang_sel);
		}
		else {
			wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Error in Scan!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
			dial->ShowModal();
		}
	}
}

img::~img() {

}

inline void img::preprocess(const cv::Mat& frame, cv::dnn::Net& net, cv::Size inpSize, bool swapRB) {
	static cv::Mat blob;
	if (inpSize.width <= 0) inpSize.width = frame.cols;
	if (inpSize.height <= 0) inpSize.height = frame.rows;
	cv::dnn::blobFromImage(frame, blob, 1.0, inpSize, cv::Scalar(), swapRB, false, CV_8U);

	net.setInput(blob, "");
	if (net.getLayer(0)->outputNameToIndex("im_info") != -1)
	{
		resize(frame, frame, inpSize);
		cv::Mat imInfo = (cv::Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
		net.setInput(imInfo, "im_info");
	}
}

void img::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, cv::dnn::Net& net) {
	static std::vector<int> outLayers = net.getUnconnectedOutLayers();
	static std::string outLayerType = net.getLayer(outLayers[0])->type;

	std::vector<int> classIdsinit;
	std::vector<float> confidencesinit;
	std::vector<cv::Rect> boxesinit;
	if (outLayerType == "DetectionOutput")
	{
		// [batchId, classId, confidence, left, top, right, bottom]

		CV_Assert(outs.size() > 0);
		for (size_t k = 0; k < outs.size(); k++)
		{
			float* data = (float*)outs[k].data;
			for (size_t i = 0; i < outs[k].total(); i += 7)
			{
				float confidence = data[i + 2];
				if (confidence > confThreshold)
				{
					int left = (int)data[i + 3];
					int top = (int)data[i + 4];
					int right = (int)data[i + 5];
					int bottom = (int)data[i + 6];
					int width = right - left + 1;
					int height = bottom - top + 1;
					if (width <= 2 || height <= 2)
					{
						left = (int)(data[i + 3] * frame.cols);
						top = (int)(data[i + 4] * frame.rows);
						right = (int)(data[i + 5] * frame.cols);
						bottom = (int)(data[i + 6] * frame.rows);
						width = right - left + 1;
						height = bottom - top + 1;
					}
					classIdsinit.push_back((int)(data[i + 1]) + 1);
					boxesinit.push_back(cv::Rect(left, top, width, height));
					confidencesinit.push_back(confidence);
				}
			}
		}
	}
	else {
		wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Unknown Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
		dial->ShowModal();
		exit(0);
	}

	std::vector<int> indices;
	std::vector<int> objectIds;
	std::vector<cv::Rect> boxes;
	std::vector<float> confidences;
	std::vector<int> classIds;

	cv::dnn::NMSBoxes(boxesinit, confidencesinit, confThreshold, nmsThreshold, indices);

	for (int i = 0; i < indices.size(); ++i) {
		int idx = indices[i];
		boxes.push_back(boxesinit[idx]);
		classIds.push_back(classIdsinit[idx]);
		confidences.push_back(confidencesinit[idx]);
		objectIds.push_back(counter);
		counter++;
		status.push_back(0);
		status1.push_back(0);
	}

	sortdown(boxes, classIds, confidences);

	std::vector<int> objectIds1 = objectIds;
	std::vector<cv::Rect> boxes1 = boxes;
	std::vector<int> classIds1 = classIds;

	sortright(boxes1, classIds1, objectIds1);

	createnodes(classIds, boxes, objectIds, classIds1, boxes1, objectIds1, 0, 0);
	if (start != NULL) {
		findloops(classIds, boxes, objectIds, classIds1, boxes1, objectIds1);
	}

	for (int idx = 0; idx < boxes.size(); idx++)
	{
		//cout << classes[classIds[idx] - 1] << endl; // for debugging
		cv::Rect box = boxes[idx];
		int boxId;
		if (classIds[idx] == 1) {
			boxId = findbox(boxes, idx + 1);
		}
		else {
			boxId = -1;
		}

		drawPred(classIds[idx], confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame, box, boxId);
	}
}

void img::drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame, cv::Rect box, int boxId) {
	if (classId == 1) {
		box.x -= 5;
		box.width += 10;
		box.height += 10;
		cv::Mat textbox = frame(box);
		std::string text;

		tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
		ocr->Init("tessdata", "eng", tesseract::OEM_LSTM_ONLY);
		ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
		ocr->SetImage(textbox.data, textbox.cols, textbox.rows, 3, textbox.step);
		text = std::string(ocr->GetUTF8Text());
		for (int i = 0; i < text.length(); i++) {
			if (text[i] == '\n') {
				text.erase(i);
			}
		}
		ocr->End();

		box.width -= 10;
		box.height -= 10;
		box.x += 5;

		inserttext(text, boxId);
	}

	rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

	std::string label = cv::format("%.2f", conf);
	label = classes[classId - 1] + ": " + label;
	int baseLine;
	cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

	top = std::max(top, labelSize.height);
	rectangle(frame, cv::Point(left, top - labelSize.height), cv::Point(left + labelSize.width, top + baseLine), cv::Scalar::all(255), cv::FILLED);
	putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar());
}

void img::sortdown(std::vector<cv::Rect>& boxes, std::vector<int>& classIds, std::vector<float>& confidences) {
	int len = boxes.size();
	for (int i = 0; i < len - 1; i++) {
		cv::Rect* min0 = &boxes[i];
		int* min1 = &classIds[i];
		float* min2 = &confidences[i];
		for (int j = i + 1; j < len; j++) {
			if (boxes[j].y < (*(min0)).y) {
				min0 = &boxes[j];
				min1 = &classIds[j];
				min2 = &confidences[j];
			}
		}
		cv::Rect temp0;
		int temp1;
		float temp2;

		temp0 = *min0;
		temp1 = *min1;
		temp2 = *min2;

		*min0 = boxes[i];
		*min1 = classIds[i];
		*min2 = confidences[i];

		boxes[i] = temp0;
		classIds[i] = temp1;
		confidences[i] = temp2;
	}
}

void img::sortright(std::vector<cv::Rect>& boxes, std::vector<int>& classIds, std::vector<int>& objectIds) {
	int len = boxes.size();
	for (int i = 0; i < len - 1; i++) {
		cv::Rect* min0 = &boxes[i];
		int* min1 = &classIds[i];
		int* min2 = &objectIds[i];
		for (int j = i + 1; j < len; j++) {
			if (boxes[j].x < (*(min0)).x) {
				min0 = &boxes[j];
				min1 = &classIds[j];
				min2 = &objectIds[j];
			}
		}
		cv::Rect temp0;
		int temp1;
		int temp2;

		temp0 = *min0;
		temp1 = *min1;
		temp2 = *min2;

		*min0 = boxes[i];
		*min1 = classIds[i];
		*min2 = objectIds[i];

		boxes[i] = temp0;
		classIds[i] = temp1;
		objectIds[i] = temp2;
	}
}

void img::createnodes(std::vector<int> classIds, std::vector<cv::Rect> boxes, std::vector<int> objectIds, std::vector<int> classIds1, std::vector<cv::Rect> boxes1, std::vector<int> objectIds1, int idx, int idx1) {
	for (int i = idx; i < classIds.size(); i++) {
		if (classIds[i] == 1 || classIds[i] == 2 || status[i] == 1) {
			continue;
		}
		if (start == NULL) {
			node* x = new node;
			if (x != NULL) {
				x->id = objectIds[i];
				x->type = classIds[i];
				start = x;
				idx = i;
				idx1 = idxright(i, objectIds1);
				status[i] = 1;
			}
			else {
				wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Memory Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
				dial->ShowModal();
				exit(0);
			}
		}
		else {
			float cent_x = boxes[i].x + ((boxes[i].width) / 2);
			float cent_y = boxes[i].y + ((boxes[i].height) / 2);
			if (checkdown(cent_x, cent_y, boxes[idx]) == true && arrowbw(boxes, classIds, 0, i, idx) == true) {
				node* d = new node;
				if (d != NULL) {
					d->id = objectIds[i];
					d->type = classIds[i];
					node* temp = traverse(start, objectIds[idx]);
					temp->down = d;
					idx = i;
					idx1 = idxright(i, objectIds1);
					status[i] = 1;
				}
				else {
					wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Memory Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
					dial->ShowModal();
					exit(0);
				}
			}
			else {
				if (checkright(cent_x, cent_y, boxes1[idx1]) == true && arrowbw(boxes1, classIds1, 1, idxright(i, objectIds1), idx1) == true) {
					node* r = new node;
					if (r != NULL) {
						r->id = objectIds[i];
						r->type = classIds[i];
						node* temp = traverse(start, objectIds1[idx1]);
						temp->right = r;
						status[i] = 1;
						createnodes(classIds, boxes, objectIds, classIds1, boxes1, objectIds1, i, idxright(i, objectIds1));
					}
					else {
						wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Memory Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
						dial->ShowModal();
						exit(0);
					}
				}
				else {
					for (int j = i + 1; j < objectIds.size(); j++) {
						if (classIds[j] == 1 || classIds[j] == 2 || status[j] == 1) {
							continue;
						}
						float cent1_x = boxes[j].x + ((boxes[j].width) / 2);
						float cent1_y = boxes[j].y + ((boxes[j].height) / 2);
						if (checkright(cent_x, cent_y, boxes[j]) == true) {
							if (checkdown(cent1_x, cent1_y, boxes[idx]) == true && arrowbw(boxes, classIds, 0, j, idx) == true) {
								node* d = new node;
								if (d != NULL) {
									d->id = objectIds[j];
									d->type = classIds[j];
									node* temp = traverse(start, objectIds[idx]);
									temp->down = d;
									idx = j;
									idx1 = idxright(j, objectIds1);
									status[j] = 1;
								}
								else {
									wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Memory Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
									dial->ShowModal();
									exit(0);
								}
								i--;
								break;
							}
							else if (checkright(cent1_x, cent1_y, boxes1[idx1]) == true && arrowbw(boxes1, classIds1, 1, idxright(j, objectIds1), idx1) == true) {
								node* r = new node;
								if (r != NULL) {
									r->id = objectIds[j];
									r->type = classIds[j];
									node* temp = traverse(start, objectIds1[idx1]);
									temp->right = r;
									status[j] = 1;
									createnodes(classIds, boxes, objectIds, classIds1, boxes1, objectIds1, i, idxright(j, objectIds1));
									createnodes(classIds, boxes, objectIds, classIds1, boxes1, objectIds1, j, idxright(j, objectIds1));
								}
								else {
									wxMessageDialog* dial = new wxMessageDialog(NULL, wxT("Memory Error!"), wxT("Error"), wxOK | wxCENTER | wxICON_ERROR);
									dial->ShowModal();
									exit(0);
								}
								i--;
								break;
							}
						}
					}
				}
			}
		}
	}
}

void img::findloops(std::vector<int> classIds, std::vector<cv::Rect> boxes, std::vector<int> objectIds, std::vector<int> classIds1, std::vector<cv::Rect> boxes1, std::vector<int> objectIds1) {
	for (int i = 0; i < objectIds1.size(); i++) {
		if (status1[i] == 1/* || classIds1[i] != 5*/) {
			continue;
		}
		int idx = arrowloop(i, objectIds1, boxes1, classIds1);
		if (idx != -1) {
			node* temp = traverse(start, objectIds1[idx]);
			node* temp1 = traverse(start, objectIds1[i]);
			if(temp != NULL && temp1 != NULL){
                temp->loop = temp1;
			}
			status1[i] = 1;
			status1[idx] = 1;
		}
	}
}

struct node* traverse(struct node* temp, int id1) {
	if (temp->id == id1) {
		return temp;
	}
	node* x = NULL;
	if (temp->right != NULL) {
		x = traverse(temp->right, id1);
	}
	if (temp->down != NULL && x == NULL) {
		x = traverse(temp->down, id1);
	}
	return x;
}

void img::inserttext(std::string text, int objectId) {
	if (objectId == -1) {
		return;
	}
	node* temp = traverse(start, objectId);
	if (temp == NULL) {
		return;
	}
	temp->text = text;
}

/*void img::print(struct node* temp) { // for debugging
	if (temp != NULL) {
		cout << temp->text << "," << temp->type << "," << temp->id << endl;
		if (temp->right != NULL) {
			cout << "(right)";
			print(temp->right);
		}
		if (temp->loop != NULL) {
			cout << "(loop)";
			cout << temp->loop->text << "," << temp->loop->type << "," << temp->loop->id << endl;
		}
		if (temp->down != NULL) {
			cout << "(down)";
			print(temp->down);
		}
	}
}*/

int img::findbox(std::vector<cv::Rect> boxes, int objectId) {
	int idx = objectId - 1;
	float cent_x = (boxes[idx].x) + ((boxes[idx].height) / 2);
	float cent_y = (boxes[idx].y) + ((boxes[idx].width) / 2);
	for (int i = 0; i < boxes.size(); ++i) {
		if (i == objectId - 1) {
			continue;
		}
		if (check(cent_x, cent_y, boxes[i]) == true) {
			return i + 1;
		}
	}
	return -1;
}

int img::idxright(int i, std::vector<int> objectIds) {
	for (int j = 0; j < objectIds.size(); j++) {
		if (objectIds[j] == i + 1) {
			return j;
		}
	}
}

bool img::check(float cent_x, float cent_y, cv::Rect box) {
	if ((cent_x > box.x&& cent_x < box.x + box.width) && (cent_y > box.y&& cent_y < box.y + box.height)) {
		return true;
	}
	return false;
}

bool img::checkdown(float cent_x, float cent_y, cv::Rect box) {
	if (cent_x > box.x&& cent_x < box.x + box.width && cent_y > box.y) {
		return true;
	}
	return false;
}

bool img::checkright(float cent_x, float cent_y, cv::Rect box) {
	if (cent_y > box.y&& cent_y < box.y + box.height && cent_x > box.x) {
		return true;
	}
	return false;
}

bool img::arrowbw(std::vector<cv::Rect> boxes, std::vector<int> classIds, int flag, int i, int idx) {
	int flag1 = 0;
	if (flag == 0) {
		float xmin, xmax;
		if (boxes[i].x < boxes[idx].x) {
			xmin = boxes[i].x;
		}
		else {
			xmin = boxes[idx].x;
		}
		if (boxes[i].x + boxes[i].width > boxes[idx].x + boxes[idx].width) {
			xmax = boxes[i].x + boxes[i].width;
		}
		else {
			xmax = boxes[idx].x + boxes[idx].width;
		}
		for (int j = idx + 1; j < i; j++) {
			if (classIds[j] == 1) {
				continue;
			}
			float cent_x = boxes[j].x + ((boxes[j].width) / 2);
			if (cent_x > xmin&& cent_x < xmax) {
				if (classIds[j] == 2) {
					flag1 = 1;
				}
				else {
					flag1 = 0;
					break;
				}
			}
		}
	}
	else if (flag == 1) {
		float ymin, ymax;
		if (boxes[i].y < boxes[idx].y) {
			ymin = boxes[i].y;
		}
		else {
			ymin = boxes[idx].y;
		}
		if (boxes[i].y + boxes[i].height > boxes[idx].y + boxes[idx].height) {
			ymax = boxes[i].y + boxes[i].height;
		}
		else {
			ymax = boxes[idx].y + boxes[idx].height;
		}
		for (int j = idx + 1; j < i; j++) {
			if (classIds[j] == 1) {
				continue;
			}
			float cent_y = boxes[j].y + ((boxes[j].height) / 2);
			if (cent_y > ymin&& cent_y < ymax) {
				if (classIds[j] == 2) {
					flag1 = 1;
				}
				else {
					flag1 = 0;
					break;
				}
			}
		}
	}
	if (flag1 == 1) {
		return true;
	}
	else {
		return false;
	}
}

int img::arrowloop(int i, std::vector<int> objectIds, std::vector<cv::Rect> boxes, std::vector<int> classIds) {
	int idx = -1, flag = 0;
	float cent_x = boxes[i].x + ((boxes[i].width) / 2);
	float cent_y = boxes[i].y + ((boxes[i].height) / 2);
	for (int j = i - 1; j >= 0; j--) {
		if (classIds[j] != 2 || status1[j] == 1) {
			continue;
		}
		float corn_x = boxes[j].x + boxes[j].width;
		float corn_y = boxes[j].y;
		if (corn_y > boxes[i].y&& corn_y < boxes[i].y + boxes[i].height) {
			for (int k = j + 1; k < objectIds.size(); k++) {
				if (k == i || (cent_y > boxes[k].y&& cent_y < boxes[k].y + boxes[k].height) || classIds[k] == 1) {
					continue;
				}
				float corn_x1 = boxes[j].x + boxes[j].width;
				float corn_y1 = boxes[j].y + boxes[j].height;
				if (corn_y1 > boxes[k].y&& corn_y1 < boxes[k].y + boxes[k].height) {
					idx = k;
					flag = 1;
					status1[j] = 1;
					break;
				}
			}
		}
		if (flag == 1) {
			break;
		}
	}
	return idx;
}

cv::Mat img::resizeKeepAspectRatio(const cv::Mat input, const cv::Size dstSize, const cv::Scalar bgcolor)
{
	cv::Mat output;

	double h1 = dstSize.width * (input.rows / (double)input.cols);
	double w2 = dstSize.height * (input.cols / (double)input.rows);
	if (h1 <= dstSize.height) {
		cv::resize(input, output, cv::Size(dstSize.width, h1));
	}
	else {
		cv::resize(input, output, cv::Size(w2, dstSize.height));
	}

	int top = (dstSize.height - output.rows) / 2;
	int down = (dstSize.height - output.rows + 1) / 2;
	int left = (dstSize.width - output.cols) / 2;
	int right = (dstSize.width - output.cols + 1) / 2;

	cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor);

	return output;
}
