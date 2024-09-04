// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRNETWORK_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
	class UImage* CrossHairImage;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* BulletPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> BulletWidgetClass;

public:
	void SetActiveCrossHair(bool bIsActive);

	void InitBulletUI(int MaxAmmo);
	void AddBulletUI();
	void RemoveBulletUI();
	void RemoveAllBulletUI();
};
