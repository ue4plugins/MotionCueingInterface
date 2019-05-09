// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine.h"
#include "MotionData.h"
#include "MyMotionHost.h"
#include "MotionProbe.generated.h"


UENUM(BlueprintType)
enum class EVehicleType : uint8
{
	Boat,
	Car,
	Hellicopter,
	Bike,
	Plane
};

UENUM(BlueprintType)
enum class EInterpolationType : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInEaseOut
};

UENUM(BlueprintType)
enum class EVibrationType : uint8
{
	Speed,
	Acceleration,
	Surge,
	Heave,
	Sway,
	Roll,
	Pitch,
	Yaw
};

UCLASS( ClassGroup=(MotionCueing), meta=(BlueprintSpawnableComponent) )
class UMotionProbe : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMotionProbe();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type) override;
	void DebugPrint(float Deltatime);

	// Low frequency counters
	float NextTickTime;
	float NextTickDT;

	// Motion data per frame
	FMotionData CurrentHostData;
	FMotionData PreviousHostData;

	// Average Motion data over time
	TArray<FMotionData> Buffer;
	FMotionData CurrentAverageHostData;
	FMotionData PreviousAverageHostData;

	// Object used to get data from
	UMeshComponent* BaseObject;

	// High frequency timer and counters
	int TickControllerCount;
	int TickControllerTotal;
	int TickControllerCounter;
	int TickControllerAverageCount;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	// Host Interface
	MotionHost* HostInterface = new MyMotionHost();

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Properties of the component
	UPROPERTY(EditAnywhere)
		EVehicleType VehicleType;

	UPROPERTY(EditAnywhere)
		EInterpolationType InterpolationType;

	UPROPERTY(EditAnywhere)
		EVibrationType VibrationInput;

	UPROPERTY(EditAnywhere)
		int OutputFrequency;

	UPROPERTY(EditAnywhere)
		FString ControllerIP;

	UPROPERTY(EditAnywhere)
		int ControllerPort;
	
	UPROPERTY(EditAnywhere)
		int HostReceivePort;

	UPROPERTY(EditAnywhere)
		bool UseDebugMode;

	// Blueprint Function to set the base object
	UFUNCTION(BlueprintCallable, Category = "MotionCueing")
		void SetBaseObject(UMeshComponent* Input);

	// High frequency function that sends data to controller
	UFUNCTION()
		void TickController();

};
