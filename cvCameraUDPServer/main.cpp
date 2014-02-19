//�T�[�o�[��
#include <stdio.h>
#include<string.h>

#include <winsock2.h>
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

int
main() {
  /* �|�[�g�ԍ��A�\�P�b�g */
  int port =55555;
  int recvSocket;

  /* sockaddr_in �\���� */
  struct sockaddr_in recvSockAddr;
  struct sockaddr_in clientSockAddr;

  int clientaddrlen = sizeof(clientSockAddr);
  /* �e��p�����[�^ */
  int status;
  int numrcv;
  unsigned long on = 1;

  /************************************************************/
  /* Windows �Ǝ��̐ݒ� */
  WSADATA data;
  WSAStartup(MAKEWORD(2,0), &data);

  /* sockaddr_in �\���̂̃Z�b�g */
 // memset(&recvSockAddr, 0, sizeof(recvSockAddr));
  recvSockAddr.sin_port = htons(port);
  recvSockAddr.sin_family = AF_INET;
  recvSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* �\�P�b�g���� */
  recvSocket = socket(AF_INET, SOCK_DGRAM, 0);
 
  /* �o�C���h */
  status = bind(recvSocket, (struct sockaddr*) &recvSockAddr, (int) sizeof(recvSockAddr));

  // �E�B���h�E���쐬����
  char windowName[] = "camera";
  cvNamedWindow( windowName, CV_WINDOW_AUTOSIZE );
  IplImage *frame =  cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,1);

  static const int receiveSize = 61440;    //�P�x�Ɏ�M����摜�̃o�b�t�@�T�C�Y        60KB(61440byte)
  static char buff[receiveSize + 2];            //��M����o�b�t�@(�w�b�_�� + �摜�f�[�^)
  static const int divImageNum = frame->imageSize / receiveSize;    //RGB�摜�̕�����

  int c;
  static int re;
  static char header;
  static char divNum;
  static int identificationNum;
  static char imageOrDepth;



  /* �p�P�b�g��M */
  while(1){
	 printf("Recieved");
	  
	  re = recvfrom(recvSocket, buff, receiveSize+2, 0,  (struct sockaddr*)&clientSockAddr, &clientaddrlen);

//	    if(re!=SOCKET_ERROR){//�G���[�Ŗ�����Ε\��
        imageOrDepth = buff[0];    //RGB�摜���[�x�摜��
        divNum = buff[1];        //�����ԍ�
		
        if(imageOrDepth == 'R'){        //RGB�C���[�W
		//	printf("%d\n",divNum);
       memcpy(frame->imageData+(receiveSize * divNum),&buff[2],receiveSize);
		//}
				
		 cvShowImage(windowName,frame);
    }
//	  memcpy(frame->imageData,buff,sizeof(buff));

	
  
  c = cvWaitKey (2);
    if (c == '\x1b')
      break;
  }

  /* Windows �Ǝ��̐ݒ� */
  WSACleanup();
}