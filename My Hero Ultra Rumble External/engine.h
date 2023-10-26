#pragma once
#include "precomp.h"







bool updateWorld();

std::string GetNameFromFName(int key);



bool updateLocalPlayer();

bool updateCameraCache();


AimTab currWeaponType();

bool refreshPawns();


void drawSkeleton(APlayerStateBattle player);


void drawBoneIDs(APlayerStateBattle player);


APlayerStateBattle findPlayerByID(int32_t ID);
ImColor getColorByRarity(INT8 rarity);


void drawDebugAActors();


void cacheLevel();



void drawWorldItems();


void projectileTP();


void mainLoop();

















