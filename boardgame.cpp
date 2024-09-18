#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>

using namespace std;

// Enum to represent the different types of cells on the board
enum CellType {
  EMPTY,
  BLUE,
  RED,
  DELETED
};

// Represents a cell on the board
struct Cell {
  int row;
  int col;
  CellType type;
};

// Represents the game board
class Board {
 public:
  int prow=0,pcol=0;
  // Initializes the board with the starting positions of the players
  Board() {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        if (i == 0 && j == 0) {
          cells[i][j] = {i, j, BLUE};
        } else if (i == 6 && j == 6) {
          cells[i][j] = {i, j, RED};
        } else {
          cells[i][j] = {i, j, EMPTY};
        }
      }
    }
  }

  // Returns the cell at the given row and column
  Cell getCell(int row, int col) {
    return cells[row][col];
  }

  // Sets the cell at the given row and column to the given type
  void setCell(int row, int col, CellType type) {
    cells[row][col] = {row, col, type};
  }

  // Returns the current player (either BLUE or RED)
  CellType getCurrentPlayer() {
    return currentPlayer;
  }

  // Switches the current player (BLUE -> RED or RED -> BLUE)
  void switchPlayer() {
    currentPlayer = (currentPlayer == BLUE) ? RED : BLUE;
  }

  // Prints the board to the console
  void print() {
    cout << "  a b c d e f g" << endl;
    for (int i = 0; i < 7; i++) {
      cout << i + 1 << " ";
      for (int j = 0; j < 7; j++) {
        Cell cell = cells[i][j];
        if (cell.type == BLUE) {
          cout << "B ";
        } else if (cell.type == RED) {
          cout << "R ";
        } else if (cell.type == DELETED) {
          cout << "X ";
        } else {
          cout << "  ";
        }
      }
      cout << endl;
    }
  }


 private:
  Cell cells[7][7];
  CellType currentPlayer = BLUE;
};

// Represents a move that can be made on the board
struct Move {
  int row;
  int col;
  int deletedRow;
  int deletedCol;
};

// Returns a list of all the possible moves that can be made from the given position
// Returns a list of all the possible moves that can be made from the given position
vector<Move> getPossibleMoves(Board board, int row, int col) {
  vector<Move> moves;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue;
      int newRow = row + i;
      int newCol = col + j;
      if (newRow < 0 || newRow >= 7 || newCol < 0 || newCol >= 7) continue;
      Cell chosenCell = board.getCell(newRow, newCol);
      if (chosenCell.type == EMPTY) {
        // Loop through all the cells on the board
        for (int k = 0; k < 7; k++) {
          for (int l = 0; l < 7; l++) {
            // Add a move for every empty cell on the board
            if (board.getCell(k, l).type == EMPTY) {
              moves.push_back({newRow, newCol, k, l});
            }
          }
        }
      }
    }
  }
  return moves;
}


// Makes the given move on the board
void makeMove(Board &board, Move move) {
  int row = move.row;
  int col = move.col;
  int deletedRow = move.deletedRow;
  int deletedCol = move.deletedCol;
  Cell cell = board.getCell(row, col);
  if (cell.type == BLUE) {
    board.setCell(row, col, EMPTY);
    board.setCell(deletedRow, deletedCol, BLUE);
  } else {
    board.setCell(row, col, EMPTY);
    board.setCell(deletedRow, deletedCol, RED);
  }
  board.setCell(deletedRow, deletedCol, DELETED);
  board.switchPlayer();
}

// Returns true if the given player has won the game, false otherwise
bool hasWon(Board board, CellType player) {
  int row=-1;
  int col=-1;
  player = (player == BLUE) ? RED : BLUE;
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board.getCell(i, j).type == player) {
        row = i;
        col = j;
        break;
      }
    }
  }
  if (row == -1 || col == -1) return true;
  vector<Move> moves = getPossibleMoves(board, row, col);
  return moves.empty();
}

