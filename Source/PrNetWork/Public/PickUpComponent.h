// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PickUpComponent.generated.h"

/**
 * 
 */

class ACharacter;

UCLASS()
class PRNETWORK_API UPickUpComponent : public USphereComponent
{
	GENERATED_BODY()

	UPROPERTY()
	ACharacter* Character;

public:
	UPickUpComponent();

public:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AttachActorToCharacter(ACharacter* TargetCharacter);
};
