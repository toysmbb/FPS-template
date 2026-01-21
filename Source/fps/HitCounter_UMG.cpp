// Fill out your copyright notice in the Description page of Project Settings.


#include "HitCounter_UMG.h"

void UHitCounter_UMG::SetCounter(int32 CounterValue)
{
		Counter->SetText(FText::AsNumber(CounterValue));
}

