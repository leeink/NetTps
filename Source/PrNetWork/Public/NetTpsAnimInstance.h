// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetTpsAnimInstance.generated.h"

/**
 * 
 */

class UCharacterMovementComponent;
class APrNetWorkCharacter;

UCLASS()
class PRNETWORK_API UNetTpsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	APrNetWorkCharacter* Owner;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipState", meta = (AllowPrivateAccess = "true"))
	int EquipIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipState", meta = (AllowPrivateAccess = "true"))
	bool bHasPistol = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;


public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayFireMontage();
	void PlayReloadMontage();

	UFUNCTION()
	void AnimNotify_ReloadEnd(UAnimNotify* Notify);
};
