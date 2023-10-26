#include "engine.h"

uintptr_t UWorld;
uintptr_t Levels;
INT32 levelCount;
uintptr_t persistentLevel;
uintptr_t GameInstance;
uintptr_t GameState;
uintptr_t localPlayers;
uintptr_t localPlayer;
uintptr_t localPlayerController;
uintptr_t localPlayerPiece;
int localPlayerID;

std::vector<APlayerStateBattle> targetPawns;
std::vector<APlayerStateBattle> enemyPawns;

std::vector<FortItem> worldItems;

uintptr_t playerArray;
 
AimSettings currAimSetting;


APlayerStateBattle localPlayerState;
int aimTarg = -1;
APlayerStateBattle aimPiece;


ImVec2 boxDimensions;

uintptr_t playerCamManager;
CameraCacheEntry CameraCache;

clock_t worldUpdateTimer = -4000;
clock_t localPlayerUpdateTimer = -4000;
clock_t pieceBaseUpdateTimer = -4000;
clock_t pieceUpdateTimer = -2000;
clock_t levelUpdateTimer = -4000;


std::string GetNameFromFName(int key)
{
    unsigned int chunkOffset = (unsigned int)((int)(key) >> 16);
    unsigned short nameOffset = (unsigned short)key;

    UINT64 namePoolChunk = util::Read<UINT64>(GNames + ((chunkOffset + 2) * 8), sizeof(UINT64));
    UINT64 entryOffset = namePoolChunk + (unsigned long)(2 * nameOffset);

    INT16 nameEntry = util::Read<INT16>(entryOffset, sizeof(INT16));

    UINT16 nameLength = nameEntry >> 6;

    char* buf = new char[nameLength];
    SecureZeroMemory(buf, nameLength);

    ReadProcessMemory(hProc, (BYTE*)(entryOffset + 2), buf, nameLength, NULL);

    std::string name = buf;
    delete[] buf;
    name = name.substr(0, nameLength);
    return name;
}

/*
std::vector<AActor> getAActors(uintptr_t levelBaseAddress) {

    uintptr_t pAActors = util::Read<uintptr_t>((levelBaseAddress + 0x98), sizeof(pAActors));

    int actorCount = util::Read<int>((levelBaseAddress + 0xA0), sizeof(actorCount));

    std::vector<AActor> AActors(actorCount);

    for (int i = 0; i < actorCount; i++) {

        AActors[i] = AActor(util::Read<uintptr_t>((pAActors + (8 * i)), sizeof(uintptr_t)));
    
        AActors[i].getPos();
    }

    return AActors;

}
*/
bool updateWorld() {


    uintptr_t buffer = UWorld;

    clock_t tempTime = clock();
    if (tempTime >= worldUpdateTimer + 4000) {

        UWorld = util::Read<uintptr_t>(GWorld + modBase, sizeof(UWorld));


       

            Levels = util::Read<uintptr_t>(UWorld + offsets::UWorld::Levels, sizeof(Levels));

            levelCount = util::Read<INT32>(UWorld + offsets::UWorld::Levels+8, sizeof(levelCount));

            persistentLevel = util::Read<uintptr_t>((UWorld + offsets::UWorld::PersistentLevel), sizeof(persistentLevel));

            GameInstance = util::Read<uintptr_t>((UWorld + offsets::UWorld::OwningGameInstance), sizeof(GameInstance));

            GameState = util::Read<uintptr_t>((UWorld + offsets::UWorld::GameState), sizeof(GameState));

            localPlayers = util::Read<uintptr_t>((GameInstance + 0x38), sizeof(localPlayers));

            localPlayer = util::Read<uintptr_t>(localPlayers, sizeof(localPlayer));



            worldUpdateTimer = tempTime;

            return true;

       



    }
    return false;
}



