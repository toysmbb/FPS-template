// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Weapon.h"
#include "Target.h"
#include "Blueprint/UserWidget.h"
#include "Interact_UMG.h"
#include "Hud_UMG.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BaseButton.h"
#include "StartTargetButton.h"
#include "StopTargetButton.h"
#include "HitCounter_UMG.h"
#include "Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	MainMesh->SetupAttachment(Camera);
	MainMesh->SetOnlyOwnerSee(true);
	MainMesh->CastShadow = false;
	MainMesh->bCastDynamicShadow = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
    bisCanFire = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		SubSystem->AddMappingContext(IMC, 0);
	}
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponRef, Params);
	Weapon->AttachToComponent(MainMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
	StartWeaponLocation = Weapon->GetRootComponent()->GetRelativeLocation();
	StartMeshLocation = MainMesh->GetRelativeLocation();

	if(InteractWidgetClass)
	{
		InteractWidget = CreateWidget<UInteract_UMG>(GetWorld(), InteractWidgetClass);
		InteractWidget->AddToViewport();
		InteractWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (HudWidgetClass)
	{
		HudWidget = CreateWidget<UHud_UMG>(GetWorld(), HudWidgetClass);
		HudWidget->AddToViewport();
		HudWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (HitWidgetClass)
	{
		HitWidget = CreateWidget<UHitCounter_UMG>(GetWorld(), HitWidgetClass);
		HitWidget->AddToViewport();
		HitWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateInteractTrace();
	UpdateWeaponRecoil(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(IA_LookYaw, ETriggerEvent::Triggered, this, &AMainCharacter::LookYaw);
		EnhancedInputComponent->BindAction(IA_LookPitch, ETriggerEvent::Triggered, this, &AMainCharacter::LookPitch);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Triggered, this, &AMainCharacter::Fire);
		EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AMainCharacter::Interact);
	}
}

void AMainCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementVector = value.Get<FVector2D>();
	if (MovementVector.X != 0.f)
		AddMovementInput(GetActorForwardVector(), MovementVector.X);
	
	
	if (MovementVector.Y != 0.f)
		AddMovementInput(GetActorRightVector(), MovementVector.Y);
}

void AMainCharacter::LookYaw(const FInputActionValue& value)
{
	AddControllerYawInput(value.Get<float>());
}

void AMainCharacter::LookPitch(const FInputActionValue& value)
{
	AddControllerPitchInput(value.Get<float>());
}

void AMainCharacter::Fire()
{
	if (!bisCanFire)
		return;

	bisCanFire = false;

	AddControllerPitchInput(-RecoilPitch);
	AddControllerYawInput(FMath::RandRange(-RecoilYaw, RecoilYaw));
	StartRecoil();

	const FVector ShooterDirection = GetController()->GetControlRotation().Vector();
	AProjectile* Projectile = Weapon->FireProjectile(ShooterDirection);
	if(Projectile)
		Projectile->OnHitTarget.AddDynamic(this, &AMainCharacter::OnProjectileHitTarget);
	
	if(FireSound)
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	GetWorldTimerManager().SetTimer(FireCooldownTimer, this, &AMainCharacter::ResetFire, Weapon->FireRate, false);
}

void AMainCharacter::UpdateInteractTrace()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->GetPlayerViewPoint(ViewVector, ViewRotation);
	FVector VecDirection = ViewRotation.Vector() * InteractDistance;
	FVector InteractEnd = ViewVector + VecDirection;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(InteractionHitResult, ViewVector, InteractEnd, ECollisionChannel::ECC_Visibility, QueryParams);
	ABaseButton* HitButton = bHit ? Cast<ABaseButton>(InteractionHitResult.GetActor()) : nullptr;
	if (HitButton)
	{
		FocusedActor = HitButton;
		InteractWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		FocusedActor = nullptr;
		InteractWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainCharacter::CallSpawnTarget()
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTargetTimer))
		return;
	GetWorldTimerManager().SetTimer(SpawnTargetTimer, this, &AMainCharacter::SpawnTarget, SpawnDelay, true );
}

