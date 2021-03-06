// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

UENUM(BlueprintType)
enum class InventoryError : uint8
{
	ESuccess					UMETA(DisplayName = "Success"),
	EInvalidStatUsed			UMETA(DisplayName = "InvalidStatUsed"), 
	EDuplicateItemType			UMETA(DisplayName = "DuplicateItemType"),
	EInvalidItemType			UMETA(DisplayName = "InvalidItemType"),
	EMaxQuantityExceeded		UMETA(DisplayName = "MaxQuantityExceeded"),
	ENoItemsToConsume			UMETA(DisplayName = "NoItemsToConsume"),
	ENotEquippable				UMETA(DisplayName = "NotEquippable"),
	EAlreadyEquipped			UMETA(DisplayName = "AlreadyEquipped"),
	ENotEquipped				UMETA(DisplayName = "NotEquipped"),
	ENotConsumable				UMETA(DisplayName = "NotConsumable"),
	EDuplicateStat				UMETA(DisplayName = "DuplicateStat")
};

USTRUCT(BlueprintType)
struct FBoostAndDuration
{
	GENERATED_BODY()

	// The boost to give to the desired stat
	UPROPERTY(BlueprintReadWrite, Category = "BoostAndDuration")
	int Boost = 0;

	// The duration of the effect. 0 indicates no duration (i.e infinite). Negative values treated as 0.
	UPROPERTY(BlueprintReadWrite, Category = "BoostAndDuration")
	int Duration = 0;
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	// The name of this inventory item
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	FString Name = "";

	// The flavor text of this inventory item
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	FString FlavorText = "";

	// The thumbnail of this inventory item
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	UTexture2D* Thumbnail = nullptr;

	// The full image of this inventory item
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	UTexture2D* FullImage = nullptr;

	// A TMap containing strings specifying a stat and a BoostAndDuration 
	// struct which specifies the boost to the respective stat as well as the
	// duration of the boost.
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	TMap<FString, FBoostAndDuration> StatsBoostsAndDurations;

	// The current quantity of this inventory item. Negative values treated as 0.
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	int Quantity = 0;

	// The maximum allowable quantity of this inventory item. Negative values treated as 0.
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	int MaximumQuantity = 0;

	// Is this item equippable?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool IsEquippable = false;

	// Is this item equipped?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool IsEquipped = false;

	// Is this this item consumable?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool IsConsumable = false;
};

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

	/** Adds a possible stat for items included in this inventory. Ideally 
	 * should add all possible stats once on BeginPlay.
	 * @param PossibleStat - Defines a possible stat for items in this 
	 * inventory.
	 * @return ESuccess if this PossibleStat was successfully added to the 
	 * internal list of possible stats. 
	 * EDuplicateStat if this stat is already in the internal list.
	 * @warning For maximum safety PossibleStat should probably be defined as
	 * a UENUM and converted to a string using the GETENUMSTRING macro.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError AddPossibleStat(const FString PossibleStat);

	/** Gets the possible stats for items included in this inventory. 
	 * @return TSet of all possible stats in this inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FString> GetPossibleStats();

	/** Adds an inventory item type. Ideally should add all possible items 
	 * once on BeginPlay.
	 * @param Name - The name of this inventory item.
	 * @param FlavorText - The flavor text for this inventory item.
	 * @param Thumbnail - The texture to be used for this inventory item's 
	 * thumbnail.
	 * @param FullImage - The texture to be used for this inventory item's 
	 * full image.
	 * @param StatsBoostsAndDurations - A TMap containing strings 
	 * specifying a stat and a BoostAndDuration struct which specifies the 
	 * boost to the respective stat as well as the duration of the boost. 
	 * @param MaximumQuantity - The maximum allowable quantity of this item.
	 * @return ESuccess if item type was successfully added to inventory. 
	 * EInvalidStatUsed if a stat in statsBoostsAndDurations was not specified 
	 * in the UInventory constructor.
	 * EDuplicateItemType if an item in this inventory with the same name 
	 * already exists.
	 * @warning For maximum safety all strings should probably be defined as a
	 * UENUM and converted to a string using the GETENUMSTRING macro.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError AddInventoryItemType(const FString& Name,
									const FString& FlavorText, 
									const UTexture2D* Thumbnail, 
									const UTexture2D* FullImage,
									const TMap<FString, FBoostAndDuration> StatsBoostsAndDurations,
									const int MaximumQuantity = 1,
									const bool IsConsumable = true,
									const bool IsEquippable = false);

	/** Add a desired quantity of an item in the inventory
	 * @param ItemToAdd is a string containing the name of an item in the
	 * inventory to add quantity to.
	 * @param Quantity is the number of items of ItemToAdd type to add 
	 * to the inventory.
	 * @return ESuccess if ItemToAdd count was successfully incremented by
	 * Quantity. 
	 * EInvalidItemType if ItemToAdd does not exist in the inventory.
	 * EMaxQuantityExceeded if Quantity would exceed the maximum quantity of
	 * ItemToAdd.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError AddItem(const FString& ItemToAdd, const int Quantity = 1);

	/** Consume a desired quantity of an item in the inventory. ItemToConsume
	 * must be consumable. If Quantity is greater than quantity of ItemToConsume,
	 * ItemToConsume.Quantity will be set to 0.
	 * @param ItemToConsume is a string containing the name of an item in the
	 * inventory to consume Quantity of. 
	 * @param Quantity is the number of items of ItemToConsume type to consume
	 * from the inventory.
	 * @return ESuccess if quantity of ItemToConsume was successfully reduced
	 * by Quantity.
	 * EInvalidItemType if ItemToConsume does not exist in the inventory.
	 * ENoItemsToConsume if quantity of ItemToConsume is 0.
	 * ENotConsumable if item is not consumable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError ConsumeItem(const FString& ItemToConsume, const int Quantity = 1);

	/** Equip an item in the inventory. This sets the isEquipped field of
	 * ItemToEquip to true. ItemToEquip must be equippable. Quantity of
	 * ItemToEquip does not change.
	 * @param ItemToEquip is a string containing the name of an item in the
	 * inventory to equip.
	 * @return ESuccess if ItemToEquip was successfully equipped.
	 * EInvalidItemType if ItemToEquip does not exist in the inventory.
	 * ENotEquippable if ItemToEquip is not equippable.
	 * EAlreadyEquipped if item is already equipped.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError EquipItem(const FString& ItemToEquip);
	
	/** Unequip an item in the inventory. This sets the isEquipped field of
	 * ItemToUnequip to false. ItemToUnequip must be equippable. Quantity of
	 * ItemToUnequip does not change.
	 * @param ItemToUnequip is a string containing the name of an item in the
	 * inventory to unequip.
	 * @return ESuccess if ItemToUnequip was successfully unequipped.
	 * EInvalidItemType if ItemToEquip does not exist in the inventory.
	 * ENotEquippable if ItemToEquip is not equippable.
	 * ENotEquipped if item is not equipped.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	InventoryError UnequipItem(const FString& ItemToUnequip);
	
	/** Get the current inventory 
	 * @return TMap containing all inventory items and their counts.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventoryItem> GetInventory();

	/** Get equipped items
	 * @return TSet containing all currently equipped inventory items.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventoryItem> GetEquippedItems();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	TSet<FString> PossibleStats;

	TMap<FString, FInventoryItem> Inventory;
};
