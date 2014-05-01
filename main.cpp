#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace cv;

#define POKTMPLPATH "/Users/xiaohang/Test/playok/templates/"
#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define RT(x) (" \033[1;31m" + string(x) + "\033[0m")
#define BT(x) (" \033[1;30m" + string(x) + "\033[0m")
#define RT_BKG(x) (" \033[42;31m" + string(x) + "\033[0m")
#define BT_BKG(x) (" \033[42;30m" + string(x) + "\033[0m")
#define BLANK_BKG " \033[42;30m  \033[0m"
#define BLANK "   "

string infile;
IplImage ** tmplImgs;

string chessnames[14] = {"卒", "車", "馬", "象", "士", "将", "炮",
	"兵", "俥", "傌", "相", "仕", "帅", "炮"};
char fennames[14] = {
    'p', 'r', 'n', 'b', 'a', 'k', 'c',
    'P', 'R', 'N', 'B', 'A', 'K', 'C'};

enum {BLACK_ZU = 0, BLACK_CHE, BLACK_MA, BLACK_XIANG, BLACK_SHI, BLACK_JIANG, BLACK_PAO,
	RED_ZU, RED_CHE, RED_MA, RED_XIANG, RED_SHI, RED_JIANG, RED_PAO};

IplImage * cropImage(IplImage * src, int x, int y, int width, int height)
{
	cvSetImageROI(src, cvRect(x, y, width , height));
	IplImage * dst = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U , src->nChannels);
	cvCopy(src, dst, 0);
    cvResetImageROI(src);
	return dst;
}

string matrix2fenstr(char matrix[10][9])
{
    ostringstream oss;
    char c;
    for(int j = 0; j < 10; j++)
    {
        int count = 0;
        for(int i = 0; i < 9; i++)
        {
            if(matrix[j][i] == -1) count++;
            else
            {
                if(count > 0)
                {
                    oss<<count;
                    count = 0;
                }
                oss<<fennames[matrix[j][i]];
            }
        }
        if(count>0)
        {
            oss<<count;
        }
        if(j!=9) oss<<'/';
    }
    return oss.str();
}

void Init(double xratio, double yratio)
{
	tmplImgs = new IplImage * [14];
	tmplImgs[BLACK_ZU] = cvLoadImage(POKTMPLPATH"black_zu.png", 1);
	tmplImgs[BLACK_CHE] = cvLoadImage(POKTMPLPATH"black_che.png", 1);
	tmplImgs[BLACK_MA] = cvLoadImage(POKTMPLPATH"black_ma.png", 1);
	tmplImgs[BLACK_XIANG] = cvLoadImage(POKTMPLPATH"black_xiang.png", 1);
	tmplImgs[BLACK_SHI] = cvLoadImage(POKTMPLPATH"black_shi.png", 1);
	tmplImgs[BLACK_JIANG] = cvLoadImage(POKTMPLPATH"black_jiang.png", 1);
	tmplImgs[BLACK_PAO] = cvLoadImage(POKTMPLPATH"black_pao.png", 1);
	tmplImgs[RED_ZU] = cvLoadImage(POKTMPLPATH"red_zu.png", 1);
	tmplImgs[RED_CHE] = cvLoadImage(POKTMPLPATH"red_che.png", 1);
	tmplImgs[RED_MA] = cvLoadImage(POKTMPLPATH"red_ma.png", 1);
	tmplImgs[RED_XIANG] = cvLoadImage(POKTMPLPATH"red_xiang.png", 1);
	tmplImgs[RED_SHI] = cvLoadImage(POKTMPLPATH"red_shi.png", 1);
	tmplImgs[RED_JIANG] = cvLoadImage(POKTMPLPATH"red_jiang.png", 1);
	tmplImgs[RED_PAO] = cvLoadImage(POKTMPLPATH"red_pao.png", 1);
	int width = 25 * xratio;
	int height = 25 * yratio;
	for(int i = 0; i < 14; i++)
	{
		IplImage * tmplImg = tmplImgs[i];
		IplImage * reszImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
		cvResize(tmplImg, reszImg);
		cvReleaseImage(&tmplImg);
		tmplImgs[i] = reszImg;

	}
}

