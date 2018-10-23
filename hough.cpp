#include "HoughTransform.h"
#include "CImg.h"
#include "canny.h"
#include <iostream>
#include <cmath>
#include <String>

#define M_PI 3.14159265358979323846
using namespace cimg_library;
using namespace std;
HoughTransform::HoughTransform(CImg<unsigned char> inputImg, CImg<unsigned char> origin, int id, int thres) {
	this->id = id;
	img = inputImg;
	result = origin;
	width = img.width();
	height = img.height();
	double temp = width * width + height * height;
	dia = ceil(sqrt(temp));
	deltaTheta = 15;
	cout << deltaTheta << endl;
	deltaRho = dia / 25;
	dis = 80;
	threshold = thres;
	maxLength = dia / 10;
	accumulation = CImg<unsigned char>(360, dia);

	string t = to_string(id) + "edge_origin.bmp";
	const char * tempName = t.c_str();
	img.save(tempName);
	initTriangle();
	fillAccumulation();
	findLocalMaximums(threshold);
	filter();
	generateLines();
	drawPoints();
}


void HoughTransform::initTriangle() {
	cout << "initTriangle" << endl;
	sinTheta = new double[360];
	cosTheta = new double[360];
	for (int i = 0; i < 360; i++) {
		sinTheta[i] = sin(i*M_PI / 180);
		cosTheta[i] = cos(i*M_PI / 180);
	}

	for (int i = 0; i < accumulation.width(); i++) {
		for (int j = 0; j < accumulation.height(); j++) {
			accumulation(i, j) = 0;
		}
	}
}

void HoughTransform::fillAccumulation() {
	cout << "fillAccumulation" << endl;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			//cout << img(i, j) << endl;
			if (img(i, j) == 255) {
				for (int theta = 0; theta < 360; theta++) {
					int ro = abs(round(i*cosTheta[theta] + j * sinTheta[theta]));
					accumulation(theta, ro) = accumulation(theta, ro) + 1;

				}
			}
		}
	}
	//accumulation.display();
}

void HoughTransform::findLocalMaximums(int threshold) {
	for (int r = 0; r < dia; r++) {
		for (int theta = 0; theta < 360; theta++) {
			bool push = true;
			if (accumulation(theta, r) > threshold) {
				for (int i = 0; i < sortBuffer.size(); i++) {
					if (sqrt(pow(sortBuffer[i].first - theta, 2) + pow(sortBuffer[i].second - r, 2)) < dis) {
						push = false;
						if (accumulation(sortBuffer[i].first, sortBuffer[i].second) <
							accumulation(theta, r)) {							
							sortBuffer[i] = make_pair(theta, r);
						} 
					}
				}
				if (push) {
					sortBuffer.push_back(make_pair(theta, r));
				}
			}
		}
	}
}

