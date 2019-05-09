// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MotionProbe.h"

// IMPLEMENTATION NOTE: fmaheux
// The controller needs to get data at high frequency (100-1000Hz)
// Unfortunately, the usual Unreal project will run at a lower frequency, usually between 30-60 Hz
// So we must collect the necessary data from the BaseObject at low frequency (30 Hz).
// Then we interpolate the data and send it at high frequency to the controller. (100-1000 Hz)

// The low frequency function is TickComponent()
// The high frequency function is TickController()
// We interpolate between PreviousAverageHostData and CurrentAverageHostData 
// This solution adds a lag equal to the low frequency interval (33ms -> 30 Hz) * TickControllerAverageCount
// If lag is an issue, then we could lerp between CurrentData and a "PredictionData"

// Sets default values for this component's properties
UMotionProbe::UMotionProbe()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;

	NextTickTime = 0.033f;	// FORCED 30Hz
	NextTickDT = 0.0f;
	TickControllerCount = 0;
	TickControllerAverageCount = 10;

	OutputFrequency = 100;
	TickControllerTotal = 0;
	ControllerIP = "127.0.0.1";
	ControllerPort = 3000;
	HostReceivePort = 3001;

	UseDebugMode = true;
	VibrationInput = EVibrationType::Acceleration;
	VehicleType = EVehicleType::Car;
	InterpolationType = EInterpolationType::Linear;

	TickControllerCounter = 0;
	Buffer.Reset();
}

// Called when the game starts
void UMotionProbe::BeginPlay()
{
	Super::BeginPlay();

	// Clamp params
	OutputFrequency = FMath::Clamp(OutputFrequency, 1, 1000);

	// Reset
	TickControllerTotal = 0;
	TickControllerCounter = 0;
	TickControllerCount = 0;
	NextTickDT = 0.0f;
	Buffer.Reset();
	CurrentHostData.Reset();
	PreviousHostData.Reset();
	CurrentAverageHostData.Reset();
	PreviousAverageHostData.Reset();

	// Set low frequency timer for TickComponent()
	PrimaryComponentTick.TickInterval = NextTickTime;

	// Set high frequency timer for TickController()
	TimerDel.BindUFunction(this, FName("TickController"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0f/OutputFrequency, true);

	// Init Host interface 
	HostInterface->Initialize();

	// Connect to controller
	HostInterface->ConnectToController(ControllerIP, ControllerPort, HostReceivePort);
}

// Called when the game ends
void UMotionProbe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	HostInterface->Cleanup();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UMotionProbe::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	NextTickDT += DeltaTime;
	if (NextTickDT > NextTickTime)
	{
		// How many times we tick the controller in that frame
		TickControllerCounter += FMath::FloorToInt(NextTickDT * OutputFrequency);

		// Save previous data since we interpolate between "previous" and "current"
		PreviousHostData = CurrentHostData;

		// Get current data
		CurrentHostData.VehiclePosition = BaseObject->GetComponentLocation();
		CurrentHostData.VehicleRotation = BaseObject->GetComponentRotation();

		// Create vehicle reference frame
		FTransform TM = FTransform();
		TM.SetRotation(CurrentHostData.VehicleRotation.Quaternion());

		// Calculate forward vector in vehicle space
		FVector ForwardVector = CurrentHostData.VehiclePosition - PreviousHostData.VehiclePosition;
		ForwardVector = TM.Inverse().TransformVector(ForwardVector);

		// Vehicle Direction and Displacement
		ForwardVector.ToDirectionAndLength(CurrentHostData.VehicleDirection, CurrentHostData.VehicleDisplacement);

		// Vehicle Velocity
		CurrentHostData.VehicleVelocity = CurrentHostData.VehicleDisplacement / NextTickDT;

		// Vehicle Acceleration
		CurrentHostData.VehicleAcceleration = (CurrentHostData.VehicleVelocity - PreviousHostData.VehicleVelocity) / NextTickDT;

		// Kinematic velocity and acceleration per axis
		CurrentHostData.VelocityX = (ForwardVector.X) / NextTickDT;	// forward speed
		CurrentHostData.VelocityY = (ForwardVector.Y) / NextTickDT;	// side speed
		CurrentHostData.VelocityZ = (ForwardVector.Z) / NextTickDT;	// up speed
		CurrentHostData.AccelerationX = (CurrentHostData.VelocityX - PreviousHostData.VelocityX) / NextTickDT;	// forward acceleration
		CurrentHostData.AccelerationY = (CurrentHostData.VelocityY - PreviousHostData.VelocityY) / NextTickDT;	// side acceleration
		CurrentHostData.AccelerationZ = (CurrentHostData.VelocityZ - PreviousHostData.VelocityZ) / NextTickDT;	// up acceleration

		// Rotational motion
		CurrentHostData.PitchDisplacement = RotationalDeltaDeg(CurrentHostData.VehicleRotation.Pitch, PreviousHostData.VehicleRotation.Pitch);
		CurrentHostData.RollDisplacement = RotationalDeltaDeg(CurrentHostData.VehicleRotation.Roll, PreviousHostData.VehicleRotation.Roll);
		CurrentHostData.YawDisplacement = RotationalDeltaDeg(CurrentHostData.VehicleRotation.Yaw, PreviousHostData.VehicleRotation.Yaw);

		CurrentHostData.PitchVelocity = CurrentHostData.PitchDisplacement / NextTickDT;
		CurrentHostData.RollVelocity = CurrentHostData.RollDisplacement / NextTickDT;
		CurrentHostData.YawVelocity = CurrentHostData.YawDisplacement / NextTickDT;

		CurrentHostData.PitchAcceleration = (CurrentHostData.PitchVelocity - PreviousHostData.PitchVelocity) / NextTickDT;
		CurrentHostData.RollAcceleration = (CurrentHostData.RollVelocity - PreviousHostData.RollVelocity) / NextTickDT;
		CurrentHostData.YawAcceleration = (CurrentHostData.YawVelocity - PreviousHostData.YawVelocity) / NextTickDT;

		// reset
		NextTickDT = 0.0f;

		// accumulate data in buffer to average over time
		Buffer.Add(CurrentHostData);
	}

	// average over time
	if (Buffer.Num() >= TickControllerAverageCount)
	{
		TickControllerTotal = TickControllerCounter;
		TickControllerCounter = 0;
		TickControllerCount = 0;
		PreviousAverageHostData = CurrentAverageHostData;
		CurrentAverageHostData.Reset();
		for (FMotionData& Data : Buffer)
		{
			CurrentAverageHostData = CurrentAverageHostData + Data;
		}
		CurrentAverageHostData = CurrentAverageHostData / Buffer.Num();
		Buffer.Empty();
	}

	// Debug print
	if (UseDebugMode)
	{
		DebugPrint(DeltaTime * 2);
	}
}

