// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Generic data structure for motion system

struct FMotionData
{
	FMotionData()
	{
		Reset();
	}

	FMotionData& operator=(const FMotionData& A)
	{
		Counter = A.Counter;
		MotionStateCommand = A.MotionStateCommand;
		VehicleRotation = A.VehicleRotation;
		VehicleDirection = A.VehicleDirection;
		VehiclePosition = A.VehiclePosition;
		VehicleDisplacement = A.VehicleDisplacement;
		VehicleVelocity = A.VehicleVelocity;
		VehicleAcceleration = A.VehicleAcceleration;
		VelocityX = A.VelocityX;
		VelocityY = A.VelocityY;
		VelocityZ = A.VelocityZ;
		AccelerationX = A.AccelerationX;
		AccelerationY = A.AccelerationY;
		AccelerationZ = A.AccelerationZ;
		PitchDisplacement = A.PitchDisplacement;
		YawDisplacement = A.YawDisplacement;
		RollDisplacement = A.RollDisplacement;
		PitchVelocity = A.PitchVelocity;
		YawVelocity = A.YawVelocity;
		RollVelocity = A.RollVelocity;
		PitchAcceleration = A.PitchAcceleration;
		YawAcceleration = A.YawAcceleration;
		RollAcceleration = A.RollAcceleration;
		return *this;
	}

	FMotionData& operator+(const FMotionData& A)
	{
		// Not additive, assigned it
		Counter = A.Counter;
		MotionStateCommand = A.MotionStateCommand;
		VehicleRotation = A.VehicleRotation;

		// keep direction normalized
		VehicleDirection += A.VehicleDirection;
		VehicleDirection.Normalize();

		// additive
		VehiclePosition += A.VehiclePosition;
		VehicleDisplacement += A.VehicleDisplacement;
		VehicleVelocity += A.VehicleVelocity;
		VehicleAcceleration += A.VehicleAcceleration;
		VelocityX += A.VelocityX;
		VelocityY += A.VelocityY;
		VelocityZ += A.VelocityZ;
		AccelerationX += A.AccelerationX;
		AccelerationY += A.AccelerationY;
		AccelerationZ += A.AccelerationZ;
		PitchDisplacement += A.PitchDisplacement;
		YawDisplacement += A.YawDisplacement;
		RollDisplacement += A.RollDisplacement;
		PitchVelocity += A.PitchVelocity;
		YawVelocity += A.YawVelocity;
		RollVelocity += A.RollVelocity;
		PitchAcceleration += A.PitchAcceleration;
		YawAcceleration += A.YawAcceleration;
		RollAcceleration += A.RollAcceleration;
		return *this;
	}

	FMotionData& operator/(float num)
	{
		//Counter
		//MotionStateCommand
		//VehicleRotation
		//VehicleDirection
		VehiclePosition = VehiclePosition / num;
		VehicleDisplacement = VehicleDisplacement / num;
		VehicleVelocity = VehicleVelocity / num;
		VehicleAcceleration = VehicleAcceleration / num;
		VelocityX = VelocityX / num;
		VelocityY = VelocityY / num;
		VelocityZ = VelocityZ / num;
		AccelerationX = AccelerationX / num;
		AccelerationY = AccelerationY / num;
		AccelerationZ = AccelerationZ / num;
		PitchDisplacement = PitchDisplacement / num;
		YawDisplacement = YawDisplacement / num;
		RollDisplacement = RollDisplacement / num;
		PitchVelocity = PitchVelocity / num;
		YawVelocity = YawVelocity / num;
		RollVelocity = RollVelocity / num;
		PitchAcceleration = PitchAcceleration / num;
		YawAcceleration = YawAcceleration / num;
		RollAcceleration = RollAcceleration / num;
		return *this;
	}

	void SetValue(float Value)
	{
		VehicleRotation = FRotator(Value, Value, Value);
		VehicleDirection = FVector(Value, Value, Value);
		VehiclePosition = FVector(Value, Value, Value);
		Counter = Value;
		MotionStateCommand = Value;
		VehicleDisplacement = Value;
		VehicleVelocity = Value;
		VehicleAcceleration = Value;
		VelocityX = Value;
		VelocityY = Value;
		VelocityZ = Value;
		AccelerationX = Value;
		AccelerationY = Value;
		AccelerationZ = Value;
		PitchDisplacement = Value;
		YawDisplacement = Value;
		RollDisplacement = Value;
		PitchVelocity = Value;
		YawVelocity = Value;
		RollVelocity = Value;
		PitchAcceleration = Value;
		YawAcceleration = Value;
		RollAcceleration = Value;
	}

	void Reset()
	{
		SetValue(0.0f);
	}

	// tick counter
	int32 Counter;

	// A Integer that represent a state of motion, TBD
	int32 MotionStateCommand;

	// Vehicle (ROLL, PITCH, YAW) with respect to horizontal plane (deg)
	FRotator VehicleRotation;

	// Vehicle direction in vehicle space (X is forward, Y is side, Z is up)
	FVector VehicleDirection;

	// Kinematic motion
	FVector VehiclePosition;
	float VehicleDisplacement;
	float VehicleVelocity;
	float VehicleAcceleration;

	// Kinematic velocity per axis (cm/s)
	float VelocityX;
	float VelocityY;
	float VelocityZ;

	// Kinematic acceleraton per axis (cm/s2)
	float AccelerationX;
	float AccelerationY;
	float AccelerationZ;

