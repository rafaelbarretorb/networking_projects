#include "MMO_Common.h"
#include "game_settings.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include <unordered_map>

#include "olcPGEX_TransformedView.h"

class MMOGame : public olc::PixelGameEngine,
                olc::net::ClientInterface<GameMsg> {
 public:
  MMOGame() { sAppName = "MMO Client"; }
  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override;

 private:
  void HandlePanAndZoom();
  void DrawWorld();
  void DrawWorldObjects();
  void UpdateScreen();
  void SendPlayerDescription();
  void ControlOfPlayerObject();
  void UpdateObjectsLocally(float fElapsedTime);
  void UpdateClientState(olc::net::Message<GameMsg>& msg);

  const static std::string sWorldMap;
  const static olc::vi2d vWorldSize;
  olc::TileTransformedView tv;
  std::unordered_map<uint32_t, sPlayerDescription> mapObjects;
  uint32_t nPlayerID = 0;
  sPlayerDescription descPlayer;
  bool bWaitingForConnection = true;
};

const std::string MMOGame::sWorldMap{kWorldMap};
const olc::vi2d MMOGame::vWorldSize{kWorldSize};

int main() {
  MMOGame demo;
  if (demo.Construct(480, 480, 1, 1)) demo.Start();
  return 0;
}

bool MMOGame::OnUserCreate() {
  tv = olc::TileTransformedView({ScreenWidth(), ScreenHeight()}, {8, 8});

  if (Connect("127.0.0.1", 60000)) {
    return true;
  }

  return false;
}

bool MMOGame::OnUserUpdate(float fElapsedTime) {
  // Check for incoming network messages
  if (IsConnected()) {
    while (!Incoming().Empty()) {
      auto msg = Incoming().PopFront().msg;
      UpdateClientState(msg);
    }
  }

  if (bWaitingForConnection) {
    Clear(olc::DARK_BLUE);
    DrawString({10, 10}, "Waiting To Connect...", olc::WHITE);
    return true;
  }

  ControlOfPlayerObject();

  UpdateObjectsLocally(fElapsedTime);

  HandlePanAndZoom();

  UpdateScreen();

  SendPlayerDescription();

  return true;
}

void MMOGame::HandlePanAndZoom() {
  if (GetMouse(2).bPressed) tv.StartPan(GetMousePos());
  if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
  if (GetMouse(2).bReleased) tv.EndPan(GetMousePos());
  if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(1.5f, GetMousePos());
  if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.75f, GetMousePos());
}

void MMOGame::DrawWorld() {
  olc::vi2d vTL = tv.GetTopLeftTile().max({0, 0});
  olc::vi2d vBR = tv.GetBottomRightTile().min(vWorldSize);
  olc::vi2d vTile;
  for (vTile.y = vTL.y; vTile.y < vBR.y; vTile.y++) {
    for (vTile.x = vTL.x; vTile.x < vBR.x; vTile.x++) {
      if (sWorldMap[vTile.y * vWorldSize.x + vTile.x] == '#') {
        tv.DrawRect(vTile, {1.0f, 1.0f});
        tv.DrawRect(olc::vf2d(vTile) + olc::vf2d(0.1f, 0.1f), {0.8f, 0.8f});
      }
    }
  }
}

void MMOGame::DrawWorldObjects() {
  for (auto& object : mapObjects) {
    // Draw Boundary
    tv.DrawCircle(object.second.vPos, object.second.fRadius);

    // Draw direction
    olc::vf2d p1 = object.second.vPos;
    olc::vf2d p2 = { object.second.fRadius * std::cos(object.second.vYaw), object.second.fRadius * std::sin(object.second.vYaw)};
    tv.DrawLine(p1,
                p1 + p2,
                olc::MAGENTA);

    // Draw bullet
    if (object.second.bulletWasShotAndStillExist) {
      tv.DrawCircle(object.second.bulletPos, object.second.bulletRadius, olc::RED);
      tv.FillCircle(object.second.bulletPos, object.second.bulletRadius, olc::RED);
    } else {
      tv.DrawCircle(object.second.vPos, object.second.bulletRadius, olc::RED);
      tv.FillCircle(object.second.vPos, object.second.bulletRadius, olc::RED);
    }

    // Draw Name
    olc::vi2d vNameSize =
        GetTextSizeProp("ID: " + std::to_string(object.first));
    tv.DrawStringPropDecal(
        object.second.vPos - olc::vf2d{vNameSize.x * 0.5f * 0.25f * 0.125f,
                                       -object.second.fRadius * 1.25f},
        "ID: " + std::to_string(object.first), olc::BLUE, {0.25f, 0.25f});
  }
}

