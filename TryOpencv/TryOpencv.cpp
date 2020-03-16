//newlebih lurus
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

#define ii pair<int,int>

using namespace cv;
using namespace std;

clock_t t1, t2;
int Row_size, Col_size, cumValue, yHorizontal, xNormal, yMax = -9999999, yMin = 9999999, xMax = -9999999, xMin = 9999999, minYLeftbottom = 9999999, maxYLeftTop = -9999999;
//nRow[8] = { 0,1,1,1,0,-1,-1,-1 }, nCol[8]= { -1,-1,0,1,1,1,0,-1 };
long hist[200], histW[4000], countCol[1400], botttomLine[2000];
float A, B;
int nRow[8] = { 1,-1 }, nCol[8] = { 0,0 }, posYbottom[4000], posYtop[4000];
Mat HRWP, imgRGB, white, imageBinary;
vector<int> posWhiteTop[2000];
//vector<int> posWhiteBottom[2000];
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
	bool cek;
	long long jum = 0;
	for (int i = 0; i < b; i++) {
		cek = false;
		for (int j = 1; j < a; j++) {
			if (cek && (dst.at<Vec3b>(j, i)[0] == 150)) dst.at<Vec3b>(j, i)[0] = 255;
			if ((int)src.at<Vec3b>(j, i)[2] == 255 && (int)dst.at<Vec3b>(j, i)[0] == 255 && (int)src.at<Vec3b>(j - 1, i)[2] != 255) {
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
							yMax = max(yMax, tmp.first + nRow[0]);
							yMin = min(yMin, tmp.first + nRow[0]);
							xMax = max(xMax, tmp.second + nCol[0]);
							xMin = min(xMin, tmp.second + nCol[0]);

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
	for (int i = yMin; i <= yMax; i++) {
		for (int j = xMin; j <= xMax; j++) {
			if ((int)src.at<Vec3b>(i, j)[2] == 255) {
				src.at<Vec3b>(i, j) = { 0, 255, 255 };
			}
		}
	}
	return src;
}

Mat RunRLSAWhite(Mat src) {
	int counts;
	Mat dst = src;
	bool cek;
	Vec3b color;
	//vertikal
	for (int i = xMin; i <= xMax; i++) {
		counts = 0;
		cek = false;
		for (int j = yMin; j <= yMax; j++) {
			if ((int)src.at<Vec3b>(j, i)[2] == 255) {
				if ((int)dst.at<Vec3b>(j, i)[0] == 255 && ((int)src.at<Vec3b>(j - 1, i)[2] == 150 || (int)src.at<Vec3b>(j - 1, i)[2] == 0) && j > 0 && !cek) {
					counts = j;
					cek = true;
				}
				else if (counts != 0 && cek) {
					if ((j - counts) <= 8 && (j - counts) >= 0) {
						//cout << 2;
						for (int k = counts; k <= j; k++) {
							dst.at<Vec3b>(k, i) = { 0, 255, 255 };
						}
					}
				}
				counts = j;
			}
		}
	}
	for (int i = xMin; i <= xMax; i++) {
		for (int j = yMin; j <= yMax; j++) {
			if ((int)src.at<Vec3b>(j, i)[2] == 150 || (int)src.at<Vec3b>(j, i)[2] == 0) botttomLine[i] = j;
		}
		for (int j = botttomLine[i] + 1; j < src.rows; j++) {
			dst.at<Vec3b>(j, i) = { 255, 255, 255 };
		}
	}
	return dst;
}

//std::vector<int> indices = { 0, 1, 0, 1 };
//cv::Mat dest_mat(source_mat.rows, source_mat.cols - 1, source_mat.type());
//int idx = 0;
//int dest_col = 0;
//for (int i = 0; i < source_mat.rows; ++i)
//{
//	int* dest_row = dest_mat.ptr<int>(i);
//	const int* source_row = source_mat.ptr<int>(i);
//	dest_col = 0;
//	for (int j = 0; j < source_mat.cols; ++j)
//	{
//		if (j != indices[idx])
//		{
//			dest_row[dest_col] = source_row[j];
//			++dest_col;
//		}
//		else
//			++idx;
//	}
//}
Mat translation(Mat source) {
	vector<int> pos[2000];
	int a = source.rows, b = source.cols;
	Mat dst(a, b, source.type(), Scalar(255, 255, 255));
	/*for (int i = xMin; i <= xMax; i++) {
		for (int j = 1; j <= 20; j++) {
			if (j <= posWhite[i].size())
				pos[i].push_back(posWhite[i][j]);
		}
	}*/
	int idx, dstCol;
	for (int i = xMin; i <= xMax; i++) {
		//	cout << endl << "vertikal";
		dstCol = yHorizontal;
		idx = 0;
		for (int j = yHorizontal; j >= yMin; j--) {
			if (posWhiteTop[i].size() == 0) {
				dst.at<uchar>(j, i) = source.at<uchar>(j, i);
				/*if (imageBinary.at<uchar>(j, i) == 0) dst.at<Vec3b>(j, i) = { 0,0,0 };
				else dst.at<Vec3b>(j, i) = { 255,255,255 };*/
			}
			else if (posWhiteTop[i].size() != 0 && j != posWhiteTop[i][idx]) {
				dst.at<uchar>(dstCol, i) = source.at<uchar>(j, i);
				dstCol--;
			}
			else if (idx < posWhiteTop[i].size() - 1) {
				//		cout << posWhiteTop[i][idx] << " ";
				//dst.at<Vec3b>(posWhiteTop[i][idx], i) = { 209, 18, 193 };
				idx++;
			}
			//else idx = posWhiteTop[i].size() - 1;
		}
		if (idx < posWhiteTop[i].size() - 1) idx++;
		dstCol = yHorizontal + 1;
		//	cout << "\nbottom";

		for (int j = yHorizontal + 1; j <= yMax; j++) {
			if (posWhiteTop[i].size() == 0) {
				dst.at<uchar>(j, i) = source.at<uchar>(j, i);
				/*if (imageBinary.at<uchar>(j, i) == 0) dst.at<Vec3b>(j, i) = { 0,0,0 };
				else dst.at<Vec3b>(j, i) = { 255,255,255 };*/
			}
			else if (posWhiteTop[i].size() != 0 && j != posWhiteTop[i][idx]) {
				dst.at<uchar>(dstCol, i) = source.at<uchar>(j, i);
				dstCol++;
			}
			else if (idx < posWhiteTop[i].size() - 1) {
				//		cout << posWhiteTop[i][idx] << " ";
				//dst.at<Vec3b>(posWhiteTop[i][idx], i) = { 209, 18, 193 };
				idx++;
			}
			else idx = posWhiteTop[i].size() - 1;
		}
		/*for (int j = yMin; j <= yMax; j++) {
			if (imageBinary.at<uchar>(j, i) == 0) dst.at<Vec3b>(j, i) = { 0,0,0 };
		}*/
	}
	return dst;
}
Mat find_component() {
	int  a = imageBinary.rows, b = imageBinary.cols;
	ii tmp;
	long long jum = 0, cc = 0;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			if (((int)imageBinary.at<uchar>(i, j) == 0) && (int)white.at<uchar>(i, j) == 255) {
				cc++;
				jum = 1;
				white.at<uchar>(i, j) = 0;
				tmp.first = i;
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
							yMax = max(yMax, tmp.first + nRow[0]);
							yMin = min(yMin, tmp.first + nRow[0]);
							xMax = max(xMax, tmp.second + nCol[0]);
							xMin = min(xMin, tmp.second + nCol[0]);
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

void posWhiteROI(Mat src) {
	//top
	int histW[2000], histAll[2000], ctr;
	Mat histoW(3000, 2000, CV_8UC3, Scalar(255, 255, 255));
	Mat histoAll(3000, 2000, CV_8UC3, Scalar(255, 255, 255));
	for (int i = xMin; i <= xMax; i++) {
		ctr = 0;
		for (int j = yHorizontal; j >= yMin; j--) {
			if (src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255) {
				posWhiteTop[i].push_back(j);
				ctr++;
			}
			else if (src.at<Vec3b>(j, i)[2] == 150 || src.at<Vec3b>(j, i)[2] == 0) ctr++;
		}
		for (int j = yHorizontal; j <= yMax; j++) {
			if (src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255) {
				posWhiteTop[i].push_back(j);
				ctr++;
			}
			else if (src.at<Vec3b>(j, i)[2] == 150 || src.at<Vec3b>(j, i)[2] == 0) ctr++;
		}
		histAll[i] = ctr;
		histW[i] = posWhiteTop[i].size();// +posWhiteBottom[i].size();
	}

	for (int i = xMin; i <= xMax; i++) {
		//cout << hist[i] << " ";
		line(histoW, Point(i, 3000), Point(i, 3000 - histW[i]), Scalar(255, 0, 0), 1, 8, 0);
		line(histoAll, Point(i, 3000), Point(i, 3000 - histAll[i]), Scalar(255, 0, 0), 1, 8, 0);
	}
	String windowNameHistW = "Histogram White Size";
	namedWindow(windowNameHistW, WINDOW_NORMAL);
	imshow(windowNameHistW, histoW);
	bool histWhite = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histWHitePos.jpg", histoW);

	String windowNameHistROI = "Histogram ROI Size";
	namedWindow(windowNameHistROI, WINDOW_NORMAL);
	imshow(windowNameHistROI, histoAll);
	bool histROI = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histROI.jpg", histoAll);
	//bottom

}
void drawHist(Mat src) {
	for (int i = 1; i <= 99; i++) {
		//cout << hist[i] << " ";
		line(src, Point(i, 12000), Point(i, 12000 - hist[i]), Scalar(255, 0, 0), 1, 8, 0);
	}
	String windowNameHist = "Histogram Size";
	namedWindow(windowNameHist, WINDOW_NORMAL);
	imshow(windowNameHist, src);
	bool histImg = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histCC.jpg", src);
}

Mat RunRLSA(Mat src, Mat dst, int value, int key) {
	int counts, a = src.rows, b = src.cols, jumPix = 0;
	//horizontal
	if (key == 1) {
		for (int i = 0; i < b; i++) {
			counts = 0;
			//jumPix = 0;
			for (int j = 0; j < a; j++) {
				if ((int)src.at<uchar>(j, i) == 0) {
					//cout << 1;
					if ((j - counts) <= 26 && (j - counts) >= 0) {
						//cout << 2;
						for (int k = counts; k <= j; k++) {
							if ((int)src.at<uchar>(k, i) != 0 && (!((int)src.at<uchar>(k, i - 1) != 0 && (int)src.at<uchar>(k, i + 1) != 0)) || j - counts < 7) {
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
		//vertikal
		for (int i = 0; i < a; i++) {
			if ((int)src.at<uchar>(i, 0) == 0) counts = 0;
			for (int j = 0; j < b; j++) {
				if ((int)src.at<uchar>(i, j) == 0) {
					//cout << 1;
					jumPix++;
					if (jumPix == 1) counts = j;
					if ((j - counts) <= 16 && (j - counts) >= 0 && jumPix > 1) {
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
					if ((j - counts) <= 75 && (j - counts) >= 0 && jumPix > 1) {
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
Mat clear(Mat src) {
	Mat dst = imageBinary;
	for (int i = xMin; i <= xMax; i++) {
		for (int j = yMin; j <= yMax; j++) {
			if (src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255)
				dst.at<uchar>(j, i) = 255;
		}
	}
	return dst;
}
void equalizeRegression(Mat src) {
	long long n, sumX = 0, sumY = 0, sumXY = 0, sumSQRx = 0, sumXsqr = 0;
	int center = 0, cek, top = 0, bottom = 0;
	//topside
	for (int i = xMin + 65; i <= xMax - 3; i++) {
		cek = 0;
		for (int j = yMin; j <= yHorizontal; j++) {
			if (!(src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255)) {
				if ((src.at<Vec3b>(j - 1, i)[0] == 0 || src.at<Vec3b>(j - 1, i)[0] == 255) && src.at<Vec3b>(j - 1, i)[1] == 255 && src.at<Vec3b>(j - 1, i)[2] == 255) {
					bottom = j;
					cek++;
				}
				if ((src.at<Vec3b>(j + 1, i)[0] == 0 || src.at<Vec3b>(j + 1, i)[0] == 255) && src.at<Vec3b>(j + 1, i)[1] == 255 && src.at<Vec3b>(j + 1, i)[2] == 255) {
					top = j;
					cek++;
				}
				if (cek == 2) {
					if (top - bottom > 4)
						center = (top + bottom) / 2;
					if (posYtop[i - 1] - center > 40 && i > xMin + 65) {
						cek = 0;
						continue;
					}
					maxYLeftTop = max(maxYLeftTop, center);
					/*sumX += i;
					sumY += center;
					sumXY += (i * center);
					sumSQRx += (i * i);*/
					posYtop[i] = center;
					//	cout << i << " " <<top<<" "<<bottom<<" "<< center << endl;
					break;
				}
			}
		}
	}
	//bottom side

	for (int i = xMin; i <= xMax - 50; i++) {
		cek = 0;
		for (int j = yMax; j > yHorizontal; j--) {
			if (!(src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255)) {
				if (src.at<Vec3b>(j - 1, i)[0] == 0 && src.at<Vec3b>(j - 1, i)[1] == 255 && src.at<Vec3b>(j - 1, i)[2] == 255) {
					bottom = j;
					cek++;
				}
				else if (src.at<Vec3b>(j + 1, i)[0] == 0 && src.at<Vec3b>(j + 1, i)[1] == 255 && src.at<Vec3b>(j + 1, i)[2] == 255) {
					top = j;
					cek++;
				}
				if (cek == 2) {
					center = (top + bottom) / 2;

					minYLeftbottom = min(minYLeftbottom, center);
					/*sumX += i;
					sumY += center;
					sumXY += (i * center);
					sumSQRx += (i * i);*/
					posYbottom[i] = center;
					//	cout << i << " " << top << " " << bottom << " " << center << endl;
					break;
				}
			}
			else cek = 0;
		}
	}
	/*n = (xMax - 100) - xMin + 1;
	sumXsqr = sumX * sumX;
	B = (n * sumXY - (sumX * sumY)) / ((n * sumSQRx) - sumXsqr);
	A = (sumX / n) - B * (sumY / n);*/
	cout << "regre " << sumX << " " << sumY << " " << sumXY << " " << sumSQRx << " " << sumXsqr << " " << A << " " << B << " " << maxYLeftTop << endl;
}
int xRegression(int x) {
	//int sumFirstPixel = ((-value) * (x - (xMin)) / ((xMax + 50) - (xMin))) + value;
	int sumFirstPixel = A + B * x;
	return sumFirstPixel;
}

int yRegression(int y, int value) {
	int sumPixelY;
	if (y <= yHorizontal) {
		sumPixelY = value - (value * (y - yMin) / ((yHorizontal)-yMin));
	}
	else {
		sumPixelY = (value * (y - (yHorizontal)) / (yMax - (yHorizontal)));
	}
	return sumPixelY;
}


void deleteWhitePixel(Mat src, int value) {
	int histW[2000], top, bottom, center, cek, sumPix, dif, count = 0;
	Mat histoW(50, 2000, CV_8UC3, Scalar(255, 255, 255));
	bool tes = 0;
	//int limit = (xMax - xMin) / value;
	ofstream myfile("output.txt");

	for (int i = xMin; i <= xMax; i++) {
		if (myfile.is_open())
		{
			myfile << endl << i << " total = ";//<< xRegression(i);
		/*if (count == limit) {
			value--;
			count = 0;
		}
		count++;*/
			cek = 0;
			cumValue = 0;
			if (i > xMin + 65 && abs(posYtop[i] - posYtop[i - 1]) > 1) posYtop[i] = (posYtop[i] + posYtop[i - 1]) / 2;
			for (int j = yHorizontal; j >= yMin; j--) {
				if (src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255) {
					if (j == yHorizontal) {
						bottom = j;
						cek++;
					}
					else if (src.at<Vec3b>(j + 1, i)[0] == 255 || src.at<Vec3b>(j + 1, i)[2] == 150 || src.at<Vec3b>(j + 1, i)[2] == 0) {
						bottom = j;
						cek++;
					}
					if (src.at<Vec3b>(j - 1, i)[0] == 255 || src.at<Vec3b>(j - 1, i)[2] == 150 || src.at<Vec3b>(j - 1, i)[2] == 0) {
						top = j;
						cek++;
					}
					if (cek == 2) {
						center = (top + bottom) / 2;
						/*if (bottom-top <= 2) {
							cek = 0;
							continue;
						}*/
						//sumPix = yRegression(center, (xRegression(i, value)));
						//value = xRegression(i) - minimum;

						value = maxYLeftTop - posYtop[i];
						if (i <= xMin + 67) value = maxYLeftTop - posYtop[xMin + 67];
						else if (i >= xMax - 3) value = 0;
						//cout << value << " ";
						sumPix = yRegression(center, value);
						//cout << center << " ";
						//if (tes) sumPix++;
						myfile << sumPix << " ";
						if (cumValue > sumPix) sumPix = 0;
						else sumPix -= cumValue;
						cumValue += sumPix;

						//if (i == xMax - 30) sumPix += value;;
						cek = 0;
						if (sumPix > 0) {
							for (int k = top + sumPix - 1; k >= top; k--) {
								//if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)
								posWhiteTop[i].push_back(k);

							}
							//for (int k = center + ((sumPix - 1) / 2); k >= center - ((sumPix - 1) / 2); k--) {
							//	//if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)
							//	posWhiteTop[i].push_back(k);
							//	//if (i <= 344 || i <= 345) cout << k << " ";
							//}
							//dif = bottom - top - sumPix+1;
							//for (int k = top + (dif / 2) + sumPix - 1; k >= top + (dif / 2); k--) {
							//	if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)  posWhiteTop[i].push_back(k);
							//	//cout << k << " ";
							//}
							/*dif = bottom - top - sumPix + 1;
							if (dif >0) {
							for (int k = top + (dif / 2) + sumPix - 1; k >= top + (dif / 2); k--) {
								if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)  posWhiteTop[i].push_back(k);
							}
							}
							else {
								for (int k = bottom; k >= top; k--) {
									if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)  posWhiteTop[i].push_back(k);
								}
							}*/
						}
					}
				}
				else cek = 0;
			}
			//	cout << endl;
			cumValue = 0;
			cek = 0;
			//if (i > xMin&& abs(posYbottom[i] - posYbottom[i - 1]) > 1) posYbottom[i] = (posYbottom[i - 1] + posYbottom[i]) / 2;
			for (int j = yHorizontal + 1; j <= yMax; j++) {
				if (src.at<Vec3b>(j, i)[0] == 0 && src.at<Vec3b>(j, i)[1] == 255 && src.at<Vec3b>(j, i)[2] == 255) {
					if (j == yHorizontal) {
						top = j;
						cek++;
					}
					else if (src.at<Vec3b>(j - 1, i)[0] == 255 || src.at<Vec3b>(j - 1, i)[2] == 150 || src.at<Vec3b>(j - 1, i)[2] == 0) {
						top = j;
						cek++;
					}
					if (src.at<Vec3b>(j + 1, i)[0] == 255 || src.at<Vec3b>(j + 1, i)[2] == 150 || src.at<Vec3b>(j + 1, i)[2] == 0) {
						bottom = j;
						cek++;
					}
					if (cek == 2) {
						sumPix = 0;
						center = (bottom + top) / 2;
						value = posYbottom[i] - minYLeftbottom;
						if (value < 0) value = 0;
						//sumPix = yRegression(center, (xRegression(i, value)));
						sumPix = yRegression(center, value);
						if (sumPix <= 0) sumPix = 0;
						//cout << value << " ";
						//if (i >= 313 && i <= 316) cout << sumPix << " ";
						if (cumValue > sumPix) sumPix = 0;
						else sumPix -= cumValue;
						cumValue += sumPix;
						cek = 0;
						if (sumPix > 0) {
							//for (int k = center - ((sumPix - 1) / 2); k <= center + ((sumPix - 1) / 2); k++) {
							//	//	if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255) 
							//	posWhiteTop[i].push_back(k);
							//	//if(i<=344 || i <= 345) cout << k << " ";
							//}
							for (int k = bottom - sumPix + 1; k <= bottom; k++) {
								//if (src.at<Vec3b>(k, i)[0] == 0 && src.at<Vec3b>(k, i)[1] == 255 && src.at<Vec3b>(k, i)[2] == 255)
								posWhiteTop[i].push_back(k);
								//if (i <= 344 || i <= 345) cout << k << " ";
							}
						}
					}
				}
				else cek = 0;
			}
		}
		histW[i] = posWhiteTop[i].size();// +posWhiteBottom[i].size();
	}
	myfile.close();

	for (int i = xMin; i <= xMax; i++) {
		//cout << hist[i] << " ";
		line(histoW, Point(i, 50), Point(i, 50 - histW[i]), Scalar(255, 0, 0), 1, 8, 0);
	}
	String windowNameHistW = "Histogram White Size";
	namedWindow(windowNameHistW, WINDOW_NORMAL);
	imshow(windowNameHistW, histoW);
	bool histWhite = imwrite("C:/Users/della_hera/Documents/Hasil Proses/histWHitePos.jpg", histoW);
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
	Mat imageDetect = imageBinary;
	bool isBiner = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Biner.jpg", imageBinary);
	String windowNameBinary = "Binary Image";
	/*namedWindow(windowNameBinary, WINDOW_NORMAL);
	imshow(windowNameBinary, imageBinary);
	bool isBiner = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Biner.jpg", imageBinary);*/
	Mat black(Row_size, Col_size, CV_8UC3, Scalar(255, 255, 255));
	Mat histogram(12000, 60, CV_8UC3, Scalar(255, 255, 255));
	cvtColor(black, white, COLOR_RGB2GRAY);
	t1 = clock();
	imageDetect = RunRLSA(imageDetect, imageDetect, 8, 2);
	imageDetect = RunRLSA(imageDetect, imageDetect, 8, 1);
	bool isVert = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Deteksi1.jpg", imageDetect);
	t2 = clock();
	cout << "Words Detection = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << endl;


	white = find_component();
	yHorizontal = (yMax + yMin) / 2;
	xNormal = (xMax + xMin) / 2;
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
	HRWP = RunRLSAWhite(imgLine);
	HRWP = whiteArea(imgLine);
	t2 = clock();
	cout << "\n\nWhite space = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds\n" << endl;
	String windowNameDetect = "Horizontal Detected Image";
	//namedWindow(windowNameDetect, WINDOW_NORMAL);
	//imshow(windowNameDetect, imgRGB);
	bool isWhite = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Deteksi Spasi11.jpg", HRWP);

	//posWhiteROI(HRWP);
	cout << yRegression(yMin, 1) << "Test " << posWhiteTop[xMin].size() << " " << yRegression(yHorizontal, 1) << " ";// << xRegression(xMin, 15) << " " << xRegression(xMin + 50, 15) << " " << xRegression(xMax, 15) << endl;
	t1 = clock();
	equalizeRegression(HRWP);
	deleteWhitePixel(HRWP, 150);
	t2 = clock();
	cout << "\n\nPosition White space = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds\n" << endl;
	adaptiveThreshold(imageContrast, imageBinary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 2);
	t1 = clock();
	imageBinary = clear(HRWP);
	Mat output = translation(imageBinary);
	t2 = clock();
	cout << "\n\nTranslation = ";
	cout << "Time " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds\n" << endl;
	bool isBinerTes = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Citra Binerhaha.jpg", imageBinary);
	bool OutputTes = imwrite("C:/Users/della_hera/Documents/Hasil Proses/Output.jpg", output);
	cout << Row_size << " " << Col_size << " " << yMax << " " << yMin << " " << xMax << " " << xMin << " " << yHorizontal << " " << endl;
	waitKey(0);
	destroyAllWindows();
}
