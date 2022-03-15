//
// Created by Brendan Berg on 15.03.22.
//

#pragma once

#include "vendor/tinyxml2/tinyxml2.h"

#include "BaseModel.hpp"

#include "ConnectFourUtility.hpp"
#include "LinkedList.hpp"

class ScoreBoardController;


class ScoreBoardModel : public BaseModel
{
    friend ScoreBoardController;


public:

    ScoreBoardModel();

    void Update() override;

    void AddGame(GameData data);


private:

    void SaveXMLDocument();

    static void AddGameToXMLDocument(GameData* game, tinyxml2::XMLElement* nGames, tinyxml2::XMLDocument* file);
    static void AddPlayerToXMLDocument(PlayerData* player, tinyxml2::XMLElement* nPlayers, tinyxml2::XMLDocument* file);

    LinkedList<GameData> m_GameList;
    LinkedList<PlayerData> m_PlayerList;


};
