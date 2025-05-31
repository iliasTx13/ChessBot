#pragma once
#include <stdint.h>
#include <iostream>
#include <cstdlib>
#define maxMoves 250//upper limit for possible moves
#define maxInf 32000//eval for checkmate
#define minInf 31000//max eval without checkmate in X (may not be enough for custom material)
typedef uint_fast8_t byte;
typedef int_fast8_t dPos;
enum struct pT : byte {
	none, pawn, knight, bishop, rook, queen, king
};
enum struct endState : byte {
	checkmate = maxMoves + 1, stalemate, insuff_mat, illegal
};
struct Piece {
	pT type;
	bool isW;
};
struct Move {
	byte start;
	byte end;
	byte extra;
	byte casChange;
};
class ChessBoard {
public://could be private
	static const dPos nAtk[8];
	static const dPos kAtk[8];
	bool wTurn;
	byte casRight;
	Piece piece[64];
public://could be private
	bool isCheck(bool forW, byte kPos = 64);
	byte possibleMoves(Move* moves, Move* prev);
	int evaluation();
	int endEval(endState end);
	bool checkNsave(Move* moves, byte& size, byte pos);
	bool canShort(byte kPos);
	bool canLong(byte kPos);
public:
	ChessBoard();
	ChessBoard(const ChessBoard& other);
	ChessBoard& operator=(const ChessBoard& other);
	void doMove(const Move& move);
	void unMove(const Move& move);
	byte legalMoves(Move* moves, Move* prev = nullptr);
	int deepEvaluation(byte depth, int prevEval, Move* prev);
	friend std::ostream& operator<<(std::ostream& stream, const ChessBoard& data);
};
inline byte posFile(byte pos) {
	return pos % 8;
}
inline byte posRank(byte pos) {
	return pos >> 3;
}
inline pT moveSpecial(byte extra) {
	return pT(extra % 8);
}
inline int whiteConv(int eval) {//conversion to track white checkmate distance
	return eval > minInf && eval != maxInf ? eval + 1 : eval;
}
inline int blackConv(int eval) {//conversion to track black checkmate distance
	return eval < -minInf && eval != -maxInf ? eval - 1 : eval;
}