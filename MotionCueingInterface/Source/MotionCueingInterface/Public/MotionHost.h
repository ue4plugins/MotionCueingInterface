// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MotionData.h"
#include "CoreMinimal.h"
#include "Engine.h"

// Generic base class to interface with external Motion System API
class MotionHost
{
private:

	// Remap data from Unreal to your API
	virtual void MapData(FMotionData& Data) = 0;

public:

	// Init API
	virtual void Initialize() = 0;

	// Clean up
	virtual void Cleanup() = 0;

	// Connect to the controller
	virtual void ConnectToController(FString ControllerIP, int ControllerPort, int HostRcvPort) = 0;

	// Send data to the controller
	virtual void SendDataToController(FMotionData& Data) = 0;

	// Receive data from the controller 
	virtual void ReceiveDataFromController() = 0;

	// Sequence of command to control state machine
	virtual void SetStateFlow() = 0;
};
