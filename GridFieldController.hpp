//
// Created by Brendan Berg on 10.03.22.
//

#pragma once

#include <memory>

#include "BaseController.hpp"

#include "ChipController.hpp"

class GridFieldModel;
class GridFieldView;

class SessionController;


class GridFieldController : public BaseController
{
    friend GridFieldModel;
    friend GridFieldView;


public:

    static std::shared_ptr<GridFieldController> MakeGridField(const std::weak_ptr<SessionController>& session,
                                                              int column, int row);

    GridFieldController(const std::weak_ptr<SessionController>& session, int column, int row);

    void Update() override;


    void SetChip(int player);
    void SetHighlightChip(bool highlight);

    bool HasChip() const;
    int GetPlayer() const;
    int GetColumn() const;
    int GetRow() const;


private:

    void HandleClick();

    std::shared_ptr<GridFieldModel> m_GridFieldModel;
    std::shared_ptr<GridFieldView> m_GridFieldView;

    std::weak_ptr<SessionController> m_Session;

    std::shared_ptr<ChipController> m_Chip;


};
