// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


TArray<FInventoryItem> UInventory::GetEquippedItems()
{
	return TArray<FInventoryItem>();
}

// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

InventoryError UInventory::AddPossibleStat(const FString PossibleStat)
{

		possibleStats.Add(PossibleStat);

		return InventoryError();
}

TArray<FString> UInventory::GetPossibleStats()
{
	return TArray<FString>();
}

InventoryError UInventory::AddInventoryItemType(const FString& Name, 
												const FString& FlavorText, 
												const UTexture2D* Thumbnail, 
												const UTexture2D* FullImage, 
												const TMap<FString, FBoostAndDuration> StatsBoostsAndDurations, 
												const bool IsConsumable /* = true*/, 
												const bool IsEquippable /* = false*/)
{
	for (auto& Elem : StatsBoostsAndDurations)
	{
		if (!possibleStats.Contains(Elem.Key))
			return InventoryError::EInvalidStatUsed;
	}

	FInventoryItem inventoryItemToAdd;
	inventoryItemToAdd.Name = Name;
	inventoryItemToAdd.FlavorText = FlavorText;
	inventoryItemToAdd.Thumbnail = const_cast<UTexture2D*>(Thumbnail);
	inventoryItemToAdd.FullImage = const_cast<UTexture2D*>(FullImage);
	inventoryItemToAdd.StatsBoostsAndDurations = StatsBoostsAndDurations;
	inventoryItemToAdd.IsConsumable = IsConsumable;
	inventoryItemToAdd.IsEquippable = IsEquippable;
	
	if (inventory.Contains(Name))
		return InventoryError::EDuplicateItemType;

	inventory[Name] = inventoryItemToAdd;

	return InventoryError::ESuccess;
}

InventoryError UInventory::AddItem(const FString& ItemToAdd, const int Quantity)
{
	return InventoryError();
}

InventoryError UInventory::ConsumeItem(const FString& ItemToConsume, const int Quantity)
{
	return InventoryError();
}

InventoryError UInventory::EquipItem(const FString& ItemToEquip)
{
	return InventoryError();
}

InventoryError UInventory::UnequipItem(const FString& ItemToUnequip)
{
	return InventoryError();
}

TArray<FInventoryItem> UInventory::GetInventory()
{
	return TArray<FInventoryItem>();
}
