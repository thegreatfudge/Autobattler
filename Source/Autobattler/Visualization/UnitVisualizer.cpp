// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitVisualizer.h"

// Sets default values
AUnitVisualizer::AUnitVisualizer()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	RootComponent = UnitMesh;
}

// Called when the game starts or when spawned
void AUnitVisualizer::BeginPlay()
{
	Super::BeginPlay();
	TargetPosition = GetActorLocation();
}

// Called every frame
void AUnitVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAlive)
	{
		UpdateVisualEffects(DeltaTime);
		return;
	}

	UpdateMovement(DeltaTime);
	UpdateVisualEffects(DeltaTime);
}

void AUnitVisualizer::Initialize(int32 InUnitId, uint8 InTeam, int32 InHP, int32 InMaxHP)
{
	UnitId = InUnitId;
	Team = InTeam;
	CurrentHP = InHP;
	MaxHP = InMaxHP;
	bIsAlive = true;

	// Set team material
	UMaterialInterface* BaseMaterial = (Team == static_cast<uint8>(ETeamColor::Red)) ? RedTeamMaterial : BlueTeamMaterial;
    
	if (BaseMaterial && UnitMesh)
	{
		DynamicMaterial = UnitMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, BaseMaterial);
	}

	TargetPosition = GetActorLocation();
}

void AUnitVisualizer::SetTargetPosition(const FVector& NewTarget)
{
	TargetPosition = NewTarget;
}

void AUnitVisualizer::SetHP(int32 NewHP)
{
	int32 OldHP = CurrentHP;
	CurrentHP = FMath::Max(0, NewHP);
    
	if (OldHP > CurrentHP)
	{
		// Flash effect when hit
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("EmissiveIntensity"), 3.0f);
		}
	}
}

void AUnitVisualizer::SetAttacking(bool bAttacking)
{
	if (!bIsAttacking && bAttacking)
	{
		OnAttack();
        
		// Make unit glow when attacking
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("EmissiveIntensity"), 2.0f);
		}
	}
	else if (bIsAttacking && !bAttacking)
	{
		// Reset glow
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("EmissiveIntensity"), 0.5f);
		}
	}
    
	bIsAttacking = bAttacking;
}

void AUnitVisualizer::SetAlive(bool bAlive)
{
	if (bIsAlive && !bAlive)
	{
		PlayDeathEffect();
		OnDeath();
	}
    
	bIsAlive = bAlive;
}

void AUnitVisualizer::PlayDeathEffect()
{
	DeathEffectTimer = 1.0f;
}

void AUnitVisualizer::UpdateMovement(float DeltaTime)
{
	FVector CurrentPos = GetActorLocation();
	FVector Direction = TargetPosition - CurrentPos;
	float Distance = Direction.Size();
    
	if (Distance > 1.0f)
	{
		FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPosition, DeltaTime, InterpolationSpeed);
		SetActorLocation(NewPos);
	}
}

void AUnitVisualizer::UpdateVisualEffects(float DeltaTime)
{
	if (!bIsAlive && DeathEffectTimer > 0.0f)
	{
		DeathEffectTimer -= DeltaTime;
        
		float Alpha = DeathEffectTimer;
		UnitMesh->SetRelativeScale3D(FVector(0.5f * Alpha));
        
		if (DeathEffectTimer <= 0.0f)
		{
			Destroy();
		}
	}
}

