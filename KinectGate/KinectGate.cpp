#include "StdAfx.h"

#include <ws2tcpip.h>
#include "MSR_NuiApi.h"
#include "KinectServer.h"

#pragma comment (lib, "MSRKinectNUI.lib")

KinectServer server;

int mixedData[(NUI_SKELETON_POSITION_COUNT+1)*6*4+4];

HRESULT    hr;
POINT         m_Points[NUI_SKELETON_POSITION_COUNT];

int		trackedCount = 0;
int		m_LastSkeletonFoundTime;
bool	m_bScreenBlanked;
bool	isClientConnected = false;

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
//	hr = NuiCameraElevationSetAngle(NUI_CAMERA_ELEVATION_MAXIMUM);
//	hr = NuiCameraElevationSetAngle(0);

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

	initKinect();

	server = KinectServer();
	server.init();
	isClientConnected = server.waitForConnection();

	while (1){

		 while (isClientConnected) {
			GrabSkeleton();

			long dataSize = 4+(NUI_SKELETON_POSITION_COUNT+1)*4*6;
			isClientConnected = server.sendData(mixedData,dataSize);
		}



	isClientConnected = server.waitForConnection();
	}

	server.closeConnection();

	return 0;
}