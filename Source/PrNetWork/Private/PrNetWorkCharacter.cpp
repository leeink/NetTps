// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrNetWorkCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthBar.h"
#include "HealthComponent.h"
#include "InputActionValue.h"
#include "MainWidget.h"
#include "NetTpsAnimInstance.h"
#include "Weapon.h"
#include "WeaponComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APrNetWorkCharacter

APrNetWorkCharacter::APrNetWorkCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset = FVector(0.f, 40.f, 60.f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PickUpCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollision"));
	PickUpCollision->SetupAttachment(RootComponent);
	PickUpCollision-> SetBoxExtent(FVector(50.f,50.f,50.f));

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidget -> SetupAttachment(GetMesh());

	HeathSystem = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthSystem"));

	PickUpCollision->OnComponentBeginOverlap.AddDynamic(this, &APrNetWorkCharacter::OnPickUpOverlap);
	PickUpCollision->OnComponentEndOverlap.AddDynamic(this, &APrNetWorkCharacter::OnEndOverlap);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APrNetWorkCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	PlayerAnimInstance = CastChecked<UNetTpsAnimInstance>(GetMesh() -> GetAnimInstance());

	InitMainWidget();
	/*MainWidget = CastChecked<UMainWidget>(CreateWidget(GetWorld(), MainWidgetClass));
	MainWidget -> AddToViewport();*/

	HealthBar = Cast<UHealthBar>(HealthWidget -> GetWidget());
}


//////////////////////////////////////////////////////////////////////////
// Input

void APrNetWorkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrNetWorkCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrNetWorkCharacter::Look);

		EnhancedInputComponent->BindAction(Slot1, ETriggerEvent::Triggered, this, &APrNetWorkCharacter::PistolSlot);
		EnhancedInputComponent->BindAction(Slot2, ETriggerEvent::Triggered, this, &APrNetWorkCharacter::BaseSlot);
		EnhancedInputComponent->BindAction(WeaponPickup, ETriggerEvent::Started, this, &APrNetWorkCharacter::Interaction);
		EnhancedInputComponent->BindAction(IA_Shot, ETriggerEvent::Triggered, this, &APrNetWorkCharacter::Shot);
		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Started, this, &APrNetWorkCharacter::Reload);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APrNetWorkCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APrNetWorkCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APrNetWorkCharacter::PistolSlot(const FInputActionValue& Value)
{
	EquipIndex = 0;
	MainWidget -> SetActiveCrossHair(true);
}

void APrNetWorkCharacter::BaseSlot(const FInputActionValue& Value)
{
	EquipIndex = 1;
	MainWidget -> SetActiveCrossHair(false);
}

void APrNetWorkCharacter::Interaction(const FInputActionValue& Value)
{
	if(bHasPistol == false)
	{
		if(OverlappingActors.Num() == 0)
		{
			return;
		}
	
		float min = 9999999.f;
		AActor* closestWeapon = nullptr;
	
		for(auto* weapon : OverlappingActors)
		{
			if(weapon -> GetOwner() != nullptr)
			{
				continue;
			}
		
			if(weapon -> GetDistanceTo(this) < min)
			{
				min = weapon -> GetDistanceTo(this);
				closestWeapon = weapon;
			}
		}
		closestWeapon -> SetOwner(this);
		closestWeapon -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
		CurrentWeapon = Cast<AWeapon>(closestWeapon);
		bHasPistol = true;
		MainWidget -> InitBulletUI(CurrentWeapon -> GetWeaponComponent() -> GetMaxAmmo());
	}
	else
	{
		if(CurrentWeapon != nullptr && bIsReloading == false)
		{
			CurrentWeapon -> SetOwner(nullptr);
			CurrentWeapon -> DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentWeapon = nullptr;
			bHasPistol = false;
		}
	}
}

void APrNetWorkCharacter::Shot(const FInputActionValue& Value)
{
	if(CurrentWeapon != nullptr && CurrentWeapon -> GetWeaponComponent() -> GetAmmo() > 0 && bIsReloading == false && EquipIndex == 0)
	{
		CurrentWeapon -> Fire();
		MainWidget -> RemoveBulletUI();
		PlayerAnimInstance -> PlayFireMontage();
	}
}

void APrNetWorkCharacter::Reload(const FInputActionValue& Value)
{
	if(CurrentWeapon != nullptr && bIsReloading == false && EquipIndex == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reload"));
		bIsReloading = true;
		CurrentWeapon -> Reload();
		PlayerAnimInstance -> PlayReloadMontage();
		MainWidget -> RemoveAllBulletUI();
	}
}

void APrNetWorkCharacter::PlayerDamage(float Amount)
{
	HeathSystem -> Damage(Amount);
	MainWidget -> UpdateHealthBar(HeathSystem -> GetHealth() / HeathSystem -> GetMaxHealth());
	HealthBar -> UpdateHealthBar(HeathSystem -> GetHealth() / HeathSystem -> GetMaxHealth());

	if(HeathSystem -> GetHealth() <= 0)
	{
		bIsDead = true;
	}
}

void APrNetWorkCharacter::InitMainWidget()
{
	APlayerController * PlayerController = Cast<APlayerController>(GetController());
	/*if(PlayerController == nullptr)
	{
		return;
	}*/
	
	MainWidget = CastChecked<UMainWidget>(CreateWidget(GetWorld(), MainWidgetClass));
	MainWidget -> AddToViewport();
}


void APrNetWorkCharacter::UpdateBulletUI()
{
	MainWidget -> InitBulletUI(CurrentWeapon -> GetWeaponComponent() -> GetMaxAmmo());
}



void APrNetWorkCharacter::OnPickUpOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor -> ActorHasTag("Weapon"))
	{
		OverlappingActors.Add(OtherActor);
		UKismetSystemLibrary::PrintString(this, "Pickup Weapon", true, true, FLinearColor::Green, 1.f);
	}
}

void APrNetWorkCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor -> ActorHasTag("Weapon"))
	{
		OverlappingActors.Remove(OtherActor);
		UKismetSystemLibrary::PrintString(this, "End Pickup Weapon", true, true, FLinearColor::Red, 1.f);
	}
}
