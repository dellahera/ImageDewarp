#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

#define ii pair<int,int>

using namespace cv;
using namespace std;

clock_t t1, t2;
int Row_size, Col_size, nRow[8] = { 1,-1 }, nCol[8] = { 0,0 }, hMax=-9999999, hMin=9999999, vMax=-9999999, vMin= 9999999;
//nRow[8] = { 0,1,1,1,0,-1,-1,-1 }, nCol[8]= { -1,-1,0,1,1,1,0,-1 };
long hist[200], histW[4000], countCol[1400];
Mat HRWP, imgRGB, white, imageBinary;

//Mat countWhite(Mat src){
//	int  a = src.rows, b = src.cols, jum;
//	for (int i = 0; i < b; i++) {
//		jum = 0;
//		for (int j = 1; j < a; j++) {
//			if (src.at<Vec3b>(j, i)[0] == 150) jum++;
//		}
//		countCol[i]++;
//	}
//	for (int i = 1; i < a; i++) {
//		//cout << hist[i] << " ";
//		line(src, Point(i, 12000), Point(i, 12000 - hist[i]), Scalar(255, 0, 0), 1, 8, 0);
//	}
//	String windowNameHist = "Histogram Size";
//	namedWindow(windowNameHist, WINDOW_NORMAL);
//	imshow(windowNameHist, src);
//	bool histImg = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histCC.jpg", src);
//}

Mat findWhiteSpace(Mat src) {
	int  a = src.rows, b = src.cols;
	Mat dst = src;
	ii tmp;
	bool cek ;
	long long jum = 0;
	for (int i = 0; i < b; i++) {
		cek = false;
		for (int j = 1; j < a; j++) {
			if(cek && (dst.at<Vec3b>(j, i)[0] == 150) ) dst.at<Vec3b>(j, i)[0] = 255;
			if ((int)src.at<Vec3b>(j, i)[2] == 255 && (int)dst.at<Vec3b>(j, i)[0] == 255 && (int)src.at<Vec3b>(j-1, i)[2] != 255 ) {
				jum = 1;
				dst.at<Vec3b>(j, i)[0] = 150;
				tmp.first = j;
				tmp.second = i;
				queue < ii > myQ;
				myQ.push(tmp);
				while (myQ.size() != 0) {
					tmp = myQ.front();
					myQ.pop();
					//for (int k = 0; k < 2; k++) {
						if (tmp.first + nRow[0] >= 0 && tmp.first + nRow[0] < a && tmp.second + nCol[0] >= 0 && tmp.second + nCol[0] < b) {
							if (((int)src.at<Vec3b>(tmp.first + nRow[0], tmp.second + nCol[0])[2] == 255) && (int)dst.at<Vec3b>(tmp.first + nRow[0], tmp.second + nCol[0])[0] == 255) {
								jum++;
								hMax = max(hMax, tmp.first + nRow[0]);
								hMin = min(hMin, tmp.first + nRow[0]);
								vMax = max(vMax, tmp.second + nCol[0]);
								vMin = min(vMin, tmp.second + nCol[0]);
								//if (jum >= 7 && jum <= 19) white.at<uchar>(tmp.first + nRow[k], tmp.second + nCol[k]) = 125;
								dst.at<Vec3b>(tmp.first + nRow[0], tmp.second + nCol[0])[0] = 150;
								myQ.push(make_pair(tmp.first + nRow[0], tmp.second + nCol[0]));
								if (tmp.first + nRow[0] == a - 1) cek = true;
							}
						}
					//}
				}
				//cout << i << "-" << j <<" -"<<jum<<endl;
				//if (jum < 3) white.at<uchar>(i, j) = 255;
				histW[jum]++;
			}
		}
	}

	cout << endl;
	for (int k = 1; k < 100; k++) {
		cout << histW[k] << " ";
		if (k % 10 == 0) cout << endl;
	}
	return dst;
}

Mat whiteArea(Mat src) {
	for (int i = hMin; i <= hMax; i++) {
		for (int j = vMin; j <= vMax; j++) {
			if ((int)src.at<Vec3b>(i, j)[2] == 255)
				src.at<Vec3b>(i, j)[0] = 150;
		}
	}
	return src;
}