// Will be called in a burst, not at equal intervals unfortunatly
// Will need to add a delay to avoid burst effect?
void UMotionProbe::TickController()
{
	// Interpolate data
	float T = float(TickControllerCount) / float(TickControllerTotal - 1);
	T = FMath::Clamp(T, 0.0f, 1.0f);
	FMotionData InterpolatedData = InterpolateMotionData(PreviousAverageHostData, CurrentAverageHostData, T);

	// receive data from controller
	HostInterface->ReceiveDataFromController();

	// go though the flow of state to control state machine
	HostInterface->SetStateFlow();

	// send data to controller
	HostInterface->SendDataToController(InterpolatedData);

	TickControllerCount++;
}

void UMotionProbe::DebugPrint(float DeltaTime)
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(15, DeltaTime, FColor::White, TEXT("Vehicle Roll accel (deg/s2): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.RollAcceleration, 2));
		GEngine->AddOnScreenDebugMessage(14, DeltaTime, FColor::White, TEXT("Vehicle Pitch accel (deg/s2): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.PitchAcceleration, 2));
		GEngine->AddOnScreenDebugMessage(13, DeltaTime, FColor::White, TEXT("Vehicle Yaw accel (deg/s2): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.YawAcceleration, 2));

		GEngine->AddOnScreenDebugMessage(12, DeltaTime, FColor::White, TEXT("Vehicle Roll velocity (deg/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.RollVelocity, 2));
		GEngine->AddOnScreenDebugMessage(11, DeltaTime, FColor::White, TEXT("Vehicle Pitch velocity (deg/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.PitchVelocity, 2));
		GEngine->AddOnScreenDebugMessage(10, DeltaTime, FColor::White, TEXT("Vehicle Yaw velocity (deg/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.YawVelocity, 2));

		GEngine->AddOnScreenDebugMessage(4, DeltaTime, FColor::White, TEXT("Current Roll (deg): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VehicleRotation.Roll, 2));
		GEngine->AddOnScreenDebugMessage(3, DeltaTime, FColor::White, TEXT("Current Pitch (deg): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VehicleRotation.Pitch, 2));
		GEngine->AddOnScreenDebugMessage(2, DeltaTime, FColor::White, TEXT("Current Yaw (deg): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VehicleRotation.Yaw, 2));

		GEngine->AddOnScreenDebugMessage(9, DeltaTime, FColor::White, TEXT("Vehicle VelocityZ (m/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VelocityZ*0.01f, 2));
		GEngine->AddOnScreenDebugMessage(8, DeltaTime, FColor::White, TEXT("Vehicle VelocityY (m/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VelocityY*0.01f, 2));
		GEngine->AddOnScreenDebugMessage(7, DeltaTime, FColor::White, TEXT("Vehicle VelocityX (m/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VelocityX*0.01f, 2));

		GEngine->AddOnScreenDebugMessage(6, DeltaTime, FColor::White, TEXT("Vehicle Acceleration (m/s2): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VehicleAcceleration*0.01f, 2));
		GEngine->AddOnScreenDebugMessage(5, DeltaTime, FColor::White, TEXT("Vehicle Velocity (m/s): ") + GetFloatAsStringWithPrecision(CurrentAverageHostData.VehicleVelocity*0.01f, 2));

		GEngine->AddOnScreenDebugMessage(1, DeltaTime, FColor::Red, TEXT("Current tick: ") + FString::FromInt(TickControllerCount));
		GEngine->AddOnScreenDebugMessage(0, DeltaTime, FColor::Red, TEXT("Total Tick: ") + FString::FromInt(TickControllerTotal));
	}
}

void UMotionProbe::SetBaseObject(UMeshComponent* Input)
{
	BaseObject = Input;
}
