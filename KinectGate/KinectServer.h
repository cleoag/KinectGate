#include "StdAfx.h"

#include <ws2tcpip.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#pragma comment (lib, "Ws2_32.lib")

#pragma once

typedef void (*callback)(void);

class KinectServer
{
public:
	int init(callback cb);
	int initServer();
	void waitForConnection();
	void closeConnection();
	int sendData(int mixedData, int dataSize);
};

