//
// Created by Brendan Berg on 15.03.22.
//

#pragma once

#include <memory>

#include "BaseController.hpp"

#include "ConnectFourUtility.hpp"

class ScoreBoardModel;
class ScoreBoardView;


class ScoreBoardController : public BaseController
{
    friend ScoreBoardView;


public:

    static std::shared_ptr<ScoreBoardController>MakeScoreBoard(float x, float y, float width);

    ScoreBoardController(float x, float y, float width);

    void Update() override;

    void AddGame(GameData game);


private:

    std::shared_ptr<ScoreBoardModel> m_ScoreBoardModel;
    std::shared_ptr<ScoreBoardView> m_ScoreBoardView;


};
