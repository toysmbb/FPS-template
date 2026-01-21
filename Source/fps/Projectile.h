// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileHitTarget);

UCLASS()
class FPS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	void InitProjectile(const FVector& ShooterDirection, float InDamage, float InSpeed);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Collisin")
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> TrailNiagara;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> ImpactNiagara;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComp;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundCue> TargetDestroySound;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintAssignable, Category = "Projectile")
	FOnProjectileHitTarget OnHitTarget;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	float DamageValue = 20.0f;
};
