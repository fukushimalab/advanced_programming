#include <fstream>
#include <opencv2/opencv.hpp>
#include <direct.h>

#define CV_LIB_PREFIX comment(lib, "opencv_"
#define CV_LIB_VERSION CVAUX_STR(CV_MAJOR_VERSION)\
        CVAUX_STR(CV_MINOR_VERSION)\
        CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef _DEBUG
#define CV_LIB_SUFFIX CV_LIB_VERSION "d.lib")
#else
#define CV_LIB_SUFFIX CV_LIB_VERSION ".lib")
#endif
#define CV_LIBRARY(lib_name) CV_LIB_PREFIX CVAUX_STR(lib_name) CV_LIB_SUFFIX
#pragma CV_LIBRARY(core)
#pragma CV_LIBRARY(highgui)
#pragma CV_LIBRARY(imgcodecs)
#pragma CV_LIBRARY(imgproc)
#pragma CV_LIBRARY(photo)

using namespace cv;
using namespace std;

struct EmbedInfo
{
	Mat mask;
	Point center;
};

void embedImage(const cv::Mat src, cv::Mat& dest, const cv::Mat embeddedImage, const double angle, const double tx, const double ty)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	src.copyTo(dest);

	if (angle != 0)
	{
		Point2d center(embeddedImage.cols / 2, embeddedImage.rows / 2);
		Mat rotateMat = cv::getRotationMatrix2D(center, angle, 1.0);
		rotateMat.at<double>(0, 2) += tx;
		rotateMat.at<double>(1, 2) += ty;
		warpAffine(embeddedImage, dest, rotateMat, dest.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
	}
	else
	{
		embeddedImage.copyTo(dest(Rect(tx, ty, embeddedImage.cols, embeddedImage.rows)));
	}
}

void cloningImage(const cv::Mat src, cv::Mat& dest, const cv::Mat embeddedImage, const double angle, const double tx, const double ty)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	src.copyTo(dest);
	Point2d center(embeddedImage.cols / 2, embeddedImage.rows / 2);
	Mat rotateMat = cv::getRotationMatrix2D(center, angle, 1.0);
	rotateMat.at<double>(0, 2) += tx;
	rotateMat.at<double>(1, 2) += ty;
	warpAffine(embeddedImage, dest, rotateMat, dest.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

}

void outputText(string outputFilename, string embeddedImageName, int tx, int ty, Mat embedImage, int angle)
{
	int x = tx - embedImage.cols / 2;
	int y = ty - embedImage.rows / 2;

	ofstream output(outputFilename + ".txt");
	output << embeddedImageName << " " << x << " " << y << " " << embedImage.cols << " " << embedImage.rows << " " << angle << endl;
}

void generateImage(const Mat src, Mat& dest, const Mat embeddedImage, bool isSeamless = false)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	const int angle = 180;
	//const int angle = (rand() + 1.f) / (RAND_MAX + 2.f) * 360;
	const int tx = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.cols - embeddedImage.cols * 2) + embeddedImage.cols;
	const int ty = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.rows - embeddedImage.rows * 2) + embeddedImage.rows;
	//EmbedInfo ret;
	//ret.center = Point(tx + embeddedImage.cols, ty + embeddedImage.rows);
	//ret.mask = Mat::zeros(src.rows, src.cols, CV_8UC1);
	//Point pts[] = {
	//	Point(tx, ty),
	//	Point(tx, ty + embeddedImage.rows),
	//	Point(tx + embeddedImage.cols, ty + embeddedImage.rows),
	//	Point(tx + embeddedImage.cols, ty),
	//};
	//fillConvexPoly(ret.mask, pts, 4, Scalar(255));

	Mat clone;
	if (!isSeamless)
	{
		embedImage(src, dest, embeddedImage, angle, tx, ty);
	}
	else
	{
		const Mat src_mask = 255 * Mat::ones(embeddedImage.rows, embeddedImage.cols, embeddedImage.depth());
		const Point center(tx, ty);
		//seamlessClone(embeddedImage, src, src_mask, center, dest, NORMAL_CLONE);
		seamlessClone(embeddedImage, src, src_mask, center, dest, MIXED_CLONE);
	}
	imshow("emb", embeddedImage);
	//outputText(tx, ty, embeddedImage, angle);
}