int minimax(Board board, CellType player, int depth, int alpha, int beta, bool maximizingPlayer) {
  if (depth == 0 || hasWon(board, player)) {
    return (player == RED) ? depth - 10 : 10 - depth;
  }
  if (maximizingPlayer) {
    int maxScore = -10000;
    int row = -1;
    int col = -1;
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        if (board.getCell(i, j).type == RED) {
          row = i;
          col = j;
          break;
        }
      }
    }
    vector<Move> moves = getPossibleMoves(board, row, col);
    for (Move move : moves) {
      Board newBoard = board;
      makeMove(newBoard, move);
      int score = minimax(newBoard, player, depth - 1, alpha, beta, false);
      maxScore = max(maxScore, score);
      alpha = max(alpha, score);
      if (beta <= alpha) {
        break;
      }
    }
    return maxScore;
  } else {
    int minScore = 10000;
    int row = -1;
    int col = -1;
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 7; j++) {
        if (board.getCell(i, j).type == RED) {
          row = i;
          col = j;
          break;
        }
      }
    }
    vector<Move> moves = getPossibleMoves(board, row, col);
    for (Move move : moves) {
      Board newBoard = board;
      makeMove(newBoard, move);
      int score = minimax(newBoard, player, depth - 1, alpha, beta, true);
      minScore = min(minScore, score);
      beta = min(beta, score);
      if (beta <= alpha) {
        break;
      }
    }
    return minScore;
  }
}

// Makes a move for the AI player
void makeAIMove(Board &board) {
  int row = -1;
  int col = -1;
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board.getCell(i, j).type == BLUE) {
        row = i;
        col = j;
        break;
      }
    }
  }
  int bestScore = -10000;
  int bestRow = -1;
  int bestCol = -1;
  int bestDeletedRow = -1;
  int bestDeletedCol = -1;
  vector<Move> moves = getPossibleMoves(board, row, col);
  for (Move move : moves) {
    Board newBoard = board;
    makeMove(newBoard, move);
    int score = minimax(newBoard, BLUE, 2, -10000, 10000, true);
    if (score > bestScore) {
      bestScore = score;
      bestDeletedRow = move.deletedRow;
      bestDeletedCol = move.deletedCol;
    }
  }
  board.setCell(bestDeletedRow, bestDeletedCol, DELETED);
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < 7; j++) {
      if (board.getCell(i, j).type == RED) {
        row = i;
        col = j;
        break;
      }
    }
  }
  bestScore = -10000;
  moves = getPossibleMoves(board, row, col);
  for (Move move : moves) {
    Board newBoard = board;
    makeMove(newBoard, move);
    int score = minimax(newBoard, RED, 2, -10000, 10000, true);
    if (score > bestScore) {
      bestScore = score;
      bestRow = move.row;
      bestCol = move.col;
    }
  }
  board.setCell(bestRow, bestCol, RED);
  board.setCell(row, col, EMPTY);
  board.switchPlayer();
}

int main() {
  Board board;
  while (true) {
    board.print();
    if (board.getCurrentPlayer() == BLUE) {
      cout << "Enter your move (col row delcol delrow): ";
      int row, col, deletedRow, deletedCol;
      char colchar,delcolchar;
      cin >>colchar >> row >> delcolchar >> deletedRow;
      row--;
      col= colchar-'a';
      deletedRow--;
      deletedCol = delcolchar - 'a';
      if (board.getCell(row, col).type != EMPTY) {
        cout << "Invalid move 1" << endl;
        continue;
      }
      if (board.getCell(deletedRow, deletedCol).type != EMPTY) {
        cout << "Invalid move 2" << endl;
        continue;
      }

      // Get all legal moves for the current player
      vector<Move> legalMoves = getPossibleMoves(board, board.prow, board.pcol);

      // Check if the entered move is a legal move
      bool isLegalMove = false;
      for (Move move : legalMoves) {
        if (move.row == row && move.col == col) {
          isLegalMove = true;
          break;
        }
      }
      if (!isLegalMove) {
        cout << "Invalid move: not a legal move" << endl;
        continue;
      }

      board.setCell(board.prow, board.pcol, EMPTY);
      board.setCell(row, col, BLUE);
      board.setCell(deletedRow, deletedCol, DELETED);
      board.prow = row;
      board.pcol = col;
      board.switchPlayer();
    } else {
      cout << "AI is making a move" << endl;
      makeAIMove(board);
    }
    if (hasWon(board, RED)) {
      cout << "AI has won!" << endl;
      break;
    }
    if (hasWon(board, BLUE)) {
      cout << "You have won!" << endl;
      break;
    }
  }
  return 0;
}
