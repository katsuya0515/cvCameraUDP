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


 /* soclkaddr_in �\���̂̃Z�b�g */

 destSocket = socket(AF_INET, SOCK_DGRAM, 0);

 destSockAddr.sin_addr.S_un.S_addr = inet_addr(destination);
 destSockAddr.sin_port = htons(port);
 destSockAddr.sin_family = AF_INET;


 CvCapture *capture = 0;
  IplImage *frame = 0;
  IplImage *grayimg;

  double w = 320, h = 240;
  int c;

  // (1)�R�}���h�����ɂ���Ďw�肳�ꂽ�ԍ��̃J�����ɑ΂���L���v�`���\���̂��쐬����
  if (argc == 1 || (argc == 2 && strlen (argv[1]) == 1 && isdigit (argv[1][0])))
    capture = cvCreateCameraCapture (argc == 2 ? argv[1][0] - '0' : 0);

  /* ���̐ݒ�́C���p����J�����Ɉˑ����� */
  // (2)�L���v�`���T�C�Y��ݒ肷��D
  cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, w);
  cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, h);


  cvNamedWindow ("Capture", CV_WINDOW_AUTOSIZE);
  frame = cvQueryFrame (capture);
//  grayimg=cvCreateImage( cvSize(w,h),IPL_DEPTH_8U,1);
 // cvCvtColor(frame, grayimg, CV_BGR2GRAY);
   //RGB�C���[�W
    static const int sendSize = 61440;    //�P�x�ɑ��M����摜�̃o�b�t�@�T�C�Y:60KB(61440byte)
    static char buff[sendSize + 2];            //���M����o�b�t�@(�w�b�_�� + �摜�f�[�^)
	static const int divImageNum = (frame->imageSize / sendSize);    //RGB�摜�̕�����

  // (3)�J��������摜���L���v�`������
  while (1) {
    frame = cvQueryFrame (capture);
//	 cvCvtColor(frame, grayimg, CV_BGR2GRAY);
	//cvCopy(frame,jpgimage);
	
	  for(int i = 0;i <= divImageNum;i++){
        //�w�b�_���̓���
        buff[0] = 'R';    //RGB�摜���Ƃ������ʋL��
        buff[1] = i;    //�������ꂽ���Ԗڂ�
        //�摜�f�[�^�̓���
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
