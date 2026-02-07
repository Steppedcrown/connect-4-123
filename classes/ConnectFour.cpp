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

    /*if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }*/

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

    Bit* bit = createPiece(getCurrentPlayer());
    bit->setPosition(target->getPosition());
    target->setBit(bit);
    endTurn();
    return true;
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

bool ConnectFour::isWinningMoveOnBoard(const std::vector<std::vector<int>> &board, int x, int y, int playerValue) const
{
    return checkLineOnBoard(board, x, y, 1, 0, playerValue) ||
           checkLineOnBoard(board, x, y, 0, 1, playerValue) ||
           checkLineOnBoard(board, x, y, 1, 1, playerValue) ||
           checkLineOnBoard(board, x, y, -1, 1, playerValue);
}

bool ConnectFour::isWinningMove(int x, int y, Player* player) const
{
    if (!player) {
        return false;
    }
    if (ownerAt(x, y) != player) {
        return false;
    }
    return checkLine(x, y, 1, 0, player) ||
           checkLine(x, y, 0, 1, player) ||
           checkLine(x, y, 1, 1, player) ||
           checkLine(x, y, -1, 1, player);
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

    int selectedColumn = -1;

    for (int column = 0; column < kColumns; column++) {
        int row = getDropRowOnBoard(board, column);
        if (row < 0) {
            continue;
        }
        board[row][column] = currentValue;
        if (isWinningMoveOnBoard(board, column, row, currentValue)) {
            selectedColumn = column;
        }
        board[row][column] = 0;
        if (selectedColumn != -1) {
            break;
        }
    }

    if (selectedColumn == -1) {
        for (int column = 0; column < kColumns; column++) {
            int row = getDropRowOnBoard(board, column);
            if (row < 0) {
                continue;
            }
            board[row][column] = opponentValue;
            if (isWinningMoveOnBoard(board, column, row, opponentValue)) {
                selectedColumn = column;
            }
            board[row][column] = 0;
            if (selectedColumn != -1) {
                break;
            }
        }
    }

    if (selectedColumn == -1) {
        int preferredColumns[] = {3, 2, 4, 1, 5, 0, 6};
        for (int column : preferredColumns) {
            if (getDropRowOnBoard(board, column) >= 0) {
                selectedColumn = column;
                break;
            }
        }
    }

    if (selectedColumn != -1) {
        int row = getDropRow(selectedColumn);
        if (row >= 0) {
            actionForEmptyHolder(*_grid->getSquare(selectedColumn, row));
        }
    }
}