Mat find_component() {
	int  a = imageBinary.rows, b = imageBinary.cols;
	ii tmp;
	long long jum = 0, cc = 0;
	for (int i = 0; i <a; i++) {
		for (int j = 0; j < b; j++) {
			if (((int)imageBinary.at<uchar>(i, j) == 0) && (int)white.at<uchar>(i, j) == 255 ) {
				cc++;
				jum = 1;
				white.at<uchar>(i, j) = 0;
				tmp.first =i;
				tmp.second = j; 
				queue < ii > myQ;
				myQ.push(tmp);
				while (myQ.size() != 0) {
					tmp = myQ.front();
					myQ.pop();
					//for (int k = 0; k < 2; k++) {
						if (tmp.first + nRow[0] >= 0 && tmp.first + nRow[0] < a && tmp.second + nCol[0] >= 0 && tmp.second + nCol[0] < b) {
							if (((int)imageBinary.at<uchar>(tmp.first + nRow[0], tmp.second + nCol[0]) == 0) && (int)white.at<uchar>(tmp.first + nRow[0], tmp.second + nCol[0]) == 255) {
								jum++;
								hMax = max(hMax, tmp.first + nRow[0]);
								hMin = min(hMin, tmp.first + nRow[0]);
								vMax = max(vMax, tmp.second + nCol[0]);
								vMin = min(vMin, tmp.second + nCol[0]);
								//if (jum >= 7 && jum <= 19) white.at<uchar>(tmp.first + nRow[k], tmp.second + nCol[k]) = 125;
								white.at<uchar>(tmp.first + nRow[0], tmp.second + nCol[0]) = 0;
								myQ.push(make_pair(tmp.first + nRow[0], tmp.second + nCol[0]));
							}
						}
					//}
				}
				//cout << i << "-" << j <<" -"<<jum<<endl;
				if (jum < 3) white.at<uchar>(i, j) = 255;
				hist[jum]++;
			}
			cc = max(cc, jum);
		}
	}
	cout << endl << cc << endl;
	for (int k = 1; k < 100; k++) {
		cout << hist[k] << " ";
		if (k % 10 == 0) cout << endl;
	}
	return white;
}

void drawHist(Mat src) {
	for (int i = 1; i <= 99; i++) {
		//cout << hist[i] << " ";
		line(src, Point( i, 12000), Point(i, 12000 - hist[i]), Scalar(255, 0, 0), 1, 8, 0);
	}
	String windowNameHist = "Histogram Size";
	namedWindow(windowNameHist, WINDOW_NORMAL);
	imshow(windowNameHist, src);
	bool histImg = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histCC.jpg", src);
}

