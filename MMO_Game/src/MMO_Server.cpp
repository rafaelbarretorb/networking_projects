#include "MMO_Common.h"

#include <iostream>
#include <unordered_map>

class GameServer : public olc::net::ServerInterface<GameMsg> {
 public:
  GameServer(uint16_t nPort) : olc::net::ServerInterface<GameMsg>(nPort) {}

  std::unordered_map<uint32_t, sPlayerDescription> m_mapPlayerRoster;
  std::vector<uint32_t> m_vGarbageIDs;

 protected:
  bool OnClientConnect(
      std::shared_ptr<olc::net::Connection<GameMsg>> client) override {
    // For now we will allow all
    return true;
  }

  void OnClientValidated(
      std::shared_ptr<olc::net::Connection<GameMsg>> client) override {
    // Client passed validation check, so send them a message informing
    // them they can continue to communicate
    olc::net::Message<GameMsg> msg;
    msg.header.id = GameMsg::Client_Accepted;
    client->Send(msg);
  }

  void OnClientDisconnect(
      std::shared_ptr<olc::net::Connection<GameMsg>> client) override {
    if (client) {
      if (m_mapPlayerRoster.find(client->GetID()) == m_mapPlayerRoster.end()) {
        // client never added to roster, so just let it disappear
      } else {
        auto& pd = m_mapPlayerRoster[client->GetID()];
        std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.nUniqueID) +
                         "\n";
        m_mapPlayerRoster.erase(client->GetID());
        m_vGarbageIDs.push_back(client->GetID());
      }
    }
  }

  void OnMessage(std::shared_ptr<olc::net::Connection<GameMsg>> client,
                 olc::net::Message<GameMsg>& msg) override {
    if (!m_vGarbageIDs.empty()) {
      for (auto pid : m_vGarbageIDs) {
        olc::net::Message<GameMsg> m;
        m.header.id = GameMsg::Game_RemovePlayer;
        m << pid;
        std::cout << "Removing " << pid << "\n";
        MessageAllClients(m);
      }
      m_vGarbageIDs.clear();
    }

    switch (msg.header.id) {
      case GameMsg::Client_RegisterWithServer: {
        sPlayerDescription desc;
        msg >> desc;
        desc.nUniqueID = client->GetID();
        m_mapPlayerRoster.insert_or_assign(desc.nUniqueID, desc);

        olc::net::Message<GameMsg> msgSendID;
        msgSendID.header.id = GameMsg::Client_AssignID;
        msgSendID << desc.nUniqueID;
        MessageClient(client, msgSendID);

        olc::net::Message<GameMsg> msgAddPlayer;
        msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
        msgAddPlayer << desc;
        MessageAllClients(msgAddPlayer);

        for (const auto& player : m_mapPlayerRoster) {
          olc::net::Message<GameMsg> msgAddOtherPlayers;
          msgAddOtherPlayers.header.id = GameMsg::Game_AddPlayer;
          msgAddOtherPlayers << player.second;
          MessageClient(client, msgAddOtherPlayers);
        }

        break;
      }

      case GameMsg::Client_UnregisterWithServer: {
        break;
      }

      case GameMsg::Game_UpdatePlayer: {
        // Simply bounce update to everyone except incoming client
        MessageAllClients(msg, client);
        break;
      }
    }
  }
};

int main() {
  GameServer server(60000);
  server.Start();

  while (1) {
    server.Update(-1, true);
  }
  return 0;
}