bool updateLocalPlayer() {


    clock_t tempTime = clock();


    if (tempTime >= localPlayerUpdateTimer + 3000) {

     
        
        localPlayerController = util::Read<uintptr_t>(localPlayer + 0x30, sizeof(localPlayerController));

        localPlayerState = APlayerStateBattle(util::Read<uintptr_t>((localPlayerController + 0x228), sizeof(uintptr_t)));

        localPlayerUpdateTimer = tempTime;
        return true;
    }
    return false;

}




bool updateCameraCache() {

    playerCamManager = util::Read<uintptr_t>(localPlayerController + 0x2B8, sizeof(playerCamManager));

    uintptr_t cameraCachePrivate = playerCamManager + 0x1AE0;

    CameraCache.POV = util::Read<FMinimalViewInfo>(cameraCachePrivate + 0x10, sizeof(FMinimalViewInfo));

    return true;

}

/*
Vector3 WorldToScreen(Vector3 WorldLocation, FMinimalViewInfo camInfo)
{
    Vector3 Screenlocation = Vector3(0, 0, 0);

    auto POV = camInfo;

    Vector3 Rotation = POV.Rotation; // FRotator

    D3DMATRIX tempMatrix = util::Matrix(Rotation); // Matrix

    Vector3 vAxisX, vAxisY, vAxisZ;


    vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
    vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
    vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

    Vector3 vDelta = WorldLocation - POV.Location;
    Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

    if (vTransformed.z < 1.f)
        vTransformed.z = 1.f;

    float FovAngle = POV.FOV;

    float ScreenCenterX = (winProperties.width / 2.0f);
    float ScreenCenterY = (winProperties.height / 2.0f);

    if (camInfo.Rotation.y > -90.f && camInfo.Rotation.y < 90.f) {
        Screenlocation.x = (ScreenCenterX - vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795 / 360.f)) / vTransformed.z) + winProperties.x;
        Screenlocation.y = (ScreenCenterY + vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795 / 360.f)) / vTransformed.z) + winProperties.y;
    }
    else {

        Screenlocation.x = (ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795 / 360.f)) / vTransformed.z) + winProperties.x;
        Screenlocation.y = (ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)3.1415926535897932384626433832795 / 360.f)) / vTransformed.z) + winProperties.y;
    }
    return Screenlocation;
}
*/

AimTab currWeaponType() {

    uintptr_t currWeapon = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + 0x700, sizeof(currWeapon));

    std::string currWeaponName = GetNameFromFName(util::Read<int>(currWeapon + 0x18, sizeof(int)));




    if (std::strstr(currWeaponName.c_str(), "Assault") != nullptr) {

        return AR;

    }
    else if (std::strstr(currWeaponName.c_str(), "Shotgun") != nullptr) {

        return SG;

    }
    else if (std::strstr(currWeaponName.c_str(), "Pistol_PDW") != nullptr || std::strstr(currWeaponName.c_str(), "Pistol_Auto") != nullptr || std::strstr(currWeaponName.c_str(), "Minigun") != nullptr) {

        return SMG;

    }
    else if (std::strstr(currWeaponName.c_str(), "Sniper") != nullptr || std::strstr(currWeaponName.c_str(), "Rifle_NoScope") != nullptr) {

        return SNIPER;

    }
    else if (std::strstr(currWeaponName.c_str(), "Pistol") != nullptr) {

        return PISTOL;

    }
    else {
        return NOAIM;
    }





}

