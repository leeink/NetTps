// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include "Components/Image.h"

void UMainWidget::SetActiveCrossHair(bool bIsActive)
{
	if (CrossHairImage != nullptr)
	{
		CrossHairImage->SetVisibility(bIsActive ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}
}