Mat RunRLSA(Mat src, Mat dst, int value, int key) {
	int counts, a = src.rows, b = src.cols, jumPix=0;
	//vertikal
	if(key==1){
		for (int i = 0; i < b; i++) {
			counts = 0;
			//jumPix = 0;
			for (int j = 0; j < a; j++) {
				if ((int)src.at<uchar>(j, i) == 0 ) {
					//cout << 1;
						if ((j - counts) <= 23 && (j - counts) >= 0 ) {
						//cout << 2;
						for (int k = counts; k <= j; k++) {	
							if ((int)src.at<uchar>(k, i) != 0 && (!((int)src.at<uchar>(k, i - 1) != 0 && (int)src.at<uchar>(k, i + 1) != 0))|| j-counts<7) {
								dst.at<uchar>(k, i) = 0;
								//if ((int)src.at<uchar>(k, i) != 0) dst.at<Vec3b>(k, i) = (0, 0, 0);
								src.at<uchar>(k, i) = 0;
							}
						}
					}
					
					counts = j;
				}
			}
		}
	}
	else if (key == 2) {
		//horizontal
		for (int i = 0; i < a; i++) {
			if ((int)src.at<uchar>(i, 0) == 0) counts = 0;
			for (int j = 0; j < b; j++) {
				if ((int)src.at<uchar>(i, j) == 0) {
					//cout << 1;
					jumPix++;
					if (jumPix == 1) counts = j;
					if ((j - counts) <= 14 && (j - counts) >= 0 && jumPix>1) {
						//cout << 2;
						for (int k = counts; k <= j; k++) {
							if ((int)src.at<uchar>(i, k) != 0) dst.at<uchar>(i, k) = 0;
							src.at<uchar>(i, k) = 0;
						}
					}
					counts = j;
				}
			}
		}
	}
	else {
		//horizontal
		for (int i = 0; i < a; i++) {
			for (int j = 0; j < b; j++) {
				if ((int)src.at<uchar>(i, j) == 0) {
					//cout << 1;
					jumPix++;
					if (jumPix == 1) counts = 1;
					if ((j - counts) <= 61 && (j - counts) >= 0 && jumPix>1) {
						//cout << 2;
						for (int k = counts; k <= j; k++) {
							if ((int)src.at<uchar>(i, k) != 0) dst.at<Vec3b>(i, k)[2] = 150;
							src.at<uchar>(i, k) = 0;
						}
					}
					counts = j;
				}
			}
		}
	}
	return dst;
}
int main(int argc, char** argv)
{
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	//Membaca Gambar
	Mat image = imread("C:/Users/della_hera/Documents/Hasil Proses/BukuA.jpg", IMREAD_GRAYSCALE);
	//image.resize(500, );
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	bool isGrayscale = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra GrayscaleA.jpg", image);

	// Create a window
	String windowName = "Citra Buku"; 
	//namedWindow(windowName, WINDOW_NORMAL);
	//imshow(windowName, image); 

	//increase the brightness
	Mat imageBrighness;
	image.convertTo(imageBrighness, -1, 1, 60);  
	String windowNameBrighness = "Brighness Increased";
	//namedWindow(windowNameBrighness, WINDOW_NORMAL);
	//imshow(windowNameBrighness, imageBrighness);
	bool isBrighness = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra BrighnessA.jpg", imageBrighness);

	//increase the contrast by 1.5
	Mat imageContrast;
	imageBrighness.convertTo(imageContrast, -1, 1.5, 0);
	String windowNameContrast = "Contrast Increased";
	//namedWindow(windowNameContrast, WINDOW_NORMAL);
	//imshow(windowNameContrast, imageContrast);
	bool isContrast = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Kontras BrighnessA.jpg", imageContrast);

	//equalize the histogram
	/*
	Mat hist_equalized_image;
	equalizeHist(imageContrast, hist_equalized_image);
	String windowNameOfHistogramEqualized = "Histogram Equalized Image";
	namedWindow(windowNameOfHistogramEqualized, WINDOW_NORMAL);
	imshow(windowNameOfHistogramEqualized, hist_equalized_image);
	*/
	int Row_size = imageContrast.rows;
	int Col_size = imageContrast.cols;

	//binary image
	adaptiveThreshold(imageContrast, imageBinary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 2);
	bool isBiner = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Biner.jpg", imageBinary);
	String windowNameBinary = "Binary Image";
	/*namedWindow(windowNameBinary, WINDOW_NORMAL);
	imshow(windowNameBinary, imageBinary);
	bool isBiner = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Biner.jpg", imageBinary);*/
	Mat black(Row_size, Col_size, CV_8UC3, Scalar(255, 255, 255));
	Mat histogram(12000, 60, CV_8UC3, Scalar(255, 255, 255));
	cvtColor(black, white, COLOR_RGB2GRAY);

	t1 = clock();
	imageBinary = RunRLSA(imageBinary, imageBinary, 8, 2);
	imageBinary = RunRLSA(imageBinary, imageBinary, 8, 1);
	bool isVert = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Deteksi1.jpg", imageBinary);
	t2 = clock();
	cout << "Words Detection = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << endl;


	white = find_component();
	drawHist(histogram);

	cvtColor(white, imgRGB, COLOR_GRAY2RGB);

	String windowNameCC = "Connected Component Image";
	/*namedWindow(windowNameCC, WINDOW_NORMAL);
	imshow(windowNameCC, white);*/
	bool isCC = imwrite("C:/Users/della_hera/Documents/Hasil Proses/CC.jpg", white);

	t1 = clock();
	Mat imgLine = RunRLSA(white, imgRGB, 8, 3);
	bool isLine = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Deteksi Garis.jpg", imgLine);
	t2 = clock();
	cout << "\nLine = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << endl;

	//cvtColor(imgLine, white, COLOR_RGB2GRAY);
	t1 = clock();
	//HRWP = findWhiteSpace(imgLine);
	HRWP = whiteArea(imgLine);
	t2 = clock();
	cout << "\n\nWhite space = ";
	cout <<  "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds\n" << endl;
	String windowNameDetect = "Horizontal Detected Image";
	//namedWindow(windowNameDetect, WINDOW_NORMAL);
	//imshow(windowNameDetect, imgRGB);
	bool isWhite = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Deteksi Spasi.jpg", HRWP);

	cout << Row_size << " " << Col_size <<" " <<hMax<<" "<<hMin<<" "<<vMax<<" "<<vMin;
	waitKey(0); 
	destroyAllWindows();
}