bool refreshPawns() {
    
    clock_t tempTime = clock();
    if (tempTime >= pieceUpdateTimer + 4000) {

        SecureZeroMemory(&targetPawns, sizeof(targetPawns));
        SecureZeroMemory(&enemyPawns, sizeof(enemyPawns));

        unsigned int playerSize = util::Read<unsigned int>(GameState + 0x240, sizeof(playerSize));
        playerArray = util::Read<uintptr_t>(GameState + 0x238, sizeof(playerArray));

        targetPawns = std::vector<APlayerStateBattle>();


        for (int i = 0; i < playerSize; i++) {

            APlayerStateBattle tempPlayerState = APlayerStateBattle(util::Read<uintptr_t>(playerArray + (i * 0x8), sizeof(uintptr_t)));
          
        
            tempPlayerState.playerPawn.refreshSkeleton();

            if (tempPlayerState.isValid()) {

                targetPawns.push_back(tempPlayerState);
            }

        }


        enemyPawns.clear();
        enemyPawns.shrink_to_fit();
        for (int id = 0; id < targetPawns.size(); id++) {
            if (!targetPawns[id].playerPawn.skeleton[0][0].isZero()) {

                if (targetPawns[id].baseAddress != localPlayerState.baseAddress) {


                    enemyPawns.push_back(targetPawns[id]);


                }
                else {

                    localPlayerState = targetPawns[id];
                }

           }
        }

        pieceUpdateTimer = tempTime;
        return true;
    }




    else {


        for (int i = 0; i < targetPawns.size(); i++) {


      

            targetPawns[i].playerPawn.refreshSkeleton();

            targetPawns[i].playerPawn.getDistance();

            targetPawns[i].playerPawn.getW2S();

            if (!targetPawns[i].isValid()) {



                    if (i >= 0 && i < targetPawns.size()) {
                        targetPawns.erase(targetPawns.begin() + i);
                    }


                
            }



        }



        enemyPawns.clear();
        enemyPawns.shrink_to_fit();
        for (int id = 0; id < targetPawns.size(); id++) {

            if (!targetPawns[id].playerPawn.skeleton[0][0].isZero()) {


          
            if (targetPawns[id].playerPawn.playerID != localPlayerState.playerPawn.playerID) {


                enemyPawns.push_back(targetPawns[id]);


            }
            else {

                localPlayerState = targetPawns[id];
            }
        }

        }

    }

    return false;

}





void drawSkeleton(APlayerStateBattle player) {


    BoneInfo previous(0, 0, 0, 0);
    BoneInfo current(0, 0, 0, 0);
    Vector3 p1, c1;
    for (int boneSet = 0; boneSet < player.playerPawn.skeleton.size(); boneSet++)
    {
        previous = BoneInfo(0, 0, 0, 0);
        for (int bone = 0; bone < player.playerPawn.skeleton[boneSet].size(); bone++)
        {
            current = player.playerPawn.skeleton[boneSet][bone];
            if (previous.x == 0.f)
            {
                previous = current;
                continue;
            }

            overlay::drawLine(Vector3(previous.W2S.x, previous.W2S.y, 0), Vector3(current.W2S.x, current.W2S.y, 0));
            previous = current;
        }
    }


}


void drawBoneIDs(APlayerStateBattle player) {

    INT32 boneArrSize = player.playerPawn.boneArrayCount;
    for (int i = 0; i < boneArrSize; i++) {

        FTransform bone = player.playerPawn.GetBoneIndex(i);

        FTransform ComponentToWorld = util::Read<FTransform>(player.playerPawn.meshAddress + 0x1c0, sizeof(FTransform));

        D3DMATRIX Matrix;
        Matrix = util::MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
        Vector3 boneWorld = Vector3(Matrix._41, Matrix._42, Matrix._43);

        overlay::drawText(util::WorldToScreen(boneWorld, CameraCache), 0.f, std::to_string(i).c_str());

    }


}


APlayerStateBattle findPlayerByID(int32_t ID) {

    for (int i = 0; i < enemyPawns.size(); i++) {

        if (enemyPawns[i].playerPawn.playerID == ID) {
            return enemyPawns[i];
        }



    }
    SecureZeroMemory(&aimPiece, sizeof(aimPiece));
    return aimPiece;


}

