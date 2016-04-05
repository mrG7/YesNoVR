// Fill out your copyright notice in the Description page of Project Settings.

#include "YesNoVRPrivatePCH.h"
#include "YesNoComponent.h"
#include "IHeadMountedDisplay.h"


// Calculates angle (in radians) between two vectors
float UYesNoComponent::Angle(const FVector& A, const FVector& B)
{
	return FMath::Acos(FVector::DotProduct(A, B) / A.Size() / B.Size());
}


void UYesNoComponent::BroadcastGestureIfCriteriaSatisfied(FGesture& Gesture, FCriteria Criteria)
{
	if (!Gesture.bIsTriggered && Gesture.NumberOfSignChanges > Criteria.MinimumRequiredSignChanges)
	{
		OnGestureDelegate.Broadcast(Gesture.Type);
		Gesture.bIsTriggered = true;
	}
}


// Sets default values for this component's properties
UYesNoComponent::UYesNoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UYesNoComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UYesNoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GEngine && GEngine->HMDDevice.IsValid()) {
		FVector HMDPosition;
		GEngine->HMDDevice->GetCurrentOrientationAndPosition(HMDOrientation.Current, HMDPosition);

		FVector PreviousForwardVector = HMDOrientation.Previous.RotateVector(FVector::ForwardVector);
		float PitchDelta = Angle(PreviousForwardVector, HMDOrientation.Current.RotateVector(FVector::UpVector)) - PI / 2;
		float YawDelta = Angle(PreviousForwardVector, HMDOrientation.Current.RotateVector(FVector::RightVector)) - PI / 2;

		UpdateGestureData(Yes, PitchDelta, YawDelta);
		UpdateGestureData(No, YawDelta, PitchDelta);

		BroadcastGestureIfCriteriaSatisfied(Yes, Criteria);
		BroadcastGestureIfCriteriaSatisfied(No, Criteria);

		HMDOrientation.Previous = HMDOrientation.Current;
	}
}

void UYesNoComponent::UpdateGestureData(FGesture& Gesture, float Angle, float TangentAngle)
{
	if (FMath::Abs(Angle) > Criteria.Threshold && FMath::Abs(TangentAngle / Angle) < Criteria.Ratio)
	{
		double CurrentTime = FPlatformTime::Seconds();
		// Outside of time window, thus reset gesture data
		if (CurrentTime - Gesture.Timestamp > Criteria.TimeWindow)
		{
			Gesture.Reset();
		}
		// Sign changes only count within time window
		else if (Gesture.Angle * Angle < 0)
		{
			Gesture.NumberOfSignChanges++;
		}
		Gesture.Timestamp = CurrentTime;
		Gesture.Angle = Angle;
		Gesture.TangentAngle = TangentAngle;
	}
}
