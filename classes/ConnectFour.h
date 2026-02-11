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
    static const int kMaxDepth = 5;
    static const int kBigScore = 1000000;

    Bit*        createPiece(Player* player);
    Player*     ownerAt(int x, int y) const;
    bool        checkLine(int x, int y, int dx, int dy, Player* player) const;
    bool        checkLineOnBoard(const std::vector<std::vector<int>> &board, int x, int y, int dx, int dy, int playerValue) const;
    int         getDropRow(int column) const;
    int         getDropRowOnBoard(const std::vector<std::vector<int>> &board, int column) const;
    bool        dropPiece(int column, int row, Player* player);
    std::vector<std::vector<int>> getBoardArray() const;
    bool        boardFull(const std::vector<std::vector<int>> &board) const;
    bool        hasWinner(const std::vector<std::vector<int>> &board, int playerValue) const;
    int         scoreWindow(const int window[4], int playerValue, int opponentValue) const;
    int         evaluateBoard(const std::vector<std::vector<int>> &board, int playerValue, int opponentValue) const;
    int         negamax(std::vector<std::vector<int>> &board, int depth, int alpha, int beta, int playerValue, int opponentValue) const;

    Grid*       _grid;
    std::chrono::steady_clock::time_point _aiTurnStart;
    unsigned int _aiTurnNumber;
    bool _aiDelayActive;
};