ImColor getColorByRarity(INT8 rarity) {



    switch (rarity) {

    case(0): {

        return { 0.741f, 0.729f, 0.729f, 1.f };

    }
    case(1): {

        return { 0.039f, 0.769f, 0.f ,1.f };
    }
    case(2): {

        return { 0.02f, 0.529f, 0.769f ,1.f };
    }

    case(3): {

        return { 0.855f, 0.f, 0.922f , 1.f };
    }
    case(4): {

        return { 0.922f, 0.529f, 0.f , 1.f };
    }

           //chest
    case(5): {


        return{ 0.922f, 0.847f, 0.f , 1.f };
    }

           //ammoBox
    case(6): {

        return { 0.553f, 0.961f, 0.553f ,1.f };
    }


    }

}


void drawDebugAActors() {

    uintptr_t levelArray = util::Read<uintptr_t>(UWorld + 0x138, sizeof(levelArray));
    INT32 levelCount = util::Read<INT32>(UWorld + 0x138 + 0x8, sizeof(levelCount));

    for (int level = 0; level < levelCount; level++) {

        uintptr_t levelBase = util::Read<uintptr_t>(levelArray + 0x8 * level, sizeof(levelBase));

        uintptr_t  actorCluster = util::Read<uintptr_t>(levelBase + 0x98, sizeof(actorCluster));

        INT32 actorClusterSize = util::Read<INT32>(levelBase + 0x98 + 0x8, sizeof(actorClusterSize));

        for (int i = 0; i < actorClusterSize; i++) {



            AActor temp(util::Read<uintptr_t>(actorCluster + 0x8 * i, sizeof(uintptr_t)));
            temp.getDistance();
            temp.getW2S();
          
            overlay::drawText(temp.W2S, 0.f, temp.Name.c_str());

            char buffer[50];
            sprintf_s(buffer, "Base Addy:  0x%llX", temp.baseAddress);
            overlay::drawText(temp.W2S, 25.f, buffer);


        }

    }
}


void cacheLevel() {

    clock_t tempTime = clock();
    if (tempTime >= levelUpdateTimer + 10000) {

        worldItems.clear();
        worldItems.shrink_to_fit();

        uintptr_t levelArray = util::Read<uintptr_t>(UWorld + 0x138, sizeof(levelArray));
        INT32 levelCount = util::Read<INT32>(UWorld + 0x138 + 0x8, sizeof(levelCount));

        for (int level = 0; level < levelCount; level++) {

            uintptr_t levelBase = util::Read<uintptr_t>(levelArray + 0x8 * level, sizeof(levelBase));

            uintptr_t  actorCluster = util::Read<uintptr_t>(levelBase + 0x98, sizeof(actorCluster));

            INT32 actorClusterSize = util::Read<INT32>(levelBase + 0x98 + 0x8, sizeof(actorClusterSize));

            for (int i = 0; i < actorClusterSize; i++) {


                AActor tempActor = AActor(util::Read<uintptr_t>(actorCluster + 0x8 * i, sizeof(uintptr_t)));

      
                if (!strcmp("FortPickupAthena", tempActor.Name.c_str())) {
                   
                   FortItem bufferItem(util::Read<uintptr_t>(tempActor.baseAddress + 0x288, sizeof(uintptr_t)), tempActor.getPos());

                    worldItems.push_back(bufferItem);
                    continue;

                }
                else if (std::strstr(tempActor.Name.c_str(), "Tiered_Short_Ammo") != nullptr) {

                    FortItem bufferItem(tempActor.baseAddress, World_AmmoBox);

                    if (!bufferItem.isSearched) {


                        worldItems.push_back(bufferItem);

                    }
                    continue;
                }
                else if (std::strstr(tempActor.Name.c_str(), "Tiered_Chest") != nullptr) {

                   
                    FortItem bufferItem(tempActor.baseAddress, World_Chest);

                    if (!bufferItem.isSearched) {


                        worldItems.push_back(bufferItem);

                    }

                    continue;

                }


            }
        }
        levelUpdateTimer = tempTime;
    }
   

    for (int i = 0; i < worldItems.size(); i++) {

      
        worldItems[i].getW2S(); 
        worldItems[i].getDistance();

     
        
    }



}



