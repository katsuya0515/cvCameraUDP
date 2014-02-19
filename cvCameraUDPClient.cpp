#include <stdio.h>
#include <winSock2.h>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

#pragma comment( lib, "wsock32.lib" )
#pragma comment( lib, "ws2_32.lib")

int
main (int argc, char **argv){
 char destination[80]="127.0.0.1";
 unsigned short port=55555;
 SOCKET destSocket;

 struct sockaddr_in destSockAddr;

 WSADATA data;
 WSAStartup(MAKEWORD(2, 0), &data);


 /* soclkaddr_in 構造体のセット */

 destSocket = socket(AF_INET, SOCK_DGRAM, 0);

 destSockAddr.sin_addr.S_un.S_addr = inet_addr(destination);
 destSockAddr.sin_port = htons(port);
 destSockAddr.sin_family = AF_INET;


 CvCapture *capture = 0;
  IplImage *frame = 0;
  IplImage *grayimg;

  double w = 320, h = 240;
  int c;

  // (1)コマンド引数によって指定された番号のカメラに対するキャプチャ構造体を作成する
  if (argc == 1 || (argc == 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])))
    capture = cvCreateCameraCapture (argc == 2 ? argv[1][0] - '0' : 0);

  /* この設定は，利用するカメラに依存する */
  // (2)キャプチャサイズを設定する．
  cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, w);
  cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, h);


  cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);
  frame = cvQueryFrame (capture);
//  grayimg=cvCreateImage( cvSize(w,h),IPL_DEPTH_8U,1);
 // cvCvtColor(frame, grayimg, CV_BGR2GRAY);
   //RGBイメージ
    static const int sendSize = 61440;    //１度に送信する画像のバッファサイズ:60KB(61440byte)
    static char buff[sendSize + 2];            //送信するバッファ(ヘッダ部 + 画像データ)
	static const int divImageNum = (frame->imageSize / sendSize);    //RGB画像の分割数

  // (3)カメラから画像をキャプチャする
  while (1) {
    frame = cvQueryFrame (capture);
//	 cvCvtColor(frame, grayimg, CV_BGR2GRAY);
	//cvCopy(frame,jpgimage);
	
	  for(int i = 0;i <= divImageNum;i++){
        //ヘッダ部の入力
        buff[0] = 'R';    //RGB画像だという識別記号
        buff[1] = i;    //分割された何番目か
        //画像データの入力
		memcpy(&buff[2],frame->imageData+(sendSize * i),sendSize);
		printf("%d\n",frame->imageData);
		printf("%d\n",frame->imageData+(sendSize * i));
        int send_len=sendto(destSocket,buff,sendSize + 2,0,(struct sockaddr*)&destSockAddr, sizeof(destSockAddr));
		printf("%d : %d\n",i,send_len);
    }
	
	cvShowImage ("Capture",frame);
    c = cvWaitKey (2);
    if (c == '\x1b')
      break;
  }

  cvReleaseCapture (&capture);
  cvDestroyWindow ("Capture");
  closesocket(destSocket);
  WSACleanup();
 
  return 0;
}
