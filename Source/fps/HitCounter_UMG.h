// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HitCounter_UMG.generated.h"

class UTextBlock;

UCLASS()
class FPS_API UHitCounter_UMG : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Counter;

	void SetCounter(int32 CounterValue);
};
