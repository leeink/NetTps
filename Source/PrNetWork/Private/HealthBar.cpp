// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"

#include "Components/ProgressBar.h"

void UHealthBar::UpdateHealthBar(float Percent)
{
	if(HealthBar != nullptr)
	{
		HealthBar->SetPercent(Percent);
	}
}
