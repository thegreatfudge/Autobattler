// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTile.h"

// Sets default values
AHexTile::AHexTile()
{
	PrimaryActorTick.bCanEverTick = false;

	HexMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HexMesh"));
	RootComponent = HexMesh;
}

void AHexTile::SetCoordinates(int32 NewQ, int32 NewR)
{
	Q = NewQ;
	R = NewR;
}