	// Vehicle rotational displacement (deg)
	float PitchDisplacement;
	float YawDisplacement;
	float RollDisplacement;

	// Vehicle Rotational velocity (deg/s)
	float PitchVelocity;
	float YawVelocity;
	float RollVelocity;

	// Vehicle Rotational acceleration (deg/s2)
	float PitchAcceleration;
	float YawAcceleration;
	float RollAcceleration;
};

// linear interpolation
static inline float Lerp(float a, float b, float f)
{
	return (a * (1.0 - f)) + (b * f);
}

// Calculate rotational difference on a circle in deg [-180, 180]
static inline float RotationalDeltaDeg(float CurrentAngle, float PreviousAngle)
{
	float PITAU = 360 + 180;
	float Delta = FMath::Fmod((CurrentAngle - PreviousAngle + PITAU), 360) - 180;
	return Delta;
}

// add 2 angles and stay in range [-180, 180]
static inline float RotationalAddDeg(float Angle1, float Angle2)
{
	float PITAU = 360 + 180;
	float Added = FMath::Fmod((Angle1 + Angle2 + PITAU), 360) - 180;
	return Added;
}


/*
// Calculate rotational difference on a circle in rad (at most PI rad)
static inline float RotationalDifferenceRad(float CurrentAngle, float PreviousAngle)
{
	float TAU = 2 * PI;
	float PITAU = PI + TAU;
	float Delta = FMath::Fmod((CurrentAngle - PreviousAngle + PITAU), TAU) - PI;
	return Delta;
}
*/

// Interpolate from A to B
static inline FMotionData InterpolateMotionData(FMotionData& A, FMotionData& B, float T)
{
	FMotionData Out;

	// nlerp 
	Out.VehicleDirection.X = Lerp(A.VehicleDirection.X, B.VehicleDirection.X, T);
	Out.VehicleDirection.Y = Lerp(A.VehicleDirection.Y, B.VehicleDirection.Y, T);
	Out.VehicleDirection.Z = Lerp(A.VehicleDirection.Z, B.VehicleDirection.Z, T);
	Out.VehicleDirection.Normalize();

	Out.VehiclePosition.X = Lerp(A.VehiclePosition.X, B.VehiclePosition.X, T);
	Out.VehiclePosition.Y = Lerp(A.VehiclePosition.Y, B.VehiclePosition.Y, T);
	Out.VehiclePosition.Z = Lerp(A.VehiclePosition.Z, B.VehiclePosition.Z, T);

	float PitchDelta = RotationalDeltaDeg(A.VehicleRotation.Pitch, B.VehicleRotation.Pitch);
	Out.VehicleRotation.Pitch = RotationalAddDeg(A.VehicleRotation.Pitch, PitchDelta*T);

	float RollDelta = RotationalDeltaDeg(A.VehicleRotation.Roll, B.VehicleRotation.Roll);
	Out.VehicleRotation.Roll = RotationalAddDeg(A.VehicleRotation.Roll, RollDelta*T);

	float YawDelta = RotationalDeltaDeg(A.VehicleRotation.Yaw, B.VehicleRotation.Yaw);
	Out.VehicleRotation.Yaw = RotationalAddDeg(A.VehicleRotation.Yaw, YawDelta*T);

	Out.VelocityX = Lerp(A.VelocityX, B.VelocityX, T);
	Out.VelocityY = Lerp(A.VelocityY, B.VelocityY, T);
	Out.VelocityZ = Lerp(A.VelocityZ, B.VelocityZ, T);

	Out.AccelerationX = Lerp(A.AccelerationX, B.AccelerationX, T);
	Out.AccelerationY = Lerp(A.AccelerationY, B.AccelerationY, T);
	Out.AccelerationZ = Lerp(A.AccelerationZ, B.AccelerationZ, T);

	Out.PitchDisplacement = Lerp(A.PitchDisplacement, B.PitchDisplacement, T);
	Out.RollDisplacement = Lerp(A.RollDisplacement, B.RollDisplacement, T);
	Out.YawDisplacement = Lerp(A.YawDisplacement, B.YawDisplacement, T);

	Out.PitchVelocity = Lerp(A.PitchVelocity, B.PitchVelocity, T);
	Out.RollVelocity = Lerp(A.RollVelocity, B.RollVelocity, T);
	Out.YawVelocity = Lerp(A.YawVelocity, B.YawVelocity, T);

	Out.PitchAcceleration = Lerp(A.PitchAcceleration, B.PitchAcceleration, T);
	Out.RollAcceleration = Lerp(A.RollAcceleration, B.RollAcceleration, T);
	Out.YawAcceleration = Lerp(A.YawAcceleration, B.YawAcceleration, T);

	Out.VehicleDisplacement = Lerp(A.VehicleDisplacement, B.VehicleDisplacement, T);
	Out.VehicleVelocity = Lerp(A.VehicleVelocity, B.VehicleVelocity, T);
	Out.VehicleAcceleration = Lerp(A.VehicleAcceleration, B.VehicleAcceleration, T);

	return Out;
}

static inline FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
{
	//Round to integral if have something like 1.9999 within precision
	float Rounded = roundf(TheFloat);
	if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		TheFloat = Rounded;
	}
	FNumberFormattingOptions NumberFormat;
	NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
	NumberFormat.AlwaysSign = true;

	FString Out;
	Out = FText::AsNumber(TheFloat, &NumberFormat).ToString();
	return Out;
}