void generateImageLvInf(const Mat src, Mat& dest, const Mat embeddedImage,Mat& temp, int& angle,int& tx,int& ty)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	angle = 0; //0,90,180,270
	float magnification = 0; //0.5,1.0,2.0

	int angle_ = rand() % 4;
	float magnification_ = rand() % 3;

	if (angle_ == 0)
	{
		angle = 0;
		temp = embeddedImage;
	}
	else if (angle_ == 1)
	{
		angle = 90;
		flip(embeddedImage.t(), temp, 1);
	}
	else if (angle_ == 2)
	{
		angle = 180;
		flip(embeddedImage, temp, -1);
	}
	else if (angle_ == 3)
	{
		angle = 270;
		flip(embeddedImage.t(), temp, 0);
	}

	if (magnification_ == 0)
	{
		magnification = 0.5;
		resize(temp, temp, Size(magnification*temp.cols, magnification*temp.rows));
	}
	else if (magnification_ == 1)
	{
		magnification = 1.f;
		//resize(temp, temp, Size(temp.cols, 0.5*temp.rows));
	}
	else if (magnification_ == 2)
	{
		magnification = 2.f;
		resize(temp, temp, Size(magnification*temp.cols, magnification*temp.rows));
	}

	tx = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.cols - embeddedImage.cols * 2) + embeddedImage.cols;
	ty = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.rows - embeddedImage.rows * 2) + embeddedImage.rows;

	const Mat src_mask =  255 * Mat::ones(temp.rows, temp.cols, temp.depth());
	const Point center(tx, ty);
	//seamlessClone(temp, src, src_mask, center, dest, NORMAL_CLONE);
	seamlessClone(temp, src, src_mask, center, dest, MIXED_CLONE);
	outputText("test", "test", tx, ty, temp, angle);
}

int main(int argc, char** argv)
{
	srand(time(nullptr));
	
	ostringstream filename;
	filename << "levelInf";
	_mkdir(filename.str().c_str());
	for (int i = 0; i < 10; i++)
	{
		string sname;
		int srcnum = rand() % 9;
		if (srcnum == 0) sname = "beach";
		else if (srcnum == 1) sname = "greekdome";
		else if (srcnum == 2) sname = "kodim02";
		else if (srcnum == 3) sname = "kodim06";
		else if (srcnum == 4) sname = "kodim11";
		else if (srcnum == 5) sname = "kodim16";
		else if (srcnum == 6) sname = "kodim17";
		else if (srcnum == 7) sname = "kodim20";
		else if (srcnum == 8) sname = "kodim23";

		string tname;
		int tnum = rand() % 4;
		if (tnum == 0) tname = "ocean_beach_kinzokutanchi";
		else if (tnum == 1) tname = "mokuzai_hakobu";
		else if (tnum == 2) tname = "kids_chuunibyou_girl";
		else if (tnum == 3) tname = "airgun_women_syufu";

		Mat src = imread("img/" + sname + ".png");
		Mat embededImage = imread("template/" + tname + ".png", IMREAD_ANYCOLOR);

		resize(embededImage, embededImage, Size(64, embededImage.rows * 64 / embededImage.cols));

		Mat dest;
		Mat temp;
		int angle, tx, ty = 0;
		generateImageLvInf(src, dest, embededImage, temp, angle, tx, ty);
		
		string outputName = "levelInf/levelInf_00" + to_string(i);
		cout << outputName << endl;
		outputText(outputName, tname, tx, ty, temp, angle);
		imwrite(outputName + ".png", dest);
	}


	//string sname = "beach";

	//string tname = "ocean_beach_kinzokutanchi";
	////string tname = "mokuzai_hakobu";
	////string tname = "kids_chuunibyou_girl";
	////string tname = "airgun_women_syufu";

	//Mat src = imread("img/" + sname + ".png");
	//Mat embededImage = imread("template/" + tname + ".png", IMREAD_ANYCOLOR);

	//resize(embededImage, embededImage, Size(64, (embededImage.rows * 64 / embededImage.cols)));
	//cout << "test" << embededImage.size() << endl;

	//Mat dest;
	//int tx, ty, angle;
	//Mat temp;
	//while (true)
	//{
	//	generateImageLvInf(src, dest, embededImage, temp, angle, tx, ty);
	//	//generateImage(src, dest, embededImage, true);
	//	//generateImage(src, dest, embededImage);
	//	imshow("create image", dest);
	//	//		imwrite(sname + ".ppm", dest);

	//	int key = waitKey();
	//	if (key == 'q')
	//	{
	//		break;
	//	}
	//}
	//imwrite(tname + ".ppm", embededImage);

	getchar();
	return 0;
}