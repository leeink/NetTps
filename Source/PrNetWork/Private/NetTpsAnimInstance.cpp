// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTpsAnimInstance.h"

#include "PrNetWorkCharacter.h"

void UNetTpsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<APrNetWorkCharacter>(TryGetPawnOwner());
	CharacterMovementComponent = Owner ? Owner->GetCharacterMovement() : nullptr;
}

void UNetTpsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
	{
		return;
	}
	
	Speed = Owner->GetVelocity().Size();
	Direction = FVector::DotProduct(Owner->GetActorForwardVector(),Owner->GetActorRightVector());
	EquipIndex = Owner->GetEquipIndex();
}

void UNetTpsAnimInstance::PlayFireMontage()
{
	if (FireMontage != nullptr)
	{
		Montage_Play(FireMontage, 2.f);
	}
}