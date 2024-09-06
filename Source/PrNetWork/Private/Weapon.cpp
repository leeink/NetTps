// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "HealthComponent.h"
#include "PrNetWorkCharacter.h"
#include "WeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::Fire()
{
	FHitResult Hit;
	FVector Start = WeaponComponent->GetSocketLocation("MuzzleFlash");
	FVector End = Start + UGameplayStatics::GetPlayerCameraManager(this, 0) -> GetActorForwardVector() * 10000.f;

	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	GetWorld() -> LineTraceSingleByChannel(Hit, Start, End, Channel, Params);
	UKismetSystemLibrary::DrawDebugLine(this, Start, End, FLinearColor::Red, .5f, 1.f);

	if(APrNetWorkCharacter* Target = Cast<APrNetWorkCharacter>(Hit.GetActor()))
	{
		Target->PlayerDamage(WeaponComponent->GetDamage());
	}
	
	if(Hit.GetActor() != nullptr && FireSpark != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireSpark, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(1.f));
	}

	WeaponComponent->SetAmmo(WeaponComponent->GetAmmo() - 1);
}

void AWeapon::Reload()
{
	if(WeaponComponent->GetAmmo() >= WeaponComponent->GetMaxAmmo())
	{
		return;
	}

	WeaponComponent->SetAmmo(WeaponComponent->GetMaxAmmo());
}

void AWeapon::ActivationCollion()
{
	Collision -> SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivationCollision()
{
	Collision -> SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
