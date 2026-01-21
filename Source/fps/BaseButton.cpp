// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseButton.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABaseButton::ABaseButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

// Called when the game starts or when spawned
void ABaseButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