void MMOGame::UpdateScreen() {
  Clear(olc::BLACK);
  DrawWorld();
  DrawWorldObjects();
}

void MMOGame::SendPlayerDescription() {
  olc::net::Message<GameMsg> msg;
  msg.header.id = GameMsg::Game_UpdatePlayer;
  msg << mapObjects[nPlayerID];
  Send(msg);
}

void MMOGame::ControlOfPlayerObject() {
  mapObjects[nPlayerID].vVel = {0.0f, 0.0f};
  if (GetKey(olc::Key::W).bHeld) mapObjects[nPlayerID].vVel += {0.0f, -1.0f};
  if (GetKey(olc::Key::S).bHeld) mapObjects[nPlayerID].vVel += {0.0f, +1.0f};
  if (GetKey(olc::Key::A).bHeld) mapObjects[nPlayerID].vVel += {-1.0f, 0.0f};
  if (GetKey(olc::Key::D).bHeld) mapObjects[nPlayerID].vVel += {+1.0f, 0.0f};

  if (GetKey(olc::Key::W).bPressed || GetKey(olc::Key::W).bHeld) mapObjects[nPlayerID].vYaw = -M_PI/2;
  if (GetKey(olc::Key::S).bPressed || GetKey(olc::Key::S).bHeld) mapObjects[nPlayerID].vYaw = M_PI/2;
  if (GetKey(olc::Key::A).bPressed || GetKey(olc::Key::A).bHeld) mapObjects[nPlayerID].vYaw = M_PI;
  if (GetKey(olc::Key::D).bPressed || GetKey(olc::Key::D).bHeld) mapObjects[nPlayerID].vYaw = 0.0;
  if (GetKey(olc::Key::D).bHeld && GetKey(olc::Key::W).bHeld) mapObjects[nPlayerID].vYaw = -M_PI/4;
  if (GetKey(olc::Key::A).bHeld && GetKey(olc::Key::W).bHeld) mapObjects[nPlayerID].vYaw = 5 * M_PI/4;
  if (GetKey(olc::Key::A).bHeld && GetKey(olc::Key::S).bHeld) mapObjects[nPlayerID].vYaw = 3 * M_PI/4;
  if (GetKey(olc::Key::D).bHeld && GetKey(olc::Key::S).bHeld) mapObjects[nPlayerID].vYaw = M_PI/4;

  if (GetKey(olc::Key::SPACE).bPressed && !mapObjects[nPlayerID].bulletWasShotAndStillExist) {
    mapObjects[nPlayerID].bulletVel = { (float)(std::cos(mapObjects[nPlayerID].vYaw)) , (float)(std::sin(mapObjects[nPlayerID].vYaw)) };
    mapObjects[nPlayerID].bulletWasShotAndStillExist = true;
  }
}

