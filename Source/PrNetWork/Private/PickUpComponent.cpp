// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpComponent.h"

#include "GameFramework/Character.h"

UPickUpComponent::UPickUpComponent()
{
	OnComponentBeginOverlap.AddDynamic(this, &UPickUpComponent::OnBeginOverlap);
}

void UPickUpComponent::AttachActorToCharacter(ACharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if(Character != nullptr)
	{
		Character->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");

		OnComponentBeginOverlap.RemoveDynamic(this, &UPickUpComponent::OnBeginOverlap);
	}
}

void UPickUpComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACharacter* Target = Cast<ACharacter>(OtherActor))
	{
		AttachActorToCharacter(Target);
	}
}