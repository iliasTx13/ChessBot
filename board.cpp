#include "board.h"
ChessBoard::ChessBoard() {//starting position
	wTurn = true;
	casRight = 0b1111;
	piece[0] = { pType::rook, true };
	piece[1] = { pType::knight, true };
	piece[2] = { pType::bishop, true };
	piece[3] = { pType::queen, true };
	piece[4] = { pType::king, true };
	piece[5] = { pType::bishop, true };
	piece[6] = { pType::knight, true };
	piece[7] = { pType::rook, true };
	byte i = 8;
	while (i < 16) {
		piece[i] = { pType::pawn, true };
		i++;
	}
	while (i < 48) {
		piece[i].type = pType::none;
		i++;
	}
	while (i < 56) {
		piece[i] = { pType::pawn, false };
		i++;
	}
	piece[56] = { pType::rook, false };
	piece[57] = { pType::knight, false };
	piece[58] = { pType::bishop, false };
	piece[59] = { pType::queen, false };
	piece[60] = { pType::king, false };
	piece[61] = { pType::bishop, false };
	piece[62] = { pType::knight, false };
	piece[63] = { pType::rook, false };
}
ChessBoard::ChessBoard(const ChessBoard& other) {
	*this = other;
}
ChessBoard& ChessBoard::operator=(const ChessBoard& other) {
	wTurn = other.wTurn;
	casRight = other.casRight;
	for (byte i = 0; i < 64; i++)
		piece[i] = other.piece[i];
	return *this;
}
int ChessBoard::endEval(endState end) {
	switch (end) {
	case endState::illegal:
		return wTurn ? maxInf : -maxInf;
	case endState::insuff_mat:
	case endState::stalemate:
		return 0;
	default://checkmate
		return wTurn ? -maxInf : maxInf;
	}
}
inline bool ChessBoard::noAlly(byte pos) {
	return piece[pos].type == pType::none || piece[pos].isWhite != wTurn;
}
bool ChessBoard::canShort(byte kingPos) {
	if (!(casRight & (wTurn ? 0b0001 : 0b0100)))
		return false;
	if (piece[kingPos + 1].type != pType::none)
		return false;
	if (piece[kingPos + 2].type != pType::none)
		return false;
	if (isCheck(wTurn, kingPos))
		return false;
	if (isCheck(wTurn, kingPos + 1))
		return false;
	return true;
}
bool ChessBoard::canLong(byte kingPos) {
	if (!(casRight & (wTurn ? 0b0010 : 0b1000)))
		return false;
	if (piece[kingPos - 1].type != pType::none)
		return false;
	if (piece[kingPos - 2].type != pType::none)
		return false;
	if (piece[kingPos - 3].type != pType::none)
		return false;
	if (isCheck(wTurn, kingPos))
		return false;
	if (isCheck(wTurn, kingPos - 1))
		return false;
	return true;
}
void ChessBoard::doMove(const Move& move) {
	piece[move.end] = piece[move.start];
	piece[move.start].type = pType::none;
	casRight -= move.casChange;
	switch (moveSpecial(move.extra)) {
	case pType::none:
		break;
	case pType::pawn://en passant
		if (wTurn)
			piece[move.end - 8].type = pType::none;
		else
			piece[move.end + 8].type = pType::none;
		break;
	case pType::king://castling
		if (move.end > move.start) {//short
			piece[move.end + 1].type = pType::none;
			piece[move.end - 1] = { pType::rook, wTurn };
		}
		else {//long
			piece[move.end - 2].type = pType::none;
			piece[move.end + 1] = { pType::rook, wTurn };
		}
		break;
	default://promotion
		piece[move.end].type = moveSpecial(move.extra);
	}
	wTurn = !wTurn;
}
void ChessBoard::unMove(const Move& move) {
	piece[move.start] = piece[move.end];
	piece[move.end] = { pType(move.extra >> 3), wTurn };
	wTurn = !wTurn;
	casRight += move.casChange;
	switch (moveSpecial(move.extra)) {
	case pType::none:
		break;
	case pType::pawn://en passant
		if (wTurn)
			piece[move.end - 8] = { pType::pawn, false };
		else
			piece[move.end + 8] = { pType::pawn, true };
		break;
	case pType::king://castling
		if (move.end > move.start) {//short
			piece[move.end + 1] = { pType::rook, wTurn };
			piece[move.end - 1].type = pType::none;
		}
		else {//long
			piece[move.end - 2] = { pType::rook, wTurn };
			piece[move.end + 1].type = pType::none;
		}
		break;
	default://promotion
		piece[move.start].type = pType::pawn;
	}
}
int ChessBoard::evaluation() {
	int eval = 0;
	bool wMat = false;
	bool bMat = false;
	bool insuff = true;
	for (byte i = 0; i < 64; i++) {
		switch (piece[i].type) {
		case pType::none:
			break;
		case pType::pawn:
			insuff = false;
			eval += piece[i].isWhite ? 100 : -100;
			break;
		case pType::knight:
		case pType::bishop:
			if (piece[i].isWhite) {
				if (wMat)
					insuff = false;
				else
					wMat = true;
				eval += 300;
			}
			else {
				if (bMat)
					insuff = false;
				else
					bMat = true;
				eval += -300;
			}
			break;
		case pType::rook:
			insuff = false;
			eval += piece[i].isWhite ? 500 : -500;
			break;
		case pType::queen:
			insuff = false;
			eval += piece[i].isWhite ? 900 : -900;
			break;
		default://king
			if (isCheck(!wTurn, i))//illegal position
				return wTurn ? maxInf : -maxInf;
		}
	}
	if (insuff)
		return 0;
	return eval;
}
bool ChessBoard::isCheck(bool forWhite, byte kingPos) {
	if (kingPos == 64) {//unknown
		kingPos = 0;
		while (piece[kingPos].type != pType::king || piece[kingPos].isWhite != forWhite)
			kingPos++;
	}
	//-straight
	//-diagonal
	//-knight
	//-pawn
	return false;
}
byte ChessBoard::possibleMoves(Move* moves, Move* previous) {
	byte size = 0;
	byte first = 0;//piece move
	byte mat = 0;
	for (byte i = 0; i < 64; i++) {
		if (piece[i].isWhite != wTurn)
			continue;
		switch (piece[i].type) {//-set end, castling, special, size, mat
		case pType::none:
			break;
		case pType::pawn:
			//-
			mat = 2;
			break;
		case pType::knight:
			static const byte knightMoves[8] = {
				-17, -15, -10, -6, 6, 10, 15, 17
			};//overflow intented
			byte inBoard = 0b11111111;//1 bit per move
			switch (posFile(i)) {
			case 0:
				inBoard &= 0b10111110;
			case 1:
				inBoard &= 0b11101011;
				break;
			case 7:
				inBoard &= 0b01111101;
			case 6:
				inBoard &= 0b11010111;
			}
			switch (posRank(i)) {
			case 0:
				inBoard &= 0b11110011;
			case 1:
				inBoard &= 0b11111100;
				break;
			case 7:
				inBoard &= 0b11001111;
			case 6:
				inBoard &= 0b00111111;
			}
			for (byte j = 0; j < 8; j++)
				if ((inBoard >> j) % 2)
					if (noAlly(i + knightMoves[j])) {
						moves[size].extra = 0;
						moves[size].casChange = 0;
						moves[size++].end = i + knightMoves[j];
					}
			mat++;
			break;
		case pType::bishop:
			//-
			mat++;
			break;
		case pType::rook:
			byte casCache = 0;
			if (wTurn) {
				switch (i) {
				case 0:
					casCache = casRight & 0b0010;
					break;
				case 7:
					casCache = casRight & 0b0001;
				}
			}
			else {
				switch (i) {
				case 56:
					casCache = casRight & 0b1000;
					break;
				case 63:
					casCache = casRight & 0b0100;
				}
			}
			//-missing block handling
			/*
			for (byte j = i + 8; posRank(j) < 8 && noAlly(j); j += 8)//up
					moves[size++].end = j;
			for (byte j = i - 8; posRank(j) < 8 && noAlly(j); j -= 8)//down
				moves[size++].end = j;
			for (byte j = i + 1; posFile(j) > 0 && noAlly(j); j += 1)//right
				moves[size++].end = j;
			for (byte j = i - 1; posFile(j) < 7 && noAlly(j); j -= 1)//left
				moves[size++].end = j;
			*/
			for (byte j = first; j < size; j++) {
				moves[j].extra = 0;
				moves[j].casChange = casCache;
			}
			mat = 2;
			break;
		case pType::queen:
			//-
			mat = 2;
			break;
		case pType::king:
			static const byte kingMoves[8] = {
				1, -1, 8, -8, 7, -7, 9, -9
			};//overflow intented
			byte inBoard = 0b11111111;//1 bit per move
			byte casCache = casRight & (wTurn ? 0b0011 : 0b1100);
			switch (posFile(i)) {
			case 0:
				inBoard &= 0b10110110;
				break;
			case 7:
				inBoard &= 0b01111001;
			}
			switch (posRank(i)) {
			case 0:
				inBoard &= 0b11101010;
				break;
			case 7:
				inBoard &= 0b11010101;
			}
			for (byte j = 0; j < 8; j++)
				if ((inBoard >> j) % 2)
					if (noAlly(i + kingMoves[j])) {
						moves[size].extra = 0;
						moves[size++].end = i + kingMoves[j];
					}
			if (canShort(i)) {
				moves[size].extra = byte(pType::king);
				moves[size++].end = i + 2;
			}
			if (canLong(i)) {
				moves[size].extra = byte(pType::king);
				moves[size++].end = i - 2;
			}
			for (byte j = first; j < size; j++)
				moves[j].casChange = casCache;
		}
		for (; first < size; first++)
			moves[first].start = i;
	}
	if (!size) {//no possibles
		if (isCheck(wTurn))
			return byte(endState::checkmate);
		else
			return byte(endState::stalemate);
	}
	pType capture;
	Move temp;
	first = 0;//non capture
	for (byte i = 0; i < size; i++) {//hande captures
		capture = piece[moves[i].end].type;
		moves[i].extra += byte(capture) << 3;
		switch (capture) {
		case pType::none:
			break;
		case pType::king:
			return byte(endState::illegal);
		default://swap capture with first non capture
			temp = moves[i];
			moves[i] = moves[first];
			moves[first++] = temp;
		}
	}
	if (mat > 1)
		return byte(endState::insuff_mat);
	return size;
}
byte ChessBoard::legalMoves(Move* moves, Move* previous) {
	byte size = possibleMoves(moves, previous);
	if (size > maxMoves)//endState
		return size;
	byte i = 0;
	Move tested;
	while (i < size) {
		tested = moves[i];
		doMove(tested);
		if (isCheck(!wTurn)) {//remove illegal
			size--;
			moves[i] = moves[size];
		}
		else
			i++;
		unMove(tested);
	}
	if (!size) {//all possibles are illegal
		if (isCheck(wTurn))
			return byte(endState::checkmate);
		else
			return byte(endState::stalemate);
	}
	return size;
}
int ChessBoard::deepEvaluation(byte depth, Move* previous) {
	if (!depth)
		return evaluation();
	struct Node {
		Move moves[maxMoves];
		byte size, index;
		int bestEval, minEval, maxEval;
		inline Move& getMove() {
			return moves[index];
		}
	};
	Node* hist = new Node[depth];
	hist[0].size = possibleMoves(hist[0].moves, previous);
	int eval;
	if (hist[0].size > maxMoves) {
		eval = endEval(endState(hist[0].size));
		delete[] hist;
		return eval;
	}
	byte lvl = 0;
	hist[0].index = 0;
	hist[0].bestEval = wTurn ? -maxInf : maxInf;
	hist[0].minEval = -maxInf;
	hist[0].maxEval = maxInf;
	while (true) {//explore downwards loop
		doMove(hist[lvl++].getMove());
		if (lvl == depth)
			eval = evaluation();
		else {
			hist[lvl].size = possibleMoves(hist[lvl].moves, &hist[lvl - 1].getMove());
			if (hist[lvl].size > maxMoves)
				eval = endEval(endState(hist[lvl].size));
			else {//init node
				hist[lvl].index = 0;
				if (wTurn) {
					hist[lvl].bestEval = -maxInf;
					hist[lvl].minEval = whiteConv(hist[lvl - 1].minEval);
					hist[lvl].maxEval = whiteConv(min(hist[lvl - 1].maxEval, hist[lvl - 1].bestEval));
				}
				else {
					hist[lvl].bestEval = maxInf;
					hist[lvl].minEval = blackConv(max(hist[lvl - 1].minEval, hist[lvl - 1].bestEval));
					hist[lvl].maxEval = blackConv(hist[lvl - 1].maxEval);
				}
				continue;//no eval yet
			}
		}
		while (true) {//calculate upwards loop
			unMove(hist[--lvl].getMove());
			if (wTurn) {
				hist[lvl].bestEval = max(hist[lvl].bestEval, eval);
				if (++hist[lvl].index != hist[lvl].size && hist[lvl].bestEval < hist[lvl].maxEval)
					break;//need more evals
				eval = hist[lvl].bestEval;
				if (eval > minInf)//distance white checkmate
					eval--;
				else if (eval == -maxInf)//check for stalemate
					if (!isCheck(true))
						eval = 0;
			}
			else {
				hist[lvl].bestEval = min(hist[lvl].bestEval, eval);
				if (++hist[lvl].index != hist[lvl].size && hist[lvl].bestEval > hist[lvl].minEval)
					break;//need more evals
				eval = hist[lvl].bestEval;
				if (eval < -minInf)//distance black checkmate
					eval++;
				else if (eval == maxInf)//check for stalemate
					if (!isCheck(false))
						eval = 0;
			}
			if (!lvl) {//root eval found
				delete[] hist;
				return eval;
			}
		}
	}
}