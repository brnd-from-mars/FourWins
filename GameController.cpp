//
// Created by Brendan Berg on 10.03.22.
//

#include <iostream>

#include "GameController.hpp"

#include "AppDelegate.hpp"


std::shared_ptr<GameController> GameController::MakeGameController(int columns, int rows)
{
    auto controller = std::make_shared<GameController>(columns, rows);
    AppDelegate::Get()->RegisterController(controller);

    controller->m_GameModel->m_GameController = controller;
    controller->m_GameController = controller;

    return controller;
}


GameController::GameController(int columns, int rows)
{
    m_GameModel = std::make_shared<GameModel>(columns, rows);
    AppDelegate::Get()->RegisterModel(m_GameModel);

    m_HeadlineCyan = TextView::MakeText(48.0f, 22.0f, 75.0f, "Tron", "C0nnect F0ur", ColorPalette::Cyan, 2);
    m_HeadlineOrange = TextView::MakeText(52.0f, 26.0f, 75.0f, "Tron", "C0nnect F0ur", ColorPalette::Orange, 1);

    std::clog << "GameController constructed" << std::endl;
}


GameController::~GameController()
{
    std::clog << "GameController destructed" << std::endl;
}


void GameController::Update()
{
    // create new session if either no session exists or existing session was finished
    if (!m_SessionController || !m_SessionController->IsOngoing())
    {
        m_SessionController = SessionController::MakeSessionController(m_GameController,
                                                                       m_GameModel->m_Columns,
                                                                       m_GameModel->m_Rows);
    }
}