void drawWorldItems() {


    if (itemESP::enabled) {


        for (int i = 0; i < worldItems.size(); i++) {

            if (worldItems[i].Distance/100.f < itemESP::maxDistance) {

                if (itemESP::chests) {

                    if (worldItems[i].itemType == World_Chest) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                    }

                }

                if (itemESP::ammoBox) {

                    if (worldItems[i].itemType == World_AmmoBox) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                    }

                }

                if (itemESP::ammo) {

                    if (worldItems[i].itemType == Item_AMMO) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());

                    }


                }
                if (itemESP::consumable) {

                    if (worldItems[i].itemType == Item_Consumable) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                    }


                }
                if (itemESP::traps) {


                    if (worldItems[i].itemType == Item_Trap) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                    }

                }
                if (itemESP::materials) {

                    if (worldItems[i].itemType == Item_Material) {

                        overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                    }

                }

                if (itemESP::weaponESP::enabled) {

                    if (itemESP::weaponESP::AR) {

                        if (worldItems[i].weaponType == WEAPON_AR) {

                            overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());



                        }

                    }
                    if (itemESP::weaponESP::SG) {

                        if (worldItems[i].weaponType == WEAPON_SG) {


                            overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                        }

                    }
                    if (itemESP::weaponESP::SMG) {

                        if (worldItems[i].weaponType == WEAPON_SMG) {


                            overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                        }

                    }
                    if (itemESP::weaponESP::Sniper) {

                        if (worldItems[i].weaponType == WEAPON_SNIPER) {


                            overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());


                        }

                    }
                    if (itemESP::weaponESP::Pistol) {

                        if (worldItems[i].weaponType == WEAPON_PISTOL) {

                            overlay::drawText(worldItems[i].W2S, 0.f, worldItems[i].drawColor, worldItems[i].itemName.c_str());



                        }

                    }




                }


            }

        }




    }




}


void projectileTP() {


    if (GetAsyncKeyState(VK_CAPITAL) & 1) {



        uintptr_t test = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + offsets::AActor::RootComponent, sizeof(test));

        util::Write<Vector3>(test + 0x1d0, Vector3(aimPiece.playerPawn.skeleton[0][2].x, aimPiece.playerPawn.skeleton[0][2].y, aimPiece.playerPawn.skeleton[0][2].z));
    }
    INT32 bulletCount = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + 0x868, sizeof(bulletCount));

    if (bulletCount) {


        uintptr_t currBullets = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + 0x860, sizeof(currBullets));


        

        for (int i = 0; i < bulletCount; i++) {

            uintptr_t projBase = util::Read<uintptr_t>(currBullets + 0x8 * i, sizeof(projBase));

            int projFnameID = util::Read<int>(projBase + 0x18, sizeof(projFnameID));

            std::string projName = GetNameFromFName(projFnameID);

           

                uintptr_t weaponRootComp = util::Read<uintptr_t>(projBase + offsets::AActor::RootComponent, sizeof(weaponRootComp));



                util::Write<Vector3>(weaponRootComp + 0x1d0, Vector3(aimPiece.playerPawn.skeleton[0][2].x, aimPiece.playerPawn.skeleton[0][2].y, aimPiece.playerPawn.skeleton[0][2].z));

            


        }

    }
        



}