bool isChess(IplImage * image, int x0, int y0, int type, double thresh)
{
	if(type < 0 || type > 13) return false;
	double diffval = 0.0;
	IplImage * tmplImg = tmplImgs[type];
	assert(tmplImg);
	int width = tmplImg->width;
	int height = tmplImg->height;
	int offsets[] = {0, 1, -1, 2, -2, 3, -3};
	for(int jj = 0; jj < 3; jj++)
	{
		int dy = offsets[jj];
		for(int ii = 0; ii < 3; ii++)
		{
			int dx = offsets[ii];
			double sumdiff = 0.0;
			for(int j = 0; j < height; j++)
			{
				for(int i = 0; i < width; i++)
				{
					int R1 = CV_IMAGE_ELEM(image, unsigned char, (y0 + dy) + j, ((x0+dx) + i) * 3 + 2);
					int G1 = CV_IMAGE_ELEM(image, unsigned char, (y0 + dy) + j, ((x0+dx) + i) * 3 + 1);
					int B1 = CV_IMAGE_ELEM(image, unsigned char, (y0 + dy) + j, ((x0+dx) + i) * 3 + 0);
					int R2 = CV_IMAGE_ELEM(tmplImg, unsigned char, j, i * 3 + 2);
					int G2 = CV_IMAGE_ELEM(tmplImg, unsigned char, j, i * 3 + 1);
					int B2 = CV_IMAGE_ELEM(tmplImg, unsigned char, j, i * 3 + 0);
					sumdiff += ABS(R1-R2) + ABS(G1-G2) + ABS(B1-B2);
				}
			}
			double avgdiff = sumdiff/(width * height);
			if(avgdiff < thresh) return true;
		}
	}
	return false;
}

int whichBlackChess(IplImage * image, int x0, int y0)
{
	double threshs[] = {50, 70, 90, 110, 130, 150, 200};
	for(int t = 0; t < 4; t++)
	{
		double thresh = threshs[t];
		for(int i = 0; i < 7; i++)
		{
			if(isChess(image, x0, y0, i, thresh)) 
				return i;
		}
	}
	return -1;
}
int whichRedChess(IplImage * image, int x0, int y0)
{
	double threshs[] = {50, 70, 90, 110, 130, 150, 200};
	for(int t = 0; t < 4; t++)
	{
		double thresh = threshs[t];
		for(int i = 7; i < 14; i++)
		{
			if(isChess(image, x0, y0, i, thresh)) 
				return i;
		}
	}
	return -1;
}

