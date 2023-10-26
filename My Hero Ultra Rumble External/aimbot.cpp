#include "aimbot.h"

bool bAimActive;

void aimbot::aimbot(int vKey, float smoothX, float smoothY, Vector3 BoneAimLoc) {


   


/*

    if (GetAsyncKeyState(vKey)) {

        float currValY = CameraCache.POV.Rotation.y;

        float currValX = CameraCache.POV.Rotation.x;
        float atanYX;
        float asinXY;


        ImGui::Text("currX %f currY %f", currValX, currValY);



        atanYX = atan2(BoneAimLoc.y - CameraCache.POV.Location.y, BoneAimLoc.x - CameraCache.POV.Location.x);
        asinXY = asin((BoneAimLoc.z - CameraCache.POV.Location.z) / util::getDistance(CameraCache.POV.Location, BoneAimLoc));




        float yChange = rad2Deg(atanYX);
        float xChange = rad2Deg(asinXY);



        float bufferY = yChange - currValY;

        float bufferX = xChange - currValX;
        ImGui::Text("xChange %f YChange %f", xChange, yChange);
        ImGui::Text("bufferX %f bufferY %f", bufferX, bufferY);


        if ((bufferY) >= 180.f) {

            bufferY = abs(bufferY) - 360.f;

        }
        else if (bufferY <= -180.f) {

            bufferY = 360.f - abs(bufferY);

        }

        if (abs(bufferX) >= 250) {

            bufferX = 360 - abs(bufferX);
        }
        bAimActive = true;

    

        //RotationInput
        util::Write<float>(localPlayerController + 0x420, bufferX / smoothX);
        util::Write<float>(localPlayerController + 0x424, bufferY / smoothY);
    }
    else {

        bAimActive = false;
    }
    */
}

APlayerStateBattle aimbot::getAimTarget() {

    float distBuffer = 999999999.f;
    int currTarget = -1;

    for (int i = 0; i < enemyPawns.size(); i++) {


        if (!enemyPawns[i].playerPawn.skeleton[0][0].isZero()) {

            float distFromCrossHair = util::get2DDistance(enemyPawns[i].playerPawn.getW2S(), Vector3(winProperties.width / 2 + winProperties.x, winProperties.height / 2 + winProperties.y, 0));

            if (currAimSetting.bUseFov) {

                if (distFromCrossHair > currAimSetting.fovRadius) {

                    continue;
                }

            }


            if (aimPriority == 0)
            {

                //Distance from screen center

                if (distFromCrossHair < distBuffer) {

                    distBuffer = distFromCrossHair;
                    currTarget = i;

                }


            }
            else if (aimPriority == 1)
            {

                //3D distance in world

                if (enemyPawns[i].playerPawn.Distance < distBuffer) {

                    distBuffer = enemyPawns[i].playerPawn.Distance;
                    currTarget = i;

                }
            }
        }

    }

        if (currTarget < enemyPawns.size() && currTarget >= 0) {
            return enemyPawns[currTarget];
        }
        return APlayerStateBattle();
    

}

