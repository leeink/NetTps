// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PrNetWorkCharacter.generated.h"

class UHealthBar;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UWeaponComponent;
class UHealthComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APrNetWorkCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = AnimInistance, meta = (AllowPrivateAccess = "true"))
	class UNetTpsAnimInstance* PlayerAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PickUpCollision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* PickUpCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealthWidget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = PickUpCollision, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HeathSystem;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Shot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Reload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Slot2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WeaponPickup;

	UPROPERTY()
	TArray<AActor*> OverlappingActors;

	UPROPERTY()
	class AWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WidgetClass, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMainWidget> MainWidgetClass;
	
	UPROPERTY()
	UMainWidget* MainWidget;

	UPROPERTY()
	UHealthBar* HealthBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	int EquipIndex = 0;
	bool bHasPistol = false;
	bool bIsReloading = false;

public:
	APrNetWorkCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void PistolSlot(const FInputActionValue& Value);
	void BaseSlot(const FInputActionValue& Value);
	void Interaction(const FInputActionValue& Value);
	void Shot(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	void PlayerDamage(float Amount);
	
	void InitMainWidget();
	void UpdateBulletUI();
	void PrintNetLog();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE int GetEquipIndex() const { return EquipIndex; }
	FORCEINLINE void SetIsReload(bool bIsReload) { bIsReloading = bIsReload; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE void SetIsDead(bool newState) { bIsDead = newState; }
	FORCEINLINE UMainWidget* GetMainWidget() const { return MainWidget; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HeathSystem; }

	UFUNCTION()
	void OnPickUpOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