int main(int argc, char ** argv)
{
	if(argc == 1) { cout<<"Usage: "<<argv[0]<<" <screenimage> [-v]"<<endl; return 0;}
	infile = argv[1];
	bool verbose = (argc >= 3 && string(argv[2]) == "-v");
	IplImage * image = cvLoadImage(argv[1], 1);
	IplImage * drawImage = cvLoadImage(argv[1], 1);
	int width = image->width;
	int height = image->height; 
	int midx = (width+1)/2.0;
	int midy = (height+1)/2.0;

	int left = 0;
	for(int x = 0; x < width; x++)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			left = x;
			break;
		}
	}
	if(left == 0) return 0;
	int right = 0;

	for(int x = width - 1; x >= 0; x--)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, midy, 3*x + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			right = x;
			break;
		}
	}
	if(right == 0) return 0;
	
	int top = 0;
	for(int y = 0; y < height; y++)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			top = y;
			break;
		}
	}
	if(top == 0) return 0;

	int bot = 0;
	for(int y = height-1; y >= 0; y--)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*left + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			bot = y;
			break;
		}
	}
	if(bot == 0) return 0;

	int ch_x0 = 0;
	for(int x = left; x <= right; x++)
	{
		int count = 0;
		for(int y = top; y <= bot; y++)
		{
			int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 2);
			int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 1);
			int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 0);
			double dist = ABS(R - 146) + ABS(G - 103) + ABS(B - 58);
			if(dist < 20)
			{
				count++;
				if(count > 50)
				{
					ch_x0 = x;
					break;
				}
			}
		}
		if(ch_x0 > 0) break;
	}
	if(ch_x0 == 0) return 0;

	int ch_y0 = 0;
	for(int y = top; y <= bot; y++)
	{
		int count = 0;
		for(int x = left; x <= right; x++)
		{
			int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 2);
			int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 1);
			int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 0);
			double dist = ABS(R - 146) + ABS(G - 103) + ABS(B - 58);
			if(dist < 20)
			{
				count++;
				if(count > 50)
				{
					ch_y0 = y;
					break;
				}
			}
		}
		if(ch_y0 > 0) break;
	}
	if(ch_y0 == 0) return 0;

	int ch_xn = 0;
	for(int x = right; x >= left; x--)
	{
		int count = 0;
		for(int y = ch_y0; y <= bot; y++)
		{
			int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 2);
			int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 1);
			int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 0);
			double dist = ABS(R - 146) + ABS(G - 103) + ABS(B - 58);
			if(dist < 10)
			{
				count++;
				if(count > 50)
				{
					ch_xn = x;
					break;
				}
			}
		}
		if(ch_xn > 0) break;
	}
	if(ch_xn == 0) return 0;

	int ch_yn = 0;
	for(int y = bot; y >= top; y--)
	{
		int count = 0;
		for(int x = ch_x0; x <= right; x++)
		{
			int R = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 2);
			int G = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 1);
			int B = CV_IMAGE_ELEM(image, unsigned char, y, 3*x + 0);
			double dist = ABS(R - 146) + ABS(G - 103) + ABS(B - 58);
			if(dist < 10)
			{
				count++;
				if(count > 50)
				{
					ch_yn = y;
					break;
				}
			}
		}
		if(ch_yn > 0) break;
	}
	if(ch_yn == 0) return 0;

	cvRectangle(drawImage, cvPoint(ch_x0, ch_y0), cvPoint(ch_xn, ch_yn), CV_RGB(0, 0, 255), 1 );
	double xstep = (ch_xn - ch_x0)/8.0;
	double ystep = (ch_yn - ch_y0)/9.0;
	double xratio = xstep/63;
	double yratio = ystep/57;

	Init(xratio, yratio);
	char matrix[10][9];
	vector<int> movPoses;
	for(int j = 0; j < 10; j++)
	{
		for(int i = 0; i < 9; i++)
		{
			int x0 = ch_x0 + xstep * i;
			int y0 = ch_y0 + ystep * j;
			bool ismove = false;

			{
				int x1 = x0 - 26*xratio;
				int y1 = y0 - 20*yratio;
				int R = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 2);
				int G = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 1);
				int B = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 0);
				int dist = ABS(R - 172) + ABS(G - 135) + ABS(B - 86);
				if(dist < 20)
				{
					ismove = true;
					movPoses.push_back(j*9 + i);
					//cvRectangle(drawImage, cvPoint(x0-31*xratio, y0-28*yratio), cvPoint(x0 + 30*xratio, y0 + 27*yratio), CV_RGB(0, 0, 255), 1 );
					cvRectangle(drawImage, cvPoint(x1, y1), cvPoint(x0 + 30*xratio, y0 + 27*yratio), CV_RGB(0, 0, 255), 1 );
				}
			}

			{
				int x1 = x0 - 14*xratio;
				int y1 = y0 - 8*yratio;
				int R = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 2);
				int G = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 1);
				int B = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 0);
				//int dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
				//if(dist < 5) continue;

				int distred = ABS(R - 243) + ABS(G - 27) + ABS(B - 31);
				int distblack = ABS(R - 69) + ABS(G - 50) + ABS(B - 36);
				if(distred < 5)
				{
					cvRectangle(drawImage, cvPoint(x0-25*xratio, y0-26*yratio), cvPoint(x0 + 25*xratio, y0 + 26*yratio), CV_RGB(255, 0, 0), 1 ); 
					if(0)
					{
						IplImage * tmpImg = cropImage(image, x0 - 12*xratio, y0 - 12*yratio, 25*xratio, 25*yratio);
						ostringstream oss;
						oss << infile <<".out"<<j*9+i<<".png";
						cvSaveImage(oss.str().c_str(), tmpImg);
						cvReleaseImage(&tmpImg);
					}
					int type = whichRedChess(image, x0 - 12*xratio, y0 - 12*yratio);
					matrix[j][i] = type;
					if(type >= 7 && type <= 13) 
					{
						if(verbose)
						{
							if(ismove) cout<<RT_BKG(chessnames[type]);
							else cout<<RT(chessnames[type]);
						}
					}
					continue;
				}
				else if(distblack < 5)
				{
					cvRectangle(drawImage, cvPoint(x0-25*xratio, y0-26*yratio), cvPoint(x0 + 25*xratio, y0 + 26*yratio), CV_RGB(0, 0, 0), 1 ); 
					if(0)
					{
						IplImage * tmpImg = cropImage(image, x0 - 12*xratio, y0 - 12*yratio, 25*xratio, 25*yratio);
						ostringstream oss;
						oss << infile <<".out"<<j*9+i<<".png";
						cvSaveImage(oss.str().c_str(), tmpImg);
						cvReleaseImage(&tmpImg);
					}
					int type = whichBlackChess(image, x0 - 12*xratio, y0 - 12*yratio);
					matrix[j][i] = type;
					if(type >= 0 && type <= 6) 
					{
						if(verbose)
						{
							if(ismove) cout<<BT_BKG(chessnames[type]);
							else cout<<BT(chessnames[type]);
						}
					}
					continue;
				}
				else 
				{
					matrix[j][i] = -1;
					if(verbose)
					{
						if(ismove) cout<<BLANK_BKG;
						else cout<<BLANK;
					}
				}
			}
		}
		if(verbose) cout<<endl;
	}
	string fenstr = matrix2fenstr(matrix);
	string mycolor = "";
	string whoNext = "";
	for(int j = 7; j <= 9; j++)
	{
		for(int i = 0; i < 9; i++)
		{
			if(matrix[j][i] == BLACK_JIANG)
			{
				mycolor = "b";
				break;
			}
			else if(matrix[j][i] == RED_JIANG)
			{
				mycolor = "w";
				break;
			}
		}
		if(mycolor != "") break;
	}
	assert(movPoses.size() == 2 || movPoses.size() == 0);
	if(movPoses.size() == 2)
	{
		int pos1 = movPoses[0];
		int i1 = pos1 % 9;
		int j1 = pos1 / 9;
		int pos2 = movPoses[1];
		int i2 = pos2 % 9;
		int j2 = pos2 / 9;
		if(matrix[j1][i1] == -1 && matrix[j2][i2] != -1 && matrix[j2][i2] <= 6) whoNext = "w";
		else if(matrix[j1][i1] == -1 && matrix[j2][i2] != -1 && matrix[j2][i2] > 6) whoNext = "b";
		else if(matrix[j1][i1] != -1 && matrix[j2][i2] == -1 && matrix[j1][i1] <= 6) whoNext = "w";
		else if(matrix[j1][i1] != -1 && matrix[j2][i2] == -1 && matrix[j1][i1] > 6) whoNext = "b";
		else cerr<<"Invalid moves"<<endl;
	}
	else
	{
		if(fenstr == "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR" || fenstr == "RNBAKABNR/9/1C5C1/P1P1P1P1P/9/9/p1p1p1p1p/1c5c1/9/rnbakabnr") whoNext = "w";
	}
	cout<<fenstr<<" "<<mycolor<<" "<<whoNext<<" "<<ch_x0<<" "<<ch_y0<<" "<<xstep<<" "<<ystep<<endl;

	cvSaveImage((infile + ".out.png").c_str(), drawImage);
	cvReleaseImage(&image);
	cvReleaseImage(&drawImage);
	return 0;
}
