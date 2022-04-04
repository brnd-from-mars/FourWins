//
// Created by Brendan Berg on 10.03.22.
//

#include "SessionModel.hpp"
#include "SessionController.hpp"

#include "AppDelegate.hpp"
#include "ConnectFourUtility.hpp"


SessionModel::SessionModel(int columns, int rows)
    : m_Columns(columns), m_Rows(rows)
{ }


SessionModel::SessionModel(const SessionModel& oldSessionModel)
    : m_Columns(oldSessionModel.m_Columns), m_Rows(oldSessionModel.m_Rows)
{
    // copy color change data
    m_RandomNameForColorPick = oldSessionModel.m_RandomNameForColorPick;
    m_ColorsChanged = oldSessionModel.m_ColorsChanged;

    // skip nameEnter and colorPick SessionState
    m_State = SessionState::inGame;
}


void SessionModel::Update()
{ }


bool SessionModel::HandleInitialNameEnter()
{
    // abort if SessionState is in wrong state
    if (m_State != SessionState::nameEnter)
    {
        return false;
    }

    // abort if controller does not exist
    auto controller = m_SessionController.lock();
    if (!controller)
    {
        return false;
    }

    // abort if not both names are entered
    if (controller->GetName(0).empty() || controller->GetName(1).empty())
    {
        return false;
    }

    // go to next state
    m_State = SessionState::colorPick;
    return true;
}


bool SessionModel::HandleColorPick(int color)
{
    // abort if SessionState is in wrong state
    if (m_State != SessionState::colorPick)
    {
        return false;
    }

    // Check if player index does not match with color chosen -> colors were changed
    m_ColorsChanged = color != m_RandomNameForColorPick;

    // go to next state
    m_State = SessionState::inGame;
    return true;
}


std::string SessionModel::GetRandomPlayerForColorPick()
{
    // abort if SessionState is in wrong state
    if (m_State != SessionState::colorPick)
    {
        return std::string();
    }

    // abort if controller does not exist
    auto controller = m_SessionController.lock();
    if (!controller)
    {
        return std::string();
    }

    // get random player (0 = player in upper text field, 1 = player in lower text field)
    m_RandomNameForColorPick = AppDelegate::Get()->GetRandomNumber() % 2;
    // return name of the chosen random player
    return controller->GetName(m_RandomNameForColorPick);
}


int SessionModel::GetCurrentPlayerIndex() const
{
    // current player is 1 for red and 2 for blue
    // if colors were not changed: index will be 0 for red and 1 for blue
    // if colors were changed: index will be 1 for red and 0 for blue
    // IMPORTANT: current player always refers to the color (red or blue) and not which text field the name is in
    return (m_ColorsChanged ? 3 - m_CurrentPlayer : m_CurrentPlayer) - 1;
}


void SessionModel::AddChip(int column)
{
    // abort if SessionState is in wrong state
    if (m_State != SessionState::inGame)
    {
        return;
    }

    // abort if controller does not exist
    auto controller = m_SessionController.lock();
    if (!controller)
    {
        return;
    }

    // go through each field in column
    for (auto& field : controller->m_Grid[column])
    {
        // check if field does not contain chip
        if (!field->HasChip())
        {
            // add the chip
            field->SetChip(m_CurrentPlayer);

            // increment move count of current player
            ++m_Moves[m_CurrentPlayer - 1];

            // get row number of added chip
            auto row = field->GetRow();

            // compute if a victory or tie have occurred due to the added chip at position column, row
            auto winState = GetWinState(column, row);
            if (winState != PlayerState::none)
            {
                GameData game;
                game.moves = m_Moves[m_CurrentPlayer - 1];

                // if a tie has occurred
                if (winState == PlayerState::tie)
                {
                    // fill in results, order of player names is negligible in a tie
                    game.winningPlayer = controller->GetName(0);
                    game.loosingPlayer = controller->GetName(1);
                }
                else
                {
                    // interpret the RHS statement as !(ColorsChanged XOR (player1 won))
                    int index = m_ColorsChanged == (winState == PlayerState::player1);
                    // fill in results
                    game.winningPlayer = controller->GetName(index);
                    game.loosingPlayer = controller->GetName(1 - index);
                }

                // controller should adapt UI to game end and forward results to the GameController to save them
                controller->HandleGameEnd(winState, game);

                // go to next state
                m_State = SessionState::finished;
            }

            // change current player (player1 -> player2, player2 -> player1)
            m_CurrentPlayer = 3 - m_CurrentPlayer;

            return;
        }
    }
}