void mainLoop() {




    util::updateWindow();




    if (updateWorld()) {


    }



    if (UWorld && GameState) {


        updateCameraCache();

        if (updateLocalPlayer()) {

        }


      if (refreshPawns()) {


       }

        if (enemyPawns.size()) {



           
                aimPiece = aimbot::getAimTarget();


            

        
        }
        else {
            SecureZeroMemory(&aimPiece, sizeof(aimPiece));

        }



       currAimSetting = aimSettings[0];

      //  drawDebugAActors();

        if (itemESP::enabled) {

            cacheLevel();
            drawWorldItems();

        }

     
        //drawBoneIDs(localPlayerState);
     }

  

    std::vector<APlayerStateBattle> pawnsToLoop;

    currAimSetting.fovRadius = abs((currAimSetting.fov / CameraCache.POV.FOV) * winProperties.height * (winProperties.height / winProperties.width));


    if (bDrawFov) {

        ImGui::GetBackgroundDrawList()->AddCircle({ winProperties.width / 2 + winProperties.x, winProperties.height / 2 + winProperties.y }, currAimSetting.fovRadius, ImColor(1.f, 0.f, 0.f, 1.f), 100, 2.f);

    }

    if (localESP) {

        pawnsToLoop = targetPawns;

    }
    else {

        pawnsToLoop = enemyPawns;
    }

    
    if (bulletTP) {
        overlay::drawLine(aimPiece.playerPawn.W2S);

        projectileTP();

    }

    if (giantMode) {

        uintptr_t capSule = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + 0x290, sizeof(capSule));

        util::Write<float>(capSule + 0x134, GiantScale);
        util::Write<float>(capSule + 0x138, GiantScale);
        util::Write<float>(capSule + 0x13c, GiantScale);



    }
    else {

        uintptr_t capSule = util::Read<uintptr_t>(localPlayerState.playerPawn.baseAddress + 0x290, sizeof(capSule));

        util::Write<float>(capSule + 0x134, 1.f);
        util::Write<float>(capSule + 0x138, 1.f);
        util::Write<float>(capSule + 0x13c, 1.f);

    }

    if (bSpeedHack) {

        util::Write<float>(localPlayerState.playerPawn.baseAddress + 0x98, speedValue);


    }
    else {

        util::Write<float>(localPlayerState.playerPawn.baseAddress + 0x98, 1.f);

    }


    for (int i = 0; i < pawnsToLoop.size(); i++) {




        

        if (pawnsToLoop[i].playerPawn.Distance > 0.f ) {




            boxDimensions = { (60 / (pawnsToLoop[i].playerPawn.Distance / 1000.f)) * (winProperties.width / 1920) , (110 / (pawnsToLoop[i].playerPawn.Distance / 1000.f)) * (winProperties.height / 1080) };

            if (boxESP) {

                if (bRounded) {
                    overlay::drawBox(pawnsToLoop[i].playerPawn.W2S, rounding);

                }
                else {

                    overlay::drawBox(pawnsToLoop[i].playerPawn.W2S);


                }

            }

            if (lineESP) {


                overlay::drawLine(pawnsToLoop[i].playerPawn.W2S);

            }

            if (skeletonESP) {

                drawSkeleton(pawnsToLoop[i]);

            }

            if (distanceESP) {

                char distText[50];
                sprintf_s(distText, "%i Meters", ((int)pawnsToLoop[i].playerPawn.Distance / 100));
                overlay::drawText(pawnsToLoop[i].playerPawn.W2S, boxDimensions.y + 15.f, distText);
            }
            if (playerNameESP) {


                overlay::drawText(pawnsToLoop[i].playerPawn.W2S, -boxDimensions.y - 10.f, pawnsToLoop[i].playerName.c_str());
            }



        }

    }
    
    if (drawTPS) {

        char TPSBuffer[50];
        sprintf_s(TPSBuffer, "TPS: %i", TPS);
        overlay::drawText(Vector3(winProperties.x + 40.f, winProperties.y + 20.f, 0), 0.f, TPSBuffer);

        char TotalBuf[50];
        sprintf_s(TotalBuf, "Average TPS %i", totalTPS / totalTime);
        overlay::drawText(Vector3(winProperties.x + 65.f, winProperties.y + 35.f, 0), 0.f, TotalBuf);

    }

   


}

















