#include "canny.h"
#include "HoughTransform.h"
#include "CImg.h"
#include <String>
#include "CircleDetector.h"
using namespace cimg_library;
using namespace std;
int main()
{
	for (int i = 1; i < 6; i++) {
		string name = to_string(i) + ".bmp";
		const char* input = name.c_str();
		CImg<unsigned char> originalImg(input);
		//string edge_origin = to_string(i) + "edge_origin.bmp";
		//input = edge_origin.c_str();
		//CImg<unsigned char> edgeImg(input); 
		canny answer = canny(originalImg, 0.6, 200);
		HoughTransform houghTransform = HoughTransform(answer.getResult(), originalImg, i, 225);
	}
	/*for (int i = 1; i <= 6; i++) {
		string name = to_string(i) + ".bmp";
		const char* input = name.c_str();
		CImg<unsigned char> originalImg(input);
		canny answer = canny(originalImg);
		CircleDetector circle = CircleDetector(answer.getResult(), originalImg, i);
	}*/
	//HoughTransform houghTransform = HoughTransform(answer.getResult(), originalImg, i);
	
	system("pause");
}
