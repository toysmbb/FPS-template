// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Target.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(10.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->IgnoreActorWhenMoving(GetOwner(), true);
	RootComponent = CollisionSphere;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->bAutoActivate = false;

	InitialLifeSpan = 5.0f; 
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

void AProjectile::InitProjectile(const FVector& ShooterDirection, float InDamage, float InSpeed)
{
	DamageValue = InDamage;
	ProjectileMovement->InitialSpeed = InSpeed;
	ProjectileMovement->MaxSpeed = InSpeed;
	ProjectileMovement->Velocity = ShooterDirection.GetSafeNormal() * InSpeed;
	if (TrailNiagara && NiagaraComp)
	{
		NiagaraComp->SetAsset(TrailNiagara);
		NiagaraComp->Activate(true);
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* AnyActor = Cast<AActor>(OtherActor);
	if(AnyActor)
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ImpactNiagara, RootComponent, NAME_None,
			FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector, EAttachLocation::KeepRelativeOffset,
			true, ENCPoolMethod::None, true, true);

	ATarget* CurrentTarget = Cast<ATarget>(OtherActor);
	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is hited by player"));
		if(TargetDestroySound)
		{
			UGameplayStatics::PlaySoundAtLocation(CurrentTarget, TargetDestroySound, CurrentTarget->GetActorLocation());
		}
		OnHitTarget.Broadcast();
		CurrentTarget->Destroy();
	}
}