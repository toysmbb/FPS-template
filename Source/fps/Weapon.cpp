// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Projectile.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	MuzzleLocation->SetupAttachment(WeaponMesh);
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

AProjectile* AWeapon::FireProjectile(const FVector& ShootDirection)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	const float CurrentTime = World->GetTimeSeconds();

	if (CurrentTime - LastFireTime < FireRate)
		return nullptr;

	LastFireTime = CurrentTime;

	if (!Projectileref) return nullptr;

	const FVector SpawnLocation = MuzzleLocation->GetComponentLocation();
	const FRotator SpawnRotation = ShootDirection.Rotation();

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = GetInstigator();

	AProjectile* Projectile = World->SpawnActor<AProjectile>(Projectileref, SpawnLocation, SpawnRotation, Params);

	if (Projectile)
	Projectile->InitProjectile(ShootDirection, InDamage, InSpeed);

	if (!MuzzleNiagara)
		return nullptr;
	
	NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleNiagara, MuzzleLocation, NAME_None,
		FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset,
		true, ENCPoolMethod::None, true, true);
	
	return Projectile;
}

