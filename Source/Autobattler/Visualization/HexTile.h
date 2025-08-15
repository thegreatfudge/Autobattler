// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.generated.h"

UCLASS()
class AUTOBATTLER_API AHexTile : public AActor
{
	GENERATED_BODY()
	
public:
	AHexTile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* HexMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Hex")
	int32 Q;

	UPROPERTY(BlueprintReadOnly, Category = "Hex")
	int32 R;

	UFUNCTION(BlueprintCallable, Category = "Hex")
	void SetCoordinates(int32 NewQ, int32 NewR);

};
