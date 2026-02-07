#pragma once
#include "Game.h"
#include <vector>

class ConnectFour : public Game
{
public:
    ConnectFour();
    ~ConnectFour();

    void        setUpBoard() override;
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;

    void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }

private:
    static const int kColumns = 7;
    static const int kRows = 6;
    static const int kRedPlayer = 0;
    static const int kYellowPlayer = 1;

    Bit*        createPiece(Player* player);
    Player*     ownerAt(int x, int y) const;
    bool        isWinningMove(int x, int y, Player* player) const;
    bool        isWinningMoveOnBoard(const std::vector<std::vector<int>> &board, int x, int y, int playerValue) const;
    bool        checkLine(int x, int y, int dx, int dy, Player* player) const;
    bool        checkLineOnBoard(const std::vector<std::vector<int>> &board, int x, int y, int dx, int dy, int playerValue) const;
    int         getDropRow(int column) const;
    int         getDropRowOnBoard(const std::vector<std::vector<int>> &board, int column) const;
    bool        dropPiece(int column, int row, Player* player);
    std::vector<std::vector<int>> getBoardArray() const;

    Grid*       _grid;
};
