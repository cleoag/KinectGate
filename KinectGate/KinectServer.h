#include "StdAfx.h"

#pragma once

class KinectServer
{
public:
	int init();
	int initServer();
	bool waitForConnection();
	void closeConnection();
	bool sendData(int mixedData[], int dataSize);
};

