// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "YesNoComponent.generated.h"

UENUM(BlueprintType)
enum class EGestures : uint8 {
	Yes,
	No
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureDelegate, EGestures, Gesture);

USTRUCT()
struct FGesture {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	EGestures Type;
	UPROPERTY(VisibleAnywhere)
	double Timestamp;
	UPROPERTY(VisibleAnywhere)
	float Angle;
	UPROPERTY(VisibleAnywhere)
	float TangentAngle;
	UPROPERTY(VisibleAnywhere)
	int32 NumberOfSignChanges;
	UPROPERTY(VisibleAnywhere)
	bool bIsTriggered;

	void Reset() {
		NumberOfSignChanges = 0;
		bIsTriggered = false;
	};
};

USTRUCT()
struct FCriteria {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Threshold;
	UPROPERTY(EditAnywhere)
	float Ratio;
	UPROPERTY(EditAnywhere)
	int32 MinimumRequiredSignChanges;
	UPROPERTY(EditAnywhere)
	float TimeWindow;
};

USTRUCT()
struct FHMDOrientation {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	FQuat Current;
	FQuat Previous;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UYesNoComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	FGesture Yes = { EGestures::Yes, 0.0, 0.0, 0.0, 0, false };

	UPROPERTY(VisibleAnywhere)
	FGesture No = { EGestures::No, 0.0, 0.0, 0.0, 0, false };

	UPROPERTY(VisibleAnywhere)
	FHMDOrientation HMDOrientation;

	float Angle(const FVector& A, const FVector& B);

	void UpdateGestureData(FGesture& Gesture, float Angle, float TangentAngle);

	void BroadcastGestureIfCriteriaSatisfied(FGesture& Gesture, FCriteria Criteria);

public:
	UPROPERTY(EditAnywhere)
	FCriteria Criteria = { 0.01f, 0.1f, 2, 0.5f };

	UPROPERTY(BlueprintAssignable, Category = "YesNoVR")
	FGestureDelegate OnGestureDelegate;

	// Sets default values for this component's properties
	UYesNoComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};