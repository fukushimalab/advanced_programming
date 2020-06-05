#include <fstream>
#include <sstream>
#include <iomanip>
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

struct option {
	int angle;
	int tx;
	int ty;
};

//void embedImage(const cv::Mat src, cv::Mat& dest, const cv::Mat embeddedImage, const double angle, const double tx, const double ty)
void embedImage(const cv::Mat src, cv::Mat& dest, const cv::Mat embeddedImage, const option op)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	src.copyTo(dest);

	const int angle = op.angle;
	const int tx = op.tx;
	const int ty = op.ty;

	if (angle != 0)
	{
		Point2d center(embeddedImage.cols / 2, embeddedImage.rows / 2);
		Mat rotateMat = cv::getRotationMatrix2D(center, angle, 1.0); // 変換行列作成
		rotateMat.at<double>(0, 2) += tx; // オフセット足す
		rotateMat.at<double>(1, 2) += ty;
		warpAffine(embeddedImage, dest, rotateMat, dest.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
	}
	else
	{
		Mat roi = dest(Rect(tx, ty, embeddedImage.cols, embeddedImage.rows));
		if (embeddedImage.channels() == 4) {
			vector<Mat> p;
			split(embeddedImage, p);
			// アルファチャンネルでマスク画像生成
			Mat mask = p[3],emb;
			p.pop_back();
			// それ以外でRGB画像
			cv::merge(p, emb);
			emb.copyTo(roi, mask);
		}
		else {
			embeddedImage.copyTo(roi);
		}
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
	/*int x = tx - embedImage.cols / 2;
	int y = ty - embedImage.rows / 2;*/
	int x = tx;
	int y = ty;

	ofstream output(outputFilename + ".txt");
	output << embeddedImageName << " " << x << " " << y << " " << embedImage.cols << " " << embedImage.rows << " " << angle << flush;
}

void generateImage(const Mat src, Mat& dest, const Mat embeddedImage, option op, bool isSeamless = false)
{
	CV_Assert(!src.empty() && !embeddedImage.empty());

	const int angle = op.angle;
	//const int angle = (rand() + 1.f) / (RAND_MAX + 2.f) * 360;

	//const int tx = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.cols - embeddedImage.cols * 2) + embeddedImage.cols;
	//const int ty = (rand() + 1.f) / (RAND_MAX + 2.f) * (src.rows - embeddedImage.rows * 2) + embeddedImage.rows;
	const int tx = op.tx;
	const int ty = op.ty;

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
		embedImage(src, dest, embeddedImage, op);
	}
	else
	{
		const Mat src_mask = 255 * Mat::ones(embeddedImage.rows, embeddedImage.cols, embeddedImage.depth());
		const Point center(tx, ty);
		//seamlessClone(embeddedImage, src, src_mask, center, dest, NORMAL_CLONE);
		seamlessClone(embeddedImage, src, src_mask, center, dest, MIXED_CLONE);
	}
	//imshow("emb", embeddedImage);
	//outputText(tx, ty, embeddedImage, angle);
}

// レベルInf作成用(エラー未修正)
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

	tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols * 2) + embeddedImage.cols;
	ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows * 2) + embeddedImage.rows;

	const Mat src_mask =  255 * Mat::ones(temp.rows, temp.cols, temp.depth());
	const Point center(tx, ty);
	//seamlessClone(temp, src, src_mask, center, dest, NORMAL_CLONE);
	seamlessClone(temp, src, src_mask, center, dest, MIXED_CLONE);
	tx -= embeddedImage.cols / 2;
	ty -= embeddedImage.rows / 2;
	outputText("test", "test", tx, ty, temp, angle);
}

void addImpulseNoise(const Mat src, Mat& dest, double x) {
	dest = src.clone();
	Vec3b max = { 255,255,255 };
	Vec3b min = { 0,0,0 };
	long int size = dest.size().height * dest.size().width;
	Vec3b* ptr = dest.ptr<cv::Vec3b>(0);
	for (int i = 0; i < size; i++) {
		if ((double)rand() / RAND_MAX <= x) {
			if ((double)rand() / RAND_MAX < 0.5)
				*ptr = max;
			else
				*ptr = min;
		}
		ptr++;
	}
}

int load_image(Mat& src, const vector<string> src_names, Mat& temp, const vector<string> temp_names, bool isLevel4 = false) {
	string sname = src_names[rand() % src_names.size()];
	int tnum = rand() % temp_names.size();
	string tname = temp_names[tnum];

	src = imread("img/" + sname + ".png");
	if (isLevel4)
		temp = imread("template/" + tname + ".png", IMREAD_UNCHANGED);
	else
		temp = imread("template/" + tname + ".png", IMREAD_ANYCOLOR);

	// 入力エラー処理
	bool srcEmp, tempEmp;
	if ((srcEmp = src.empty()) | (tempEmp = temp.empty())) {
		cerr << (srcEmp ? tempEmp ? sname + ", " + tname : sname : tname) << " is not found.\n";
		assert(false);
	}
	return tnum;
}

