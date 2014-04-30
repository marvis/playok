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

string chessnames[7] = {"卒", "车", "马", "象", "士", "将", "炮"};
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

void Init()
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
}

bool isChess(IplImage * image, int x0, int y0, int type, double thresh)
{
	if(type < 0 || type > 13) return false;
	double diffval = 0.0;
	IplImage * tmplImg = tmplImgs[type];
	assert(tmplImg);
	int width = tmplImg->width;
	int height = tmplImg->height;
	for(int dy = -1; dy <= 1; dy++)
	{
		for(int dx = -1; dx <= 1; dx++)
		{
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

int whichBlackChess(IplImage * image, int x0, int y0, double thresh = 20.0)
{
	for(int i = 0; i < 7; i++)
	{
		if(isChess(image, x0, y0, i, thresh)) 
			return i;
	}
	return -1;
}
int whichRedChess(IplImage * image, int x0, int y0, double thresh = 20.0)
{
	for(int i = 7; i < 14; i++)
	{
		if(isChess(image, x0, y0, i, thresh)) 
			return i;
	}
	return -1;
}

int main(int argc, char ** argv)
{
	if(argc == 1) { cout<<"Usage: "<<argv[0]<<" <screenimage> [-v]"<<endl; return 0;}
	Init();
	infile = argv[1];
	bool verbose = (argc >= 3 && string(argv[2]) == "-v");
	IplImage * image = cvLoadImage(argv[1], 1);
	IplImage * drawImage = cvLoadImage(argv[1], 1);
	int width = image->width;
	int height = image->height; 

	int left = 0;
	for(int x = 0; x < width; x++)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			left = x;
			break;
		}
	}
	int right = 0;

	for(int x = width - 1; x >= 0; x--)
	{
		int R = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 2);
		int G = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 1);
		int B = CV_IMAGE_ELEM(image, unsigned char, 102, 3*x + 0);
		double dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
		if(dist < 5)
		{
			right = x;
			break;
		}
	}

	left += 76;
	right -= 78;
	int top = 144, bot = 656;
	double xstep = 63, ystep = 56.7;

	//cvRectangle(drawImage, cvPoint(left, top), cvPoint(right, bot), CV_RGB(0, 0, 255), 1 );
	char matrix[10][9];
	for(int j = 0; j < 10; j++)
	{
		for(int i = 0; i < 9; i++)
		{
			int x0 = left + xstep * i;
			int y0 = top + ystep * j;
			bool ismove = false;

			{
				int x1 = x0 - 27;
				int y1 = y0 - 14;
				int R = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 2);
				int G = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 1);
				int B = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 0);
				int dist = ABS(R - 172) + ABS(G - 135) + ABS(B - 86);
				if(dist < 10)
				{
					ismove = true;
					cvRectangle(drawImage, cvPoint(x0-31, y0-28), cvPoint(x0 + 30, y0 + 27), CV_RGB(0, 0, 255), 1 );
				}
			}

			{
				int x1 = x0 - 14;
				int y1 = y0 - 8;
				int R = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 2);
				int G = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 1);
				int B = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 0);
				//int dist = ABS(R - 227) + ABS(G - 175) + ABS(B - 108);
				//if(dist < 5) continue;

				int distred = ABS(R - 243) + ABS(G - 27) + ABS(B - 31);
				int distblack = ABS(R - 69) + ABS(G - 50) + ABS(B - 36);
				if(distred < 5)
				{
					cvRectangle(drawImage, cvPoint(x0-25, y0-26), cvPoint(x0 + 25, y0 + 26), CV_RGB(255, 0, 0), 1 ); 
					if(0)
					{
						IplImage * tmpImg = cropImage(image, x0 - 12, y0 - 12, 25, 25);
						ostringstream oss;
						oss << infile <<".out"<<j*9+i<<".png";
						cvSaveImage(oss.str().c_str(), tmpImg);
						cvReleaseImage(&tmpImg);
					}
					int type = whichRedChess(image, x0 - 12, y0 - 12);
					matrix[j][i] = type;
					if(type >= 7 && type <= 13) 
					{
						if(verbose)
						{
							if(ismove) cout<<RT_BKG(chessnames[type-7]);
							else cout<<RT(chessnames[type-7]);
						}
					}
					continue;
				}
				else if(distblack < 5)
				{
					cvRectangle(drawImage, cvPoint(x0-25, y0-26), cvPoint(x0 + 25, y0 + 26), CV_RGB(0, 0, 0), 1 ); 
					if(0)
					{
						IplImage * tmpImg = cropImage(image, x0 - 12, y0 - 12, 25, 25);
						ostringstream oss;
						oss << infile <<".out"<<j*9+i<<".png";
						cvSaveImage(oss.str().c_str(), tmpImg);
						cvReleaseImage(&tmpImg);
					}
					int type = whichBlackChess(image, x0 - 12, y0 - 12);
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
	cout<<matrix2fenstr(matrix)<<endl;
	for(int j = 0; j < 10; j++)
	{
		for(int i = 0; i < 9; i++)
		{
			int x0 = left + xstep * i;
			int y0 = top + ystep * j;
			int x1 = x0 - 27;
			int y1 = y0 - 14;
			int R = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 2);
			int G = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 1);
			int B = CV_IMAGE_ELEM(image, unsigned char, y1, 3*x1 + 0);
			int dist = ABS(R - 172) + ABS(G - 135) + ABS(B - 86);
			if(dist < 10)cvRectangle(drawImage, cvPoint(x0-31, y0-28), cvPoint(x0 + 30, y0 + 27), CV_RGB(0, 0, 255), 1 );
		}
	}

	cvSaveImage((infile + ".out.png").c_str(), drawImage);
	cvReleaseImage(&image);
	cvReleaseImage(&drawImage);
	return 0;
}
