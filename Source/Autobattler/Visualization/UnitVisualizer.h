// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimulationManager.h"
#include "GameFramework/Actor.h"
#include "UnitVisualizer.generated.h"

UCLASS()
class AUTOBATTLER_API AUnitVisualizer : public AActor
{
	GENERATED_BODY()
	
public:
    AUnitVisualizer();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* UnitMesh;

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    int32 UnitId = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    uint8 Team = static_cast<uint8>(ETeamColor::Red);

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    int32 CurrentHP = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    int32 MaxHP = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    bool bIsAlive = true;

    UPROPERTY(BlueprintReadOnly, Category = "Unit")
    bool bIsAttacking = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InterpolationSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    class UMaterialInterface* RedTeamMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    class UMaterialInterface* BlueTeamMaterial;

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void Initialize(int32 InUnitId, uint8 InTeam, int32 InHP, int32 InMaxHP);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void SetTargetPosition(const FVector& NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void SetHP(int32 NewHP);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void SetAttacking(bool bAttacking);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    void SetAlive(bool bAlive);

    UFUNCTION(BlueprintCallable, Category = "Unit")
    bool IsAlive() const { return bIsAlive; }

    UFUNCTION(BlueprintCallable, Category = "Effects")
    void PlayDeathEffect();

private:
    FVector TargetPosition;
    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;
    float DeathEffectTimer = 0.0f;

    void UpdateMovement(float DeltaTime);
    void UpdateVisualEffects(float DeltaTime);

};