int main(int argc, char** argv)
{
	srand(time(nullptr));

	const int genenum = 20; // 生成画像枚数

	vector<string> src_names;
	src_names.push_back("beach");
	src_names.push_back("greekdome");
	src_names.push_back("kodak/kodim02");
	src_names.push_back("kodak/kodim06");
	src_names.push_back("kodak/kodim11");
	src_names.push_back("kodak/kodim16");
	src_names.push_back("kodak/kodim17");
	src_names.push_back("kodak/kodim20");
	src_names.push_back("kodak/kodim23");

	vector<string> temp_names;
	temp_names.push_back("ocean_beach_kinzokutanchi");
	temp_names.push_back("mokuzai_hakobu");
	temp_names.push_back("kids_chuunibyou_girl");
	temp_names.push_back("airgun_women_syufu");

	// level 1 : 単一画像埋め込み
	if (true) {
		const int level = 1;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			op.angle = 0; // 角度0
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX* (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 2 : 単一画像埋め込み，インパルスノイズ付与
	if (true) {
		const int level = 2;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			op.angle = 0; // 角度0
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			// 確率0.04でインパルスノイズを付与
			addImpulseNoise(dest, dest, 0.04);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 3 : 単一画像埋め込み，コントラスト変化(0.5,1.0,1.5,2.0)
	if (true) {
		const int level = 3;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			op.angle = 0; // 角度0
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			// コントラスト
			double alpha = 0.5 * (rand() % 4 + 1.0);
			dest.convertTo(dest, -1, alpha);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 4 : 単一画像埋め込み，テンプレート背景透過
	if (true) {
		const int level = 4;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names, true);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			op.angle = 0; // 角度0
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 5 : 単一画像埋め込み，テンプレートリサイズ(0.5,1.0,2.0)
	if (true) {
		const int level = 5;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			double rate = 0.5 * pow(2.0, rand() % 3); // リサイズ比率
			resize(embeddedImage, embeddedImage, Size(), rate, rate);

			option op;
			op.angle = 0; // 角度0
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 6 : 単一画像埋め込み，テンプレート回転(0,90,180,270)
	if (true) {
		const int level = 6;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			op.angle = 90 * (rand() % 4); // 角度
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);

			embedImage(src, dest, embeddedImage, op);

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level 7 : 1~6全部入り
	if (true) {
		const int level = 7;
		ostringstream filename;
		filename << "level" << to_string(level);
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			const int lvl = rand() % 6 + 1;

			Mat src, embeddedImage, dest;
			int tnum;
			if (lvl == 4) {
				tnum = load_image(src, src_names, embeddedImage, temp_names, true);
			}
			else {
				tnum = load_image(src, src_names, embeddedImage, temp_names);
			}

			//テンプレートのリサイズ処理(大きすぎて埋め込めない)
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			option op;
			// 角度
			if (lvl == 6) {
				op.angle = 90 * (rand() % 4);
			}
			else {
				op.angle = 0; // 角度0
			}
			// 埋め込み座標ランダム
			op.tx = (rand() + 0.f) / RAND_MAX * (src.cols - embeddedImage.cols);
			op.ty = (rand() + 0.f) / RAND_MAX * (src.rows - embeddedImage.rows);
			
			if (lvl == 5) {
				// リサイズ
				double rate = 0.5 * pow(2.0, rand() % 3);
				resize(embeddedImage, embeddedImage, Size(), rate, rate);
			}

			embedImage(src, dest, embeddedImage, op);

			if (lvl == 2) {
				// 確率0.04でインパルスノイズを付与
				addImpulseNoise(dest, dest, 0.04);
			}

			if (lvl == 3) {
				// コントラスト
				double alpha = 0.5 * (rand() % 4 + 1.0);
				dest.convertTo(dest, -1, alpha);
			}

			stringstream outputNameSS;
			outputNameSS << "level" << level << "/level" << level << "_" << setfill('0') << right << setw(3) << i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], op.tx, op.ty, embeddedImage, op.angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
	}

	// level Inf : 単一画像埋め込み，Seamless Cloning，テンプレート回転(0,90,180,270)，リサイズ(0.5,1.0,2.0)
	if (false) {
		ostringstream filename;
		filename << "levelInf";
		_mkdir(filename.str().c_str());
		for (int i = 0; i < genenum; i++)
		{
			Mat src, embeddedImage, dest;
			int tnum = load_image(src, src_names, embeddedImage, temp_names);

			//テンプレートのリサイズ処理
			resize(embeddedImage, embeddedImage, Size(64, embeddedImage.rows * 64 / embeddedImage.cols));

			Mat temp;
			int angle, tx, ty;
			generateImageLvInf(src, dest, embeddedImage, temp, angle, tx, ty);

			stringstream outputNameSS;
			outputNameSS << "levelInf/levelInf_" << setfill('0') << right << setw(3) <<  i;
			cout << outputNameSS.str() << endl;
			outputText(outputNameSS.str(), temp_names[tnum], tx, ty, temp, angle);
			imwrite(outputNameSS.str() + ".png", dest);
		}
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

	cout << "press any key";
	waitKey(0);
	return 0;
}