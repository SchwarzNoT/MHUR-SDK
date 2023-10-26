#pragma once
#include "precomp.h"



class UObject {


public:

	uintptr_t baseAddress;
	int FNameID;
	std::string Name;
	UObject(uintptr_t baseAddy);
	UObject();

	bool isValid();


};


class AActor : public UObject {

public:
	Vector3 W2S;
	float Distance;
	USceneComponent RootComponent;

	AActor(uintptr_t baseAddy);	
	AActor();

	virtual Vector3 getPos();
	virtual float getDistance();
	virtual Vector3 getW2S();


};

class ACharacterBattle : public AActor {

public:
	int playerID;
	uintptr_t playerStateBase;
	uintptr_t meshAddress;
	uintptr_t boneArrayAddr;
	INT32 boneArrayCount;
	std::vector< std::vector<BoneInfo>> skeleton;
	uintptr_t currentWeaponPtr;
	Vector3 location;
	bool isHiddenForDeath;


	ACharacterBattle(uintptr_t baseAddy);

	ACharacterBattle();


	FTransform GetBoneIndex(int index);

	BoneInfo GetBoneWithRotation(BoneInfo toRefresh);

	void refreshSkeleton();

	UObject getCurrentWeapon();

	Vector3 getPos() override;

	float getDistance() override;
	Vector3 getW2S() override;

		


};

class APlayerStateBattle : public UObject {

public:
	uintptr_t pawnPtr;
	ACharacterBattle playerPawn;
	std::string playerName;

	APlayerStateBattle(uintptr_t baseAddy);
	APlayerStateBattle();
	std::string getPlayerName();
};



class FortItem : public  AActor {

public:
	std::string itemName;
	INT8 itemRarity;
	fortItemType itemType;
	weaponType weaponType;
	ImColor drawColor;
	bool isSearched;

	FortItem(uintptr_t baseAddy, Vector3 worldPos);

	FortItem(uintptr_t baseAddy, fortItemType itemType);

	FortItem();



};



extern std::vector<APlayerStateBattle> targetPawns;
extern std::vector<APlayerStateBattle> enemyPawns;



