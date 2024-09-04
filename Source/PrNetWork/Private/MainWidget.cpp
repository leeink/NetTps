// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"

void UMainWidget::SetActiveCrossHair(bool bIsActive)
{
	if (CrossHairImage != nullptr)
	{
		CrossHairImage->SetVisibility(bIsActive ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		BulletPanel->SetVisibility(bIsActive ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UMainWidget::InitBulletUI(int MaxAmmo)
{
	for(int i = 0; i < MaxAmmo; i++)
	{
		AddBulletUI();
	}
}

void UMainWidget::AddBulletUI()
{
	auto* BulletWidget = CreateWidget<UUserWidget>(GetWorld(), BulletWidgetClass);
	BulletPanel->AddChildToUniformGrid(BulletWidget, 0, BulletPanel -> GetChildrenCount());
}

void UMainWidget::RemoveBulletUI()
{
	if(BulletPanel -> GetChildrenCount() == 0)
	{
		return;
	}
	
	BulletPanel->RemoveChildAt(BulletPanel->GetChildrenCount() - 1);
}

void UMainWidget::RemoveAllBulletUI()
{
	for(auto child : BulletPanel->GetAllChildren())
	{
		BulletPanel->RemoveChild(child);
	}
}

void UMainWidget::UpdateHealthBar(float Percent)
{
	if(HealthBar != nullptr)
	{
		HealthBar->SetPercent(Percent);
	}
}