void AMainCharacter::SpawnTarget()
{
	FVector NewLocation;
	NewLocation.X = FMath::RandRange(FMath::Min(StartTargetSpawnLocation.X, EndTargetSpawnLocation.X), 
		FMath::Max(StartTargetSpawnLocation.X, EndTargetSpawnLocation.X));
    
	NewLocation.Y = FMath::RandRange(FMath::Min(StartTargetSpawnLocation.Y, EndTargetSpawnLocation.Y),
			FMath::Max(StartTargetSpawnLocation.Y, EndTargetSpawnLocation.Y));
	
	NewLocation.Z = FMath::RandRange(FMath::Min(StartTargetSpawnLocation.Z, EndTargetSpawnLocation.Z),
		FMath::Max(StartTargetSpawnLocation.Z, EndTargetSpawnLocation.Z));

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Target = GetWorld()->SpawnActor<ATarget>(TargetRef, Params);
	Target->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AMainCharacter::StopSpawnTarget()
{
	if (GetWorldTimerManager().IsTimerActive(SpawnTargetTimer))
		GetWorldTimerManager().ClearTimer(SpawnTargetTimer);

	HitCounter = 0;

	if (HitWidget)
		HitWidget->SetCounter(HitCounter);
}

void AMainCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact func is called"));
	if(FocusedActor)
	{
		if (AActor* InteractActor = Cast<AStartTargetButton>(FocusedActor))
			CallSpawnTarget();
		if (AActor* InteractActor = Cast<AStopTargetButton>(FocusedActor))
			StopSpawnTarget();
	}
}

void AMainCharacter::ResetFire()
{
	bisCanFire = true;	
}

void AMainCharacter::OnProjectileHitTarget()
{
	HitCounter++;
	UE_LOG(LogTemp, Warning, TEXT("Hit count: %d"), HitCounter);

	if (HitWidget)
		HitWidget->SetCounter(HitCounter);
}

void AMainCharacter::StartRecoil()
{
	bisRecoiling = true;
	bisReturning = false;
    
	TargetWeaponLocation = StartWeaponLocation + FVector(0.f, -RecoilDistance, 0.f);
	TargetMeshLocation = StartMeshLocation + FVector(-RecoilDistance, 0.f, 0.f);
}

void AMainCharacter::EndRecoil()
{
	bisRecoiling = false;
	bisReturning = true;
}

void AMainCharacter::UpdateWeaponRecoil(float DeltaTime)
{
	USceneComponent* WeaponRoot = Weapon->GetRootComponent();

	if (bisRecoiling) {
		FVector NewLoc = FMath::VInterpTo(WeaponRoot->GetRelativeLocation(), TargetWeaponLocation, DeltaTime, RecoilSpeed);
		FVector NewMeshLoc = FMath::VInterpTo(MainMesh->GetRelativeLocation(), TargetMeshLocation, DeltaTime, RecoilSpeed);

		WeaponRoot->SetRelativeLocation(NewLoc);
		MainMesh->SetRelativeLocation(NewMeshLoc);

		if (NewLoc.Equals(TargetWeaponLocation, 0.1f) && NewMeshLoc.Equals(TargetMeshLocation, 0.1))
		{
			bisRecoiling = false;
			bisReturning = true;
		}
	}
	else if(bisReturning)
	{
		FVector NewLoc = FMath::VInterpTo(WeaponRoot->GetRelativeLocation(), StartWeaponLocation, DeltaTime, RecoilReturnSpeed);
		FVector NewMeshLoc = FMath::VInterpTo(MainMesh->GetRelativeLocation(), StartMeshLocation, DeltaTime, RecoilSpeed);

		WeaponRoot->SetRelativeLocation(NewLoc);
		MainMesh->SetRelativeLocation(NewMeshLoc);
		if (NewLoc.Equals(StartWeaponLocation, 0.1f) && NewMeshLoc.Equals(StartMeshLocation, 0.1))
			bisReturning = false;
	}
}

