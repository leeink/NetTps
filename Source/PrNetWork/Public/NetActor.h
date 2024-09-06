// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

struct FTimerHandle;

UCLASS()
class PRNETWORK_API ANetActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SearchDistance", meta = (AllowPrivateAccess = "true"))
	float SearchDistance = 100.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_RotYaw)
	float RotYaw = 0.f;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_ChangeColor)
	FLinearColor ChangeColor;

	UPROPERTY()
	TArray<AActor*> ActorList;

	FTimerHandle ColorTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PrintNetLog();
	void FindOwner();
	
	UFUNCTION()
	void OnRep_RotYaw();
	UFUNCTION()
	void OnRep_ChangeColor();
};
