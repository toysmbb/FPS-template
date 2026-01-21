// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharPC.h"
#include "MainCharacter.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;
class UInputMappingContext;
class UStaticMeshComponent;
class USoundCue;
class UInteract_UMG;
class UHud_UMG;
class UHitCounter_UMG;
class AWeapon;
class ATarget;
class AStartButton;

UCLASS()
class FPS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* IA_LookYaw;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_LookPitch;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Fire;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Interact;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UInteract_UMG> InteractWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	UInteract_UMG* InteractWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHud_UMG> HudWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	UHud_UMG* HudWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHitCounter_UMG> HitWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	UHitCounter_UMG* HitWidget;
private:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* MainMesh;

	UPROPERTY(EditAnywhere, Category="Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundCue> FireSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponRef;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	AWeapon* Weapon;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RecoilPitch = 1.f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RecoilYaw = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RecoilDistance = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RecoilSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RecoilReturnSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Target")
	TSubclassOf<ATarget> TargetRef;

	UPROPERTY(EditAnywhere, Category = "Target")
	ATarget* Target;

	UPROPERTY()
	AActor* FocusedActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Target")
	FVector StartTargetSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Target")
	FVector EndTargetSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Target")
	float SpawnDelay = 3.0f;
	
	UPROPERTY(EditAnywhere, Category = "Interact")
	float InteractDistance = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	bool bisCanFire;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bisRecoiling = false;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bisReturning = false;

	UPROPERTY()
	int32 HitCounter = 0;

	FTimerHandle SpawnTargetTimer;
	FTimerHandle FireCooldownTimer;
	FHitResult InteractionHitResult;
	FVector ViewVector;
	FRotator ViewRotation;
	FVector StartWeaponLocation;
	FVector TargetWeaponLocation;
	FVector StartMeshLocation;
	FVector TargetMeshLocation;
public:
	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void LookYaw(const FInputActionValue& value);

	UFUNCTION()
	void LookPitch(const FInputActionValue& value);

	UFUNCTION()
	void Fire();

	UFUNCTION()
	void UpdateInteractTrace();

	UFUNCTION()
	void CallSpawnTarget();

	UFUNCTION()
	void SpawnTarget();

	UFUNCTION()
	void StopSpawnTarget();

	UFUNCTION()
	void Interact();

	UFUNCTION()
	void ResetFire();

	UFUNCTION()
	void OnProjectileHitTarget();

	UFUNCTION()
	void StartRecoil();

	UFUNCTION()
	void EndRecoil();

	UFUNCTION()
	void UpdateWeaponRecoil(float DeltaTime);
};