PlayerState SessionModel::GetPlayerAt(int column, int row) const
{
    // if cout of bounds -> return none
    if ((column < 0) || (m_Columns <= column) || (row < 0) || (m_Rows <= row))
    {
        return PlayerState::none;
    }

    // abort if controller does not exist
    auto controller = m_SessionController.lock();
    if (!controller)
    {
        return PlayerState::none;
    }

    // return chip color at the coordinates
    switch (controller->GetPlayerAt(column, row))
    {
        case 1:
            return PlayerState::player1;
        case 2:
            return PlayerState::player2;
        default:
            return PlayerState::none;
    }
}



PlayerState SessionModel::GetWinState(int column, int row)
{
    PlayerState winState;

    winState = CheckHorizontal(column, row);
    if (winState != PlayerState::none)
    {
        return winState;
    }

    winState = CheckVertical(column, row);
    if (winState != PlayerState::none)
    {
        return winState;
    }

    winState = CheckDiagonal(column, row);
    if (winState != PlayerState::none)
    {
        return winState;
    }

    winState = CheckFieldFull();
    if (winState != PlayerState::none)
    {
        return winState;
    }

    return PlayerState::none;

}


void SessionModel::CheckChips(int column, int row, PlayerState& prev, int& count)
{
    PlayerState current = GetPlayerAt(column, row); 

    if ((current == PlayerState::player1) || (current == PlayerState::player2))
    {
        if (current == prev)
        {
            ++count;
        }
        else
        {
            count = 1;
        }

        m_WinningChips[count - 1].x = column;   //add location of current Chip to a Vector 
        m_WinningChips[count - 1].y = row;      //vector contains coordinates of the 4 winnning chips
    }
    else
    {
        count = 0;
    }

    prev = current;                             //going to the next Field: current Chip is now the previous
}


PlayerState SessionModel::CheckHorizontal(int column, int row) {

    int count = 0;                              //reset control variables                                            
    PlayerState prev = PlayerState::none;       //

    for (int x = column - 3; x <= column + 3; ++x)
    {
        CheckChips(x, row, prev, count);
        if (count == 4)
        {
            return prev;
        }
    }

    return PlayerState::none;
}


PlayerState SessionModel::CheckVertical(int column, int row) {

    int count = 0;
    PlayerState prev = PlayerState::none; 

    for (int y = row - 3; y <= row; ++y)
    {
        CheckChips(column, y, prev, count);
        if (count == 4)
        {
            return prev;
        }
    }

    return PlayerState::none;
}


PlayerState SessionModel::CheckDiagonal(int column, int row)
{
    int count = 0;                          //reset control variables
    PlayerState prev = PlayerState::none;   // 

	int y = row - 3;
	for (int x = column - 3; x <= column + 3; ++x)
	{
		CheckChips(x, y, prev, count);
        if (count == 4)
        {
            return prev;
        }
		++y;
	}


	count = 0;                               //reset control variables
	prev = PlayerState::none;                //

	y = row + 3;
	for (int x = column - 3; x <= column + 3; ++x)
	{
		CheckChips(x, y, prev, count);
        if (count == 4)
        {
            return prev;
        }
        --y;
	}

	return PlayerState::none;
}


PlayerState SessionModel::CheckFieldFull() const
{
	for (int x = 0; x < m_Columns; ++x)
	{
		if (GetPlayerAt(x, m_Rows - 1) == PlayerState::none)
		{
            return PlayerState::none;
        }
	}

	return PlayerState::tie;
}
