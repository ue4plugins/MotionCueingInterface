// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MotionHost.h"

#if WITH_THEAPI
// add API include here
// ...
// ...
// ...
// ...
#endif


class MyMotionHost : public MotionHost
{
private:

	virtual void MapData(FMotionData& Data) override
	{
		// Map data from Unreal to your API data structure
		// ...


	}

public:

	virtual void Initialize() override
	{
		// Initialize your API object here
		// ...

	}

	virtual void Cleanup() override
	{
		// Perform some cleanup if necessary
		// ...

	}

	virtual void ConnectToController(FString ControllerIP, int ControllerPort, int HostRcvPort) override
	{
		// Connect to your controller using arguments
		// ...

	}

	virtual void ReceiveDataFromController() override
	{
		// Read data from controller
		// ...

	}


	virtual void SetStateFlow() override
	{
		// If required, go through a state flow in a specific order before sending/receiving data to the controller
		// ...

	}

	virtual void SendDataToController(FMotionData& Data) override
	{
		// Map data from Unreal to Controller
		MapData(Data);


		// Send data
		// ...
	}
};
