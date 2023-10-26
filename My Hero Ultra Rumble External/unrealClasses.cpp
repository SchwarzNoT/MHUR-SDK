#include "unrealClasses.h"





	UObject::UObject(uintptr_t baseAddy)
	{

		this->baseAddress = baseAddy;
		this->FNameID = util::Read<int>(this->baseAddress + 0x18, sizeof(FNameID));
		this->Name = GetNameFromFName(this->FNameID);

	}
	UObject::UObject() {

	}

	bool UObject::isValid() {

		int ObjFlags = util::Read<int>(this->baseAddress + 0x8, sizeof(ObjFlags));
		if (ObjFlags & PendingKill) {

			return false;
		}
		else {
			return true;
		}
	}








	AActor::AActor(uintptr_t baseAddy) : UObject(baseAddy) {


		this->RootComponent.RelativeLocation = util::Read<Vector3>(util::Read<uintptr_t>(this->baseAddress + offsets::AActor::RootComponent, sizeof(uintptr_t)) + offsets::USceneComponent::RelativeLocation, sizeof(this->RootComponent.RelativeLocation));



	}

	AActor::AActor() : UObject(0){}
	 Vector3 AActor::getPos() {
		this->RootComponent.RelativeLocation = util::Read<Vector3>(util::Read<uintptr_t>(this->baseAddress + offsets::AActor::RootComponent, sizeof(uintptr_t)) + offsets::USceneComponent::RelativeLocation, sizeof(this->RootComponent.RelativeLocation));
		return this->RootComponent.RelativeLocation;

	}
	float AActor::getDistance() {

		this->Distance = util::getDistance(CameraCache.POV.Location, this->RootComponent.RelativeLocation);
		return this->Distance;


	}
	Vector3 AActor::getW2S() {

		this->W2S = util::WorldToScreen(this->RootComponent.RelativeLocation, CameraCache);
		return this->W2S;

	}




	ACharacterBattle::ACharacterBattle(uintptr_t baseAddy) : AActor(baseAddy) {

		//meshnAddress
		this->meshAddress = util::Read<uintptr_t>(this->baseAddress + 0x280, sizeof(this->meshAddress));
		//Bone Array Addr
		this->boneArrayAddr = util::Read<uintptr_t>(this->meshAddress + 0x4c0, sizeof(this->boneArrayAddr));
		//Bone Array Count
		this->boneArrayCount = util::Read<INT32>(this->meshAddress + 0x4c8, sizeof(this->boneArrayCount));

		std::vector<BoneInfo> right = { BoneInfo(0,0,0,60),		BoneInfo(0,0,0,100),		BoneInfo(0,0,0, 101), BoneInfo(0,0,0, 102) };

		std::vector<BoneInfo> left = { BoneInfo(0,0,0,60),		BoneInfo(0,0,0,97),		BoneInfo(0,0,0, 92), BoneInfo(0,0,0, 96) };
		std::vector<BoneInfo> spineBones = { BoneInfo(0,0,0,60),		BoneInfo(0,0,0, 5),		BoneInfo(0,0,0, 4), BoneInfo(0,0,0, 2) };
		std::vector<BoneInfo> lowerright = { BoneInfo(0,0,0,2),		BoneInfo(0,0,0,136),		BoneInfo(0,0,0, 141), BoneInfo(0,0,0, 138) };
		std::vector<BoneInfo> lowerleft = { BoneInfo(0,0,0,2),		BoneInfo(0,0,0,129),		BoneInfo(0,0,0, 134), BoneInfo(0,0,0, 131) };

		this->skeleton = std::vector<std::vector<BoneInfo>>{  right, left, spineBones, lowerright, lowerleft };

		

	}
	ACharacterBattle::ACharacterBattle() : AActor(0){}

	
	FTransform ACharacterBattle::GetBoneIndex(int index)
	{
		return util::Read<FTransform>(this->boneArrayAddr + (index * 0x30), sizeof(FTransform));
	}

	BoneInfo ACharacterBattle::GetBoneWithRotation(BoneInfo toRefresh)
	{
		FTransform bone = GetBoneIndex(toRefresh.boneID);

		if (!bone.scale.x) {

			return toRefresh;
		}



		//CompToWorld
		FTransform ComponentToWorld = util::Read<FTransform>(this->meshAddress + 0x1c0, sizeof(FTransform));

		D3DMATRIX Matrix;
		Matrix = util::MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
		return BoneInfo(Matrix._41, Matrix._42, Matrix._43, toRefresh.boneID);
	}

	void ACharacterBattle::refreshSkeleton() {


		for (int i = 0; i < this->skeleton.size(); i++) {


			for (int j = 0; j < this->skeleton[i].size(); j++) {

				BoneInfo temp = GetBoneWithRotation(this->skeleton[i][j]);
				if (temp.isZero() && !this->skeleton[i][j].isZero()) {

					if (!this->skeleton[i][j].isZero()) {

						this->skeleton[i][j].W2S = util::WorldToScreen(Vector3(this->skeleton[i][j].x, this->skeleton[i][j].y, this->skeleton[i][j].z), CameraCache);

					}
					

				}
				else {
					this->skeleton[i][j] = temp;
					this->skeleton[i][j].W2S = util::WorldToScreen(Vector3(this->skeleton[i][j].x, this->skeleton[i][j].y, this->skeleton[i][j].z), CameraCache);
				}
			}

		}

	}

	UObject ACharacterBattle::getCurrentWeapon() {


		this->currentWeaponPtr = util::Read<uintptr_t>(this->baseAddress + 0x820, sizeof(this->currentWeaponPtr));

		return UObject(this->currentWeaponPtr);

	}

	Vector3 ACharacterBattle::getPos()  {

		this->location = Vector3(this->skeleton[4][0].x, this->skeleton[4][0].y, this->skeleton[4][0].z);
		return this->location;

	}

	float ACharacterBattle::getDistance()  {

		this->Distance = util::getDistance(CameraCache.POV.Location, Vector3(this->skeleton[0][0].x, this->skeleton[0][0].y, this->skeleton[0][0].z));
		return this->Distance;


	}
	Vector3 ACharacterBattle::getW2S()  {

		this->W2S = this->skeleton[4][0].W2S;
		return this->W2S;


	}







	APlayerStateBattle::APlayerStateBattle(uintptr_t baseAddy) : UObject(baseAddy) {
		
		this->playerName = getPlayerName();

		this->pawnPtr = util::Read<uintptr_t>(this->baseAddress + 0x280, sizeof(this->pawnPtr));
		this->playerPawn = ACharacterBattle(pawnPtr);

	}

	APlayerStateBattle::APlayerStateBattle() : UObject(0) {}

	std::string APlayerStateBattle::getPlayerName() {




			INT32 nameLength = util::Read<INT32>((this->baseAddress + 0x308), sizeof(nameLength));

			

			wchar_t* buffer = new wchar_t[nameLength * 2];
			SecureZeroMemory(buffer, nameLength * 2);

			uintptr_t nameAddr = util::Read<uintptr_t>((this->baseAddress + 0x300), sizeof(nameAddr));

			ReadProcessMemory(hProc, (BYTE*)(nameAddr), buffer, nameLength * 2, NULL);

			std::wstring ws(buffer);
			std::string str(ws.begin(), ws.end());
			delete[] buffer;

			return str;



		

	}


	FortItem::FortItem(uintptr_t baseAddy, Vector3 worldPos) : AActor(baseAddy) {

		this->RootComponent.RelativeLocation = worldPos;

		uintptr_t itemNameCont = util::Read<uintptr_t>(this->baseAddress + 0x78, sizeof(itemNameCont));

		uintptr_t FTEXTPTR = util::Read<uintptr_t>(itemNameCont + 0x38, sizeof(FTEXTPTR));

		INT32 ftextSz = util::Read<INT32>(itemNameCont + 0x40, sizeof(ftextSz));

		wchar_t* buffer = new wchar_t[ftextSz * 2];
		SecureZeroMemory(buffer, ftextSz * 2);

		ReadProcessMemory(hProc, (BYTE*)(FTEXTPTR), buffer, ftextSz * 2, NULL);

		std::wstring ws(buffer);
		std::string str(ws.begin(), ws.end());

		delete[] buffer;

		this->itemName = str;

		this->itemRarity = util::Read<INT8>(this->baseAddress + 0x50, sizeof(this->itemRarity));

		this->drawColor = getColorByRarity(this->itemRarity);

		this->itemType = (fortItemType)util::Read<INT8>(this->baseAddress + 0x51, sizeof(this->itemType));



		if (std::strstr(this->itemName.c_str(), "Assault") != nullptr) {

			this->weaponType = WEAPON_AR;

		}
		else if (std::strstr(this->itemName.c_str(), "Shotgun") != nullptr) {

			this->weaponType = WEAPON_SG;

		}
		else if (std::strstr(this->itemName.c_str(), "Submachine") != nullptr || std::strstr(this->itemName.c_str(), "Minigun") != nullptr) {

			this->weaponType = WEAPON_SMG;

		}
		else if (std::strstr(this->itemName.c_str(), "Sniper") != nullptr || std::strstr(this->itemName.c_str(), "Hunting") != nullptr || std::strstr(this->itemName.c_str(), "Infantry") != nullptr) {

			this->weaponType = WEAPON_SNIPER;

		}
		else if (std::strstr(this->itemName.c_str(), "Pistol") != nullptr || std::strstr(this->itemName.c_str(), "Hand Cannon") != nullptr) {

			this->weaponType = WEAPON_PISTOL;

		}

		else {
			this->weaponType = WEAPON_NA;
		}


	}

	FortItem::FortItem(uintptr_t baseAddy, fortItemType itemType) : AActor(baseAddy) {


		this->itemType = itemType;

		if (itemType == World_AmmoBox) {

			this->drawColor = getColorByRarity(6);

			this->itemName = "Ammo Box";


		}
		else if (itemType == World_Chest) {

			this->drawColor = getColorByRarity(5);

			this->itemName = "Chest";


		}


		int searchFlags = util::Read<int>(this->baseAddress + 0xc41, sizeof(searchFlags));

		if (searchFlags & 1 << 5) {

			this->isSearched = true;

		}
		else {
			this->isSearched = false;
		}


	}

	FortItem::FortItem() : AActor(0) {}





