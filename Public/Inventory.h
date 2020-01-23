// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/ObjectMacros.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

UENUM()
enum class InventoryError : int32
{
	ESuccess					UMETA(DisplayName = "Success"),
	EInvalidStatUsed			UMETA(DisplayName = "InvalidStatUsed"), 
	EDuplicateItemType			UMETA(DisplayName = "DuplicateItemType"),
	EInvalidItemType			UMETA(DisplayName = "InvalidItemType"),
	EMaxQuantityExceeded		UMETA(DisplayName = "MaxQuantityExceeded"),
	ENotEnoughItemsToConsume	UMETA(DisplayName = "NotEnoughItemsToConsume"),
	ENotEquippable				UMETA(DisplayName = "NotEquippable"),
	EAlreadyEquipped			UMETA(DisplayName = "AlreadyEquipped"),
	ENotEquipped				UMETA(DisplayName = "NotEquipped"),
	ENotConsumable				UMETA(DisplayName = "NotConsumable")
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
	TMap<FString, FBoostAndDuration> statsBoostsAndDurations;

	// The maximum allowable quantity of this inventory item. Negative values treated as 0.
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	int MaximumQuantity = 0;

	// Is this item equippable?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool isEquippable = false;

	// Is this item equipped?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool isEquipped = false;

	// Is this this item consumable?
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	bool isConsumable = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

	/** Sets the possible stats for items included in this inventory. Should
	 * just call once on BeginPlay.
	 * @param PossibleStats - Defines possible stats to be used by all items
	 * in this inventory
	 * @warning For maximum safety possibleStats should probably be defined as
	 * UENUMs and converted to strings using the GETENUMSTRING macro.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetPossibleStats(TSet<FString> PossibleStats);

	/** Adds an inventory item type. Ideally should add all possible items 
	 * once on BeginPlay.
	 * @param Name - The name of this inventory item.
	 * @param FlavorText - The flavor text for this inventory item.
	 * @param Thumbnail - The texture to be used for this inventory item's 
	 * thumbnail.
	 * @param FullImage - The texture to be used for this inventory item's 
	 * full image.
	 * @param StatsBoostsAndDurations - An unordered_map containing strings 
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
									const bool isConsumable = true,
									const bool isEquippable = false);

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
	 * must be consumable.
	 * @param ItemToConsume is a string containing the name of an item in the
	 * inventory to consume Quantity of. 
	 * @param Quantity is the number of items of ItemToConsume type to consume
	 * from the inventory.
	 * @return ESuccess if quantity of ItemToConsume was successfully reduced
	 * by Quantity.
	 * EInvalidItemType if ItemToConsume does not exist in the inventory.
	 * ENotEnoughItemsToConsume if quantity of ItemToConsume is less than Quantity.
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
	TMap<FInventoryItem, int> GetInventory();

	/** Get equipped items
	 * @return TSet containing all currently equipped inventory items.
	 */
	TSet<FInventoryItem> GetEquippedItems();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
