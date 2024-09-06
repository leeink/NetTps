// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "EngineUtils.h"
#include "PrNetWork.h"
#include "PrNetWorkCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANetActor::ANetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetSphereRadius(36.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName(FName("NoCollision"));
	Mesh->SetRelativeScale3D(FVector(0.5f));

	bReplicates = true;
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = Mesh -> CreateAndSetMaterialInstanceDynamic(0);

	if(HasAuthority())
	{
		GetWorld() -> GetTimerManager().SetTimer(ColorTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			ChangeColor = FLinearColor(FMath::RandRange(0.f, 1.f),
				FMath::RandRange(0.f, 1.f),
				FMath::RandRange(0.f, 1.f));
					OnRep_ChangeColor();
		}), 1.f, true);
	}
}

void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetActor, RotYaw);
	DOREPLIFETIME(ANetActor, ChangeColor);
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();
	FindOwner();
	//RotateYaw(DeltaTime);

	if(HasAuthority())
	{
		AddActorLocalRotation(FRotator(0.f, 50* DeltaTime, 0.f));
		RotYaw = GetActorRotation().Yaw;
	}
}

void ANetActor::PrintNetLog()
{
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection!") : TEXT("InValid Connection!");
	const FString ownerName = GetOwner() != nullptr ? GetOwner() -> GetName() : TEXT("No Owner");
	const FString logStr = FString::Printf(TEXT("Connection : %s\n LocalRole: %s\nRemoteRole : %s\nOwner : %s"), *conStr, *ownerName, *LOCALROLE, *REMOTEROLE);

	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Red, 0.01f);
}

void ANetActor::FindOwner()
{
	if(HasAuthority())
	{
		AActor* newOwner = nullptr;
		float minDistance = SearchDistance;

		for(TActorIterator<APrNetWorkCharacter> it(GetWorld()); it; ++it)
		{
			AActor* otherActor = *it;
			float dist = GetDistanceTo(otherActor);

			if(dist < minDistance)
			{
				minDistance = dist;
				newOwner = otherActor;
			}
		}
		if(GetOwner() != newOwner)
		{
			SetOwner(newOwner);
		}
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), SearchDistance, 16, FColor::Orange, true,0.01f, 0, 1.f);
}

void ANetActor::OnRep_RotYaw()
{
	FRotator NewRot = GetActorRotation();
	NewRot.Yaw = RotYaw;
	SetActorRotation(NewRot);
}

void ANetActor::OnRep_ChangeColor()
{
	if(DynamicMaterial)
	{
		DynamicMaterial -> SetVectorParameterValue(TEXT("FloorColor"), ChangeColor);
	}
}