void MMOGame::UpdateObjectsLocally(float fElapsedTime) {
  for (auto& object : mapObjects) {
    // Where will object be worst case?
    olc::vf2d vPotentialPosition =
        object.second.vPos + object.second.vVel * fElapsedTime;

    olc::vf2d vBulletPotentialPosition =
        object.second.bulletPos + object.second.bulletVel * fElapsedTime;

    // Extract region of world cells that could have collision this frame
    olc::vi2d vCurrentCell = object.second.vPos.floor();
    olc::vi2d vTargetCell = vPotentialPosition;

    olc::vi2d vBulletCurrentCell = object.second.bulletPos.floor();
    olc::vi2d vBulletTargetCell = vBulletPotentialPosition;

    olc::vi2d vAreaTL =
        (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({0, 0});
    olc::vi2d vAreaBR =
        (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);


    olc::vi2d vBulletAreaTL =
    (vBulletCurrentCell.min(vBulletTargetCell) - olc::vi2d(1, 1)).max({0, 0});

    olc::vi2d vBulletAreaBR =
    (vBulletCurrentCell.max(vBulletTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);

    // Iterate through each cell in test area
    olc::vi2d vCell;
    for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++) {
      for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++) {
        // Check if the cell is actually solid...
        //	olc::vf2d vCellMiddle = vCell.floor();
        if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#') {
          // ...it is! So work out nearest point to future player position,
          // around perimeter of cell rectangle. We can test the distance to
          // this point to see if we have collided.

          olc::vf2d vNearestPoint;
          // Inspired by this (very clever btw)
          // https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
          vNearestPoint.x =
              std::max(float(vCell.x),
                       std::min(vPotentialPosition.x, float(vCell.x + 1)));
          vNearestPoint.y =
              std::max(float(vCell.y),
                       std::min(vPotentialPosition.y, float(vCell.y + 1)));

          // But modified to work :P
          olc::vf2d vRayToNearest = vNearestPoint - vPotentialPosition;
          float fOverlap = object.second.fRadius - vRayToNearest.mag();
          if (std::isnan(fOverlap)) fOverlap = 0;  // Thanks Dandistine!

          // If overlap is positive, then a collision has occurred, so we
          // displace backwards by the overlap amount. The potential position
          // is then tested against other tiles in the area therefore
          // "statically" resolving the collision
          if (fOverlap > 0) {
            // Statically resolve the collision
            vPotentialPosition =
                vPotentialPosition - vRayToNearest.norm() * fOverlap;
          }
        }
      }
    }

    // Set the objects new position to the allowed potential position
    object.second.vPos = vPotentialPosition;


    // Bullet
    for (vCell.y = vBulletAreaTL.y; vCell.y <= vBulletAreaBR.y; vCell.y++) {
      for (vCell.x = vBulletAreaTL.x; vCell.x <= vBulletAreaBR.x; vCell.x++) {

        if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#') {
          object.second.bulletWasShotAndStillExist = false;
          object.second.bulletPos = vPotentialPosition;
          return;
        }
      }
    }

    object.second.bulletPos = vBulletPotentialPosition;
  }
}

void MMOGame::UpdateClientState(olc::net::Message<GameMsg>& msg) {
  switch (msg.header.id) {
    case (GameMsg::Client_Accepted): {
      std::cout << "Server accepted client - you're in!\n";
      olc::net::Message<GameMsg> msg;
      msg.header.id = GameMsg::Client_RegisterWithServer;
      descPlayer.vPos = {3.0f, 3.0f};
      descPlayer.bulletPos = {3.0f, 3.0f};
      descPlayer.vYaw = 0.0;
      msg << descPlayer;
      Send(msg);
      break;
    }

    case (GameMsg::Client_AssignID): {
      // Server is assigning us OUR id
      msg >> nPlayerID;
      std::cout << "Assigned Client ID = " << nPlayerID << "\n";
      break;
    }

    case (GameMsg::Game_AddPlayer): {
      sPlayerDescription desc;
      msg >> desc;
      mapObjects.insert_or_assign(desc.nUniqueID, desc);

      if (desc.nUniqueID == nPlayerID) {
        // Now we exist in game world
        bWaitingForConnection = false;
      }
      break;
    }

    case (GameMsg::Game_RemovePlayer): {
      uint32_t nRemovalID = 0;
      msg >> nRemovalID;
      mapObjects.erase(nRemovalID);
      break;
    }

    case (GameMsg::Game_UpdatePlayer): {
      sPlayerDescription desc;
      msg >> desc;
      mapObjects.insert_or_assign(desc.nUniqueID, desc);
      break;
    }
  }
}
