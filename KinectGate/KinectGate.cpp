#include "StdAfx.h"

//#include <ws2tcpip.h>

//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>
#include "MSR_NuiApi.h"
//#include <conio.h>

//#pragma comment (lib, "Ws2_32.lib")

#pragma comment (lib, "MSRKinectNUI.lib")

//#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "27015"


KinectServer server;

int mixedData[(NUI_SKELETON_POSITION_COUNT+1)*6*4+4];

HRESULT    hr;
POINT         m_Points[NUI_SKELETON_POSITION_COUNT];

int			trackedCount = 0;

int           m_LastSkeletonFoundTime;
bool          m_bScreenBlanked;


void GrabSkeleton()
{
	NUI_SKELETON_FRAME SkeletonFrame;

	hr = NuiSkeletonGetNextFrame( 100, &SkeletonFrame );
	if( FAILED( hr ) )
	{
		printf("error getting frame\n");
		return;
	}


	bool bFoundSkeleton = true;


	trackedCount = 0;

	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	{
		if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
		{
			bFoundSkeleton = false;
			trackedCount++;
		}
	}

	// no skeletons!
	//
	if( bFoundSkeleton )
	{
		printf("no skeletons!!!\n");

	} 

		// smooth out the skeleton data
		NuiTransformSmooth(&SkeletonFrame,NULL);

		// draw each skeleton color according to the slot within they are found.
		//

		int width = 640;
		int height = 480;

		float fx=0,fy=0;

		float* ptr = (float*)mixedData;

		*ptr = (float) trackedCount;
		++ptr;

		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
			{
				*ptr = (float) i;
				++ptr;

				for (int s = 0; s < NUI_SKELETON_POSITION_COUNT; s++)
				{
					NUI_SKELETON_DATA * pSkel = &SkeletonFrame.SkeletonData[i];
					NuiTransformSkeletonToDepthImageF( pSkel->SkeletonPositions[s], &fx, &fy );

					//int jointX = (int) ( fx * width + 0.5f );
					//int jointY = (int) ( fy * height + 0.5f );


					if (s == 11){
						printf("id = %d, x = %f, y = %f\n",i, fx,fy);
					}

					*ptr = fx;
					++ptr;

					*ptr = fy;
					++ptr;


				}
			}
		}
}


void initKinect(){
	hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
	if( FAILED( hr ) )
	{
		printf("error NuiInitialize...\n");
		return;
	} else {
		printf("NuiInitialize OK!\n");
	}
	hr = NuiCameraElevationSetAngle(NUI_CAMERA_ELEVATION_MAXIMUM);
	hr = NuiCameraElevationSetAngle(5);

	hr = NuiSkeletonTrackingEnable(NULL,0);
	if( FAILED( hr ) )
	{
		printf("error NuiSkeletonTrackingEnable\n");
		return;
	} else {
		printf("NuiSkeletonTrackingEnable ok!\n");
	}


}

int __cdecl main(int argc, char *argv[]) {

	/*
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET,
		ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	*/


	if(argc > 1)
	{
		printf_s( "\nCommand-line arguments:\n" );
		for(int  count = 0; count < argc; count++ )
			printf_s( "  argv[%d]   %s\n", count, argv[count] );

		//motorAngl = atoi(argv[1]);
	}


	// ----------------------------------------------------------
	initKinect();
	


	/*
	while(1)
	{
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}


		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if ( iResult != 0 ) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		printf("create socket");
		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}
		// Setup the TCP listening socket
		iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		// No longer need server socket
		closesocket(ListenSocket);

		do {
			GrabSkeleton();

			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			// if (iResult > 0) {
			//    printf("Bytes received: %d\n", iResult);

			if(iResult)
			{
				//				scaleDiv = recvbuf[0];
				//if(scaleDiv > 0x04) scaleDiv = 0x04;
			}
			// Echo the buffer back to the sender
			long dataSize = 4+(NUI_SKELETON_POSITION_COUNT+1)*4*6;//(xRes/scaleDiv*4)*(yRes/scaleDiv);

			iSendResult = send( ClientSocket, (char *) mixedData, dataSize, 0 );
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				//return 1;
				break;
			}

		} while (1);
	}
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	*/

	return 0;
}
