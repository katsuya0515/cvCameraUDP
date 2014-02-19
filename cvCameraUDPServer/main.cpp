//サーバー側
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
  /* ポート番号、ソケット */
  int port =55555;
  int recvSocket;

  /* sockaddr_in 構造体 */
  struct sockaddr_in recvSockAddr;
  struct sockaddr_in clientSockAddr;

  int clientaddrlen = sizeof(clientSockAddr);
  /* 各種パラメータ */
  int status;
  int numrcv;
  unsigned long on = 1;

  /************************************************************/
  /* Windows 独自の設定 */
  WSADATA data;
  WSAStartup(MAKEWORD(2,0), &data);

  /* sockaddr_in 構造体のセット */
 // memset(&recvSockAddr, 0, sizeof(recvSockAddr));
  recvSockAddr.sin_port = htons(port);
  recvSockAddr.sin_family = AF_INET;
  recvSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* ソケット生成 */
  recvSocket = socket(AF_INET, SOCK_DGRAM, 0);
 
  /* バインド */
  status = bind(recvSocket, (struct sockaddr*) &recvSockAddr, (int) sizeof(recvSockAddr));

  // ウィンドウを作成する
  char windowName[] = "camera";
  cvNamedWindow( windowName, CV_WINDOW_AUTOSIZE );
  IplImage *frame =  cvCreateImage(cvSize(320,240), IPL_DEPTH_8U,1);

  static const int receiveSize = 61440;    //１度に受信する画像のバッファサイズ        60KB(61440byte)
  static char buff[receiveSize + 2];            //受信するバッファ(ヘッダ部 + 画像データ)
  static const int divImageNum = frame->imageSize / receiveSize;    //RGB画像の分割数

  int c;
  static int re;
  static char header;
  static char divNum;
  static int identificationNum;
  static char imageOrDepth;



  /* パケット受信 */
  while(1){
	 printf("Recieved");
	  
	  re = recvfrom(recvSocket, buff, receiveSize+2, 0,  (struct sockaddr*)&clientSockAddr, &clientaddrlen);

//	    if(re!=SOCKET_ERROR){//エラーで無ければ表示
        imageOrDepth = buff[0];    //RGB画像か深度画像か
        divNum = buff[1];        //分割番号
		
        if(imageOrDepth == 'R'){        //RGBイメージ
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

  /* Windows 独自の設定 */
  WSACleanup();
}