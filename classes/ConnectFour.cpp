#include "ConnectFour.h"

ConnectFour::ConnectFour() : Game()
{
    _grid = new Grid(kColumns, kRows);
}

ConnectFour::~ConnectFour()
{
    delete _grid;
}

void ConnectFour::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = kColumns;
    _gameOptions.rowY = kRows;

    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

Bit* ConnectFour::createPiece(Player* player)
{
    Bit* bit = new Bit();
    bool isRed = (player == getPlayerAt(kRedPlayer));
    bit->LoadTextureFromFile(isRed ? "red.png" : "yellow.png");
    bit->setOwner(player);
    //bit->setGameTag(player->playerNumber() + 1);
    return bit;
}

int ConnectFour::getDropRow(int column) const
{
    for (int y = kRows - 1; y >= 0; --y) {
        ChessSquare* square = _grid->getSquare(column, y);
        if (square && !square->bit()) {
            return y;
        }
    }
    return -1;
}

bool ConnectFour::dropPiece(int column, int row, Player* player)
{
    if (!player) {
        return false;
    }

    ChessSquare* target = _grid->getSquare(column, row);
    if (!target || target->bit()) {
        return false;
    }

    ChessSquare* topSquare = _grid->getSquare(column, 0);
    if (!topSquare) {
        return false;
    }

    Bit* bit = createPiece(player);
    bit->setPosition(topSquare->getPosition());
    target->setBit(bit);
    bit->moveTo(target->getPosition());
    return true;
}

bool ConnectFour::actionForEmptyHolder(BitHolder &holder)
{
    if (holder.bit()) {
        return false;
    }

    ChessSquare* square = static_cast<ChessSquare*>(&holder);
    int column = square->getColumn();
    int row = getDropRow(column);
    if (row < 0) {
        return false;
    }

    ChessSquare* target = _grid->getSquare(column, row);
    if (!target || target->bit()) {
        return false;
    }

    if (dropPiece(column, row, getCurrentPlayer())) {
        endTurn();
        return true;
    }

    return false;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    return false;
}

void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* ConnectFour::ownerAt(int x, int y) const
{
    ChessSquare* square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

bool ConnectFour::checkLine(int x, int y, int dx, int dy, Player* player) const
{
    for (int i = 1; i < 4; i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (!_grid->isValid(nx, ny)) {
            return false;
        }
        if (ownerAt(nx, ny) != player) {
            return false;
        }
    }
    return true;
}

Player* ConnectFour::checkForWinner()
{
    for (int y = 0; y < kRows; y++) {
        for (int x = 0; x < kColumns; x++) {
            Player* player = ownerAt(x, y);
            if (!player) {
                continue;
            }
            if (checkLine(x, y, 1, 0, player) ||
                checkLine(x, y, 0, 1, player) ||
                checkLine(x, y, 1, 1, player) ||
                checkLine(x, y, -1, 1, player)) {
                return player;
            }
        }
    }
    return nullptr;
}

bool ConnectFour::checkForDraw()
{
    bool isDraw = true;
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

std::string ConnectFour::initialStateString()
{
    return std::string(kColumns * kRows, '0');
}

std::string ConnectFour::stateString()
{
    std::string state(kColumns * kRows, '0');
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit* bit = square->bit();
        if (bit) {
            state[y * kColumns + x] = std::to_string(bit->getOwner()->playerNumber() + 1)[0];
        }
    });
    return state;
}

void ConnectFour::setStateString(const std::string &s)
{
    if (s.length() != kColumns * kRows) {
        return;
    }

    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * kColumns + x;
        int playerNumber = s[index] - '0';
        square->destroyBit();
        if (playerNumber == 1 || playerNumber == 2) {
            Player* player = getPlayerAt(playerNumber - 1);
            Bit* bit = createPiece(player);
            bit->setPosition(square->getPosition());
            square->setBit(bit);
        }
    });
}

std::vector<std::vector<int>> ConnectFour::getBoardArray() const
{
    std::vector<std::vector<int>> board(kRows, std::vector<int>(kColumns, 0));
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit* bit = square->bit();
        if (bit) {
            board[y][x] = bit->getOwner()->playerNumber() + 1;
        }
    });
    return board;
}

int ConnectFour::getDropRowOnBoard(const std::vector<std::vector<int>> &board, int column) const
{
    for (int y = kRows - 1; y >= 0; --y) {
        if (board[y][column] == 0) {
            return y;
        }
    }
    return -1;
}

bool ConnectFour::checkLineOnBoard(const std::vector<std::vector<int>> &board, int x, int y, int dx, int dy, int playerValue) const
{
    for (int i = 1; i < 4; i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (nx < 0 || nx >= kColumns || ny < 0 || ny >= kRows) {
            return false;
        }
        if (board[ny][nx] != playerValue) {
            return false;
        }
    }
    return true;
}

bool ConnectFour::boardFull(const std::vector<std::vector<int>> &board) const
{
    for (int y = 0; y < kRows; y++) {
        for (int x = 0; x < kColumns; x++) {
            if (board[y][x] == 0) {
                return false;
            }
        }
    }
    return true;
}

