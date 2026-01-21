// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AProjectile;
class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystem;

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Muzzle")
	USceneComponent* MuzzleLocation;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> Projectileref;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> MuzzleNiagara;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float FireRate = 0.5f;

	UPROPERTY()
	float LastFireTime = -999.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float InSpeed = 6000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float InDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	float MuzzleFlashSpawn = 0.3f;

	FTimerHandle MuzzleFlashTimer;

	AProjectile* FireProjectile(const FVector& ShootDirection);
};
