#pragma once
#include <stdint.h>
#define maxMoves 250//upper limit for possible moves
#define maxInf 32000//eval for checkmate
#define minInf 31000//max eval without checkmate in X (may not be enough for custom material)
typedef uint_fast8_t byte;
enum struct pType : byte {
	none, pawn, knight, bishop, rook, queen, king
};
enum struct endState : byte {
	checkmate = maxMoves + 1, stalemate, insuff_mat, illegal
};
struct Piece {
	pType type;
	bool isWhite;
};
struct Move {
	byte start;
	byte end;
	byte extra;
	byte casChange;
};
class ChessBoard {
	bool isCheck(bool forWhite, byte kingPos = 64);
	byte possibleMoves(Move* moves, Move* previous = nullptr);
	int evaluation();
	int endEval(endState end);
	bool checkNsave(Move* moves, byte& size, byte pos);
	bool canShort(byte kingPos);
	bool canLong(byte kingPos);
public:
	bool wTurn;
	byte casRight;
	Piece piece[64];
	ChessBoard();
	ChessBoard(const ChessBoard& other);
	ChessBoard& operator=(const ChessBoard& other);
	void doMove(const Move& move);
	void unMove(const Move& move);
	byte legalMoves(Move* moves, Move* previous = nullptr);
	int deepEvaluation(byte depth, Move* previous = nullptr);
};
inline byte posFile(byte pos) {
	return pos % 8;
}
inline byte posRank(byte pos) {
	return pos >> 3;
}
inline pType moveSpecial(byte extra) {
	return pType(extra % 8);
}
inline int max(int a, int b) {
	return a > b ? a : b;
}
inline int min(int a, int b) {
	return a < b ? a : b;
}
inline int whiteConv(int eval) {//conversion to track white checkmate distance
	return eval > minInf && eval != maxInf ? eval + 1 : eval;
}
inline int blackConv(int eval) {//conversion to track black checkmate distance
	return eval < -minInf && eval != -maxInf ? eval - 1 : eval;
}