bool ConnectFour::hasWinner(const std::vector<std::vector<int>> &board, int playerValue) const
{
    for (int y = 0; y < kRows; y++) {
        for (int x = 0; x < kColumns; x++) {
            if (board[y][x] != playerValue) {
                continue;
            }
            if (checkLineOnBoard(board, x, y, 1, 0, playerValue) ||
                checkLineOnBoard(board, x, y, 0, 1, playerValue) ||
                checkLineOnBoard(board, x, y, 1, 1, playerValue) ||
                checkLineOnBoard(board, x, y, -1, 1, playerValue)) {
                return true;
            }
        }
    }
    return false;
}

int ConnectFour::scoreWindow(const int window[4], int playerValue, int opponentValue) const
{
    int playerCount = 0;
    int opponentCount = 0;
    int emptyCount = 0;

    for (int i = 0; i < 4; i++) {
        if (window[i] == playerValue) {
            playerCount++;
        } else if (window[i] == opponentValue) {
            opponentCount++;
        } else {
            emptyCount++;
        }
    }

    if (playerCount == 4) return 100000;
    if (playerCount == 3 && emptyCount == 1) return 50;
    if (playerCount == 2 && emptyCount == 2) return 10;
    if (opponentCount == 3 && emptyCount == 1) return -80;
    if (opponentCount == 4) return -100000;

    return 0;
}

int ConnectFour::evaluateBoard(const std::vector<std::vector<int>> &board, int playerValue, int opponentValue) const
{
    int score = 0;

    // Center column preference
    int centerColumn = kColumns / 2;
    int centerCount = 0;
    for (int y = 0; y < kRows; y++) {
        if (board[y][centerColumn] == playerValue) {
            centerCount++;
        }
    }
    score += centerCount * 6;

    // Horizontal windows
    for (int y = 0; y < kRows; y++) {
        for (int x = 0; x <= kColumns - 4; x++) {
            int window[4] = { board[y][x], board[y][x + 1], board[y][x + 2], board[y][x + 3] };
            score += scoreWindow(window, playerValue, opponentValue);
        }
    }

    // Vertical windows
    for (int x = 0; x < kColumns; x++) {
        for (int y = 0; y <= kRows - 4; y++) {
            int window[4] = { board[y][x], board[y + 1][x], board[y + 2][x], board[y + 3][x] };
            score += scoreWindow(window, playerValue, opponentValue);
        }
    }

    // Diagonal (down-right)
    for (int y = 0; y <= kRows - 4; y++) {
        for (int x = 0; x <= kColumns - 4; x++) {
            int window[4] = { board[y][x], board[y + 1][x + 1], board[y + 2][x + 2], board[y + 3][x + 3] };
            score += scoreWindow(window, playerValue, opponentValue);
        }
    }

    // Diagonal (up-right)
    for (int y = 3; y < kRows; y++) {
        for (int x = 0; x <= kColumns - 4; x++) {
            int window[4] = { board[y][x], board[y - 1][x + 1], board[y - 2][x + 2], board[y - 3][x + 3] };
            score += scoreWindow(window, playerValue, opponentValue);
        }
    }

    return score;
}

int ConnectFour::negamax(std::vector<std::vector<int>> &board, int depth, int alpha, int beta, int playerValue, int opponentValue) const
{
    if (hasWinner(board, opponentValue)) {
        return -kBigScore - depth;
    }
    if (hasWinner(board, playerValue)) {
        return kBigScore + depth;
    }
    if (depth == 0 || boardFull(board)) {
        return evaluateBoard(board, playerValue, opponentValue);
    }

    int bestScore = -kBigScore;

    int preferredColumns[] = {3, 2, 4, 1, 5, 0, 6};
    for (int column : preferredColumns) {
        int row = getDropRowOnBoard(board, column);
        if (row < 0) {
            continue;
        }

        board[row][column] = playerValue;
        int score = -negamax(board, depth - 1, -beta, -alpha, opponentValue, playerValue);
        board[row][column] = 0;

        if (score > bestScore) {
            bestScore = score;
        }
        if (bestScore > alpha) {
            alpha = bestScore;
        }
        if (alpha >= beta) {
            break;
        }
    }

    return bestScore;
}

void ConnectFour::updateAI()
{
    if (!gameHasAI()) {
        return;
    }

    Player* currentPlayer = getCurrentPlayer();
    int currentValue = currentPlayer->playerNumber() + 1;
    int opponentValue = (currentPlayer->playerNumber() == kRedPlayer ? kYellowPlayer : kRedPlayer) + 1;

    std::vector<std::vector<int>> board = getBoardArray();

    int bestScore = -kBigScore;
    int bestColumn = -1;

    int preferredColumns[] = {3, 2, 4, 1, 5, 0, 6};
    for (int column : preferredColumns) {
        int row = getDropRowOnBoard(board, column);
        if (row < 0) {
            continue;
        }

        board[row][column] = currentValue;
        int score = -negamax(board, kMaxDepth - 1, -kBigScore, kBigScore, opponentValue, currentValue);
        board[row][column] = 0;

        if (score > bestScore) {
            bestScore = score;
            bestColumn = column;
        }
    }

    if (bestColumn != -1) {
        int row = getDropRow(bestColumn);
        if (row >= 0) {
            if (dropPiece(bestColumn, row, currentPlayer)) {
                endTurn();
            }
        }
    }
}