void HoughTransform::filter() {
	int count = 0;
	/*for (int i = 0; i < sortBuffer.size(); ) {
		double k = 0, b = 0;
		int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
		if (sinTheta[sortBuffer[i].first] != 0) {
			k = (-1) * (double)cosTheta[sortBuffer[i].first] / sinTheta[sortBuffer[i].first];
			b = sortBuffer[i].second / sinTheta[sortBuffer[i].first];
			linesParams.push_back(make_pair(k, b));
		}
		else {
			b = sortBuffer[i].second;
		}
		bool first = true;
		for (int j = 0; j < width; j++) {
			if(k*j + b >= 0 && k*j + b < height)
			if (img(j, k*j + b) == 255) {
				if (first) {
					first = false;
					x1 = j;
					y1 = k * j + b;
				}
				else {
					x2 = j;
					y2 = k * j + b;
				}
			}
		}
		int dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y1, 2));
		if (dist < maxLength) {
			count++;
			sortBuffer.erase(sortBuffer.begin() + i);
		}
		else {
			i++;
		}
	}*/

	for (int i = 0; i < sortBuffer.size(); ) {
		boolean eraseI = false;
		for (int j = i + 1; j < sortBuffer.size(); ) {
			if ((abs(sortBuffer[i].first - sortBuffer[j].first) < deltaTheta
				|| abs(abs(sortBuffer[i].first - sortBuffer[j].first) - 360) < deltaTheta
				|| abs(abs(sortBuffer[i].first - sortBuffer[j].first) - 180) < deltaTheta)
				&& abs(sortBuffer[i].second - sortBuffer[j].second) < deltaRho
				&&( (sortBuffer[i].first > 0 && sortBuffer[i].first < 90 || sortBuffer[i].first > 180 && sortBuffer[i].first < 270)
				&& (sortBuffer[j].first > 0 && sortBuffer[j].first < 90 || sortBuffer[j].first > 180 && sortBuffer[j].first < 270)
				|| (sortBuffer[i].first > 90 && sortBuffer[i].first < 180 || sortBuffer[i].first > 270 && sortBuffer[i].first < 360)
				&& (sortBuffer[j].first > 90 && sortBuffer[j].first < 180 || sortBuffer[j].first > 270 && sortBuffer[j].first < 360))) {
					count++;
					if (accumulation(sortBuffer[i].first, sortBuffer[i].second) <
						accumulation(sortBuffer[j].first, sortBuffer[j].second)) {
						sortBuffer.erase(sortBuffer.begin() + i);
						eraseI = true;
					}
					else if (accumulation(sortBuffer[j].first, sortBuffer[j].second) <=
						accumulation(sortBuffer[i].first, sortBuffer[i].second)) {
						sortBuffer.erase(sortBuffer.begin() + j);
						continue;
					}
				}
			j++;
		}
		if (!eraseI) {
			i++;
		}
		else {
			i--;
		}
	}
	cout << "----------in filter 2----------- count = " << count << endl;
}

void HoughTransform::generateLines() {
	
	//draw
	for (int i = 0; i < sortBuffer.size(); i++) {
		cout << "theta  " << sortBuffer[i].first << "  rho  " << sortBuffer[i].second;
		double k = 0, b = 0;
		if (sinTheta[sortBuffer[i].first] != 0) {
			k = (-1) * (double)cosTheta[sortBuffer[i].first] / sinTheta[sortBuffer[i].first];
			b = sortBuffer[i].second / sinTheta[sortBuffer[i].first];
			linesParams.push_back(make_pair(k, b));
		}
		else {
			b = sortBuffer[i].second;
		}
		cout << " k " << k << " b " << b << endl;
	}
	cout << "size  " << linesParams.size() << endl;

	const double blue[] = { 0, 0, 255 };
	const double red[] = { 255, 0, 0 };
	for (int i = 0; i < linesParams.size(); i++) {
		int x0 = 0, y0, x1, y1;
		double k = linesParams[i].first;
		double b = linesParams[i].second;
		if (k*x0 + b >= 0 && k*x0 + b < height) {
			y0 = k * x0 + b;
			x1 = width - 1;
			y1 = k * x1 + b;
		}
		else if (k != 0) {
			y0 = 0;
			x0 = (y0 - b) / k;
			y1 = height - 1;
			x1 = (y1 - b) / k;
		}
		else {
			x0 = 0;
			x1 = width - 1;
			y0 = b;
			y1 = b;
		}

		result.draw_line(x0 + 3, y0 + 3, x1 + 3, y1 + 3, blue);

	}

	string t = to_string(id) + "paperLines_origin.bmp";
	const char * temp = t.c_str();

	//result.display();
	result.save(temp);
}

void HoughTransform::drawPoints() {
	for (int i = 0; i < linesParams.size(); i++) {
		for (int j = i + 1; j < linesParams.size(); j++) {
			if (linesParams[i].first != linesParams[j].first) {
				double x = (-1) * (linesParams[i].second - linesParams[j].second) / (linesParams[i].first - linesParams[j].first);
				int y = linesParams[i].first * x + linesParams[i].second;
				if (x >= 0 && x < width && y >= 0 && y < height) {
					points.push_back(make_pair((int)x + 3, y + 3));
				}
			}
		}
	}
	const double pointColor[] = { 255, 0, 0 };
	cout << "draw points" << " " << points.size() << endl;
	for (int i = 0; i < points.size(); i++) {
		result.draw_circle(points[i].first, points[i].second, 7, pointColor);
	}
	//result.display(); 
	string t = to_string(id) + "paperPoint_origin.bmp";
	const char * temp = t.c_str();
	result.save(temp);
}