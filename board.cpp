#include "board.h"
const dPos ChessBoard::nAtk[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
const dPos ChessBoard::kAtk[8] = { 1, -1, 8, -8, 7, -7, 9, -9 };
ChessBoard::ChessBoard() {//starting position
	wTurn = true;
	casRight = 0b1111;
	piece[0] = { pT::rook, true };
	piece[1] = { pT::knight, true };
	piece[2] = { pT::bishop, true };
	piece[3] = { pT::queen, true };
	piece[4] = { pT::king, true };
	piece[5] = { pT::bishop, true };
	piece[6] = { pT::knight, true };
	piece[7] = { pT::rook, true };
	byte i = 8;
	while (i < 16) {
		piece[i] = { pT::pawn, true };
		i++;
	}
	while (i < 48) {
		piece[i] = { pT::none, false };
		i++;
	}
	while (i < 56) {
		piece[i] = { pT::pawn, false };
		i++;
	}
	piece[56] = { pT::rook, false };
	piece[57] = { pT::knight, false };
	piece[58] = { pT::bishop, false };
	piece[59] = { pT::queen, false };
	piece[60] = { pT::king, false };
	piece[61] = { pT::bishop, false };
	piece[62] = { pT::knight, false };
	piece[63] = { pT::rook, false };
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
bool ChessBoard::checkNsave(Move* moves, byte& size, byte pos) {
	if (piece[pos].type != pT::none) {
		if (piece[pos].isW != wTurn)
			moves[size++].end = pos;
		return true;
	}
	moves[size++].end = pos;
	return false;
}
bool ChessBoard::canShort(byte kPos) {
	if (!(casRight & (wTurn ? 0b0001 : 0b0100)))
		return false;
	if (piece[kPos + 1].type != pT::none ||
		piece[kPos + 2].type != pT::none)
		return false;
	if (isCheck(wTurn, kPos))
		return false;
	return !isCheck(wTurn, kPos + 1);
}
bool ChessBoard::canLong(byte kPos) {
	if (!(casRight & (wTurn ? 0b0010 : 0b1000)))
		return false;
	if (piece[kPos - 1].type != pT::none ||
		piece[kPos - 2].type != pT::none ||
		piece[kPos - 3].type != pT::none)
		return false;
	if (isCheck(wTurn, kPos))
		return false;
	return !isCheck(wTurn, kPos - 1);
}
void ChessBoard::doMove(const Move& move) {
	piece[move.end] = piece[move.start];
	piece[move.start].type = pT::none;
	casRight -= move.casChange;
	switch (moveSpecial(move.extra)) {
	case pT::none:
		break;
	case pT::pawn://en passant
		if (wTurn)
			piece[move.end - 8].type = pT::none;
		else
			piece[move.end + 8].type = pT::none;
		break;
	case pT::king://castling
		if (move.end > move.start) {//short
			piece[move.end + 1].type = pT::none;
			piece[move.end - 1] = { pT::rook, wTurn };
		}
		else {//long
			piece[move.end - 2].type = pT::none;
			piece[move.end + 1] = { pT::rook, wTurn };
		}
		break;
	default://promotion
		piece[move.end].type = moveSpecial(move.extra);
	}
	wTurn = !wTurn;
}
void ChessBoard::unMove(const Move& move) {
	piece[move.start] = piece[move.end];
	piece[move.end] = { pT(move.extra >> 3), wTurn };
	wTurn = !wTurn;
	casRight += move.casChange;
	switch (moveSpecial(move.extra)) {
	case pT::none:
		break;
	case pT::pawn://en passant
		if (wTurn)
			piece[move.end - 8] = { pT::pawn, false };
		else
			piece[move.end + 8] = { pT::pawn, true };
		break;
	case pT::king://castling
		if (move.end > move.start) {//short
			piece[move.end + 1] = { pT::rook, wTurn };
			piece[move.end - 1].type = pT::none;
		}
		else {//long
			piece[move.end - 2] = { pT::rook, wTurn };
			piece[move.end + 1].type = pT::none;
		}
		break;
	default://promotion
		piece[move.start].type = pT::pawn;
	}
}
int ChessBoard::evaluation() {
	int eval = 0;
	bool wMat = false;
	bool bMat = false;
	bool insuff = true;
	for (byte i = 0; i < 64; i++) {
		switch (piece[i].type) {
		case pT::none:
			break;
		case pT::pawn:
			insuff = false;
			eval += piece[i].isW ? 100 : -100;
			break;
		case pT::knight:
		case pT::bishop:
			if (piece[i].isW) {
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
		case pT::rook:
			insuff = false;
			eval += piece[i].isW ? 500 : -500;
			break;
		case pT::queen:
			insuff = false;
			eval += piece[i].isW ? 900 : -900;
			break;
		default://king
			if (piece[i].isW != wTurn)
				if (isCheck(!wTurn, i))//illegal position
					return wTurn ? maxInf : -maxInf;
		}
	}
	if (insuff)
		return 0;
	return eval;
}
bool ChessBoard::isCheck(bool forW, byte kPos) {
	if (kPos == 64) {//unknown
		kPos = 0;
		while (piece[kPos].type != pT::king || piece[kPos].isW != forW) {
			kPos++;
			if (kPos == 64)
				return false;
		}
	}
	byte currRank = posRank(kPos);
	byte currFile = posFile(kPos);
	byte inBoardN = 0b11111111;
	byte inBoardK = 0b11111111;
	switch (currFile) {
	case 0:
		inBoardK &= 0b01101101;
		inBoardN &= 0b10111110;
	case 1:
		inBoardN &= 0b11101011;
		break;
	case 7:
		inBoardK &= 0b10011110;
		inBoardN &= 0b01111101;
	case 6:
		inBoardN &= 0b11010111;
	}
	switch (currRank) {
	case 0:
		inBoardK &= 0b01010111;
		inBoardN &= 0b11110011;
	case 1:
		inBoardN &= 0b11111100;
		break;
	case 7:
		inBoardK &= 0b10101011;
		inBoardN &= 0b11001111;
	case 6:
		inBoardN &= 0b00111111;
	}
	for (byte j = 0; j < 8; j++)
		if ((inBoardN >> j) % 2)
			if (piece[kPos + nAtk[j]].type == pT::knight && piece[kPos + nAtk[j]].isW != forW)
				return true;
	for (byte j = 0; j < 8; j++)
		if ((inBoardK >> j) % 2)
			if (piece[kPos + kAtk[j]].type == pT::king && piece[kPos + kAtk[j]].isW != forW)
				return true;
	if (forW) {
		if (inBoardK & 0b00010000)
			if (piece[kPos + 7].type == pT::pawn && piece[kPos + 7].isW != forW)
				return true;
		if (inBoardK & 0b01000000)
			if (piece[kPos + 9].type == pT::pawn && piece[kPos + 9].isW != forW)
				return true;
	}
	else {
		if (inBoardK & 0b00100000)
			if (piece[kPos - 7].type == pT::pawn && piece[kPos - 7].isW != forW)
				return true;
		if (inBoardK & 0b10000000)
			if (piece[kPos - 9].type == pT::pawn && piece[kPos - 9].isW != forW)
				return true;
	}
	byte kSees[4];
	for (kSees[0] = kPos; posRank(kSees[0]) != 7;)//U
		if (piece[kSees[0] += 8].type != pT::none) break;
	for (kSees[1] = kPos; posRank(kSees[1]) != 0;)//D
		if (piece[kSees[1] -= 8].type != pT::none) break;
	for (kSees[2] = kPos; posFile(kSees[2]) != 7;)//R
		if (piece[kSees[2] += 1].type != pT::none) break;
	for (kSees[3] = kPos; posFile(kSees[3]) != 0;)//L
		if (piece[kSees[3] -= 1].type != pT::none) break;
	for (byte j = 0; j < 4; j++)
		if ((piece[kSees[j]].type == pT::rook || piece[kSees[j]].type == pT::queen) && piece[kSees[j]].isW != forW)
			return true;
	if (currRank > currFile) {//UL
		for (kSees[0] = kPos; posRank(kSees[0]) != 7;)//UR -> U
			if (piece[kSees[0] += 9].type != pT::none) break;
		for (kSees[1] = kPos; posFile(kSees[1]) != 0;)//DL -> L
			if (piece[kSees[1] -= 9].type != pT::none) break;
	}
	else {//DR
		for (kSees[0] = kPos; posFile(kSees[0]) != 7;)//UR -> R
			if (piece[kSees[0] += 9].type != pT::none) break;
		for (kSees[1] = kPos; posRank(kSees[1]) != 0;)//DL -> D
			if (piece[kSees[1] -= 9].type != pT::none) break;
	}
	if (currRank + currFile < 8) {//DL
		for (kSees[2] = kPos; posFile(kSees[2]) != 0;)//UL -> L
			if (piece[kSees[2] += 7].type != pT::none) break;
		for (kSees[3] = kPos; posRank(kSees[3]) != 0;)//DR -> D
			if (piece[kSees[3] -= 7].type != pT::none) break;
	}
	else {//UR
		for (kSees[2] = kPos; posRank(kSees[2]) != 7;)//UL -> U
			if (piece[kSees[2] += 7].type != pT::none) break;
		for (kSees[3] = kPos; posFile(kSees[3]) != 7;)//DR -> R
			if (piece[kSees[3] -= 7].type != pT::none) break;
	}
	for (byte j = 0; j < 4; j++)
		if ((piece[kSees[j]].type == pT::bishop || piece[kSees[j]].type == pT::queen) && piece[kSees[j]].isW != forW)
			return true;
	return false;
}
byte ChessBoard::possibleMoves(Move* moves, Move* prev) {
	byte size = 0;
	byte first = 0;//piece move
	byte mat = 0;
	byte inBoard, tempB, currRank, currFile;
	dPos forw;
	for (byte i = 0; i < 64; i++) {
		if (piece[i].isW != wTurn) {
			switch (piece[i].type) {
			case pT::none:
			case pT::king:
				break;
			case pT::knight:
			case pT::bishop:
				mat++;
				break;
			default:
				mat = 2;
			}
			continue;
		}
		currRank = posRank(i);
		currFile = posFile(i);
		switch (piece[i].type) {//-set end, castling, special, size, mat
		case pT::none:
			break;
		case pT::pawn:
			if (i < 8 || i > 55)
				return byte(endState::illegal);
			forw = wTurn ? 8 : -8;
			if (piece[i + forw].type == pT::none) {
				moves[size++].end = i + forw;
				if (currRank == (wTurn ? 1 : 6))//could move 2 squares
					if (piece[i + 2 * forw].type == pT::none)
						moves[size++].end = i + 2 * forw;
			}
			if (currFile != 0) {
				tempB = i + forw - 1;
				if (piece[tempB].isW != wTurn && piece[tempB].type != pT::none)
					moves[size++].end = tempB;
			}
			if (currFile != 7) {
				tempB = i + forw + 1;
				if (piece[tempB].isW != wTurn && piece[tempB].type != pT::none)
					moves[size++].end = tempB;
			}
			if (currRank == (wTurn ? 6 : 1)) {//promote
				tempB = size;
				for (byte j = first; j < tempB; j++) {
					moves[size].end = moves[j].end;//dublicate x3
					moves[size + 1].end = moves[j].end;
					moves[size + 2].end = moves[j].end;
					moves[j].extra = byte(pT::queen);//apply 4 promotions
					moves[size].extra = byte(pT::rook);
					moves[size + 1].extra = byte(pT::bishop);
					moves[size + 2].extra = byte(pT::knight);
					size += 3;
				}
			}
			else {
				for (byte j = first; j < size; j++)
					moves[j].extra = 0;
				if (prev != nullptr)
					if (piece[prev->end].type == pT::pawn &&
						(prev->end == i + 1 || prev->end == i - 1) &&
						prev->start == prev->end + 2 * forw) {//en passant
						moves[size].extra = byte(pT::pawn);
						moves[size++].end = prev->end + forw;
					}
			}
			for (byte j = first; j < size; j++)
				moves[j].casChange = 0;
			mat = 2;
			break;
		case pT::knight:
			inBoard = 0b11111111;//1 bit per move
			switch (currFile) {
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
			switch (currRank) {
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
				if ((inBoard >> j) % 2) {
					tempB = i + nAtk[j];
					if (piece[tempB].type == pT::none || piece[tempB].isW != wTurn) {
						moves[size].extra = 0;
						moves[size].casChange = 0;
						moves[size++].end = tempB;
					}
				}
			mat++;
			break;
		case pT::queen:
			for (byte j = i; posRank(j) != 7;)//U
				if (checkNsave(moves, size, j += 8)) break;
			for (byte j = i; posRank(j) != 0;)//D
				if (checkNsave(moves, size, j -= 8)) break;
			for (byte j = i; posFile(j) != 7;)//R
				if (checkNsave(moves, size, j += 1)) break;
			for (byte j = i; posFile(j) != 0;)//L
				if (checkNsave(moves, size, j -= 1)) break;
			mat = 2;
			//no break
		case pT::bishop:
			if (currRank > currFile) {//UL
				for (byte j = i; posRank(j) != 7;)//UR -> U
					if (checkNsave(moves, size, j += 9)) break;
				for (byte j = i; posFile(j) != 0;)//DL -> L
					if (checkNsave(moves, size, j -= 9)) break;
			}
			else {//DR
				for (byte j = i; posFile(j) != 7;)//UR -> R
					if (checkNsave(moves, size, j += 9)) break;
				for (byte j = i; posRank(j) != 0;)//DL -> D
					if (checkNsave(moves, size, j -= 9)) break;
			}
			if (currRank + currFile < 8) {//DL
				for (byte j = i; posFile(j) != 0;)//UL -> L
					if (checkNsave(moves, size, j += 7)) break;
				for (byte j = i; posRank(j) != 0;)//DR -> D
					if (checkNsave(moves, size, j -= 7)) break;
			}
			else {//UR
				for (byte j = i; posRank(j) != 7;)//UL -> U
					if (checkNsave(moves, size, j += 7)) break;
				for (byte j = i; posFile(j) != 7;)//DR -> R
					if (checkNsave(moves, size, j -= 7)) break;
			}
			for (byte j = first; j < size; j++) {
				moves[j].extra = 0;
				moves[j].casChange = 0;
			}
			mat++;
			break;
		case pT::rook:
			for (byte j = i; posRank(j) != 7;)//U
				if (checkNsave(moves, size, j += 8)) break;
			for (byte j = i; posRank(j) != 0;)//D
				if (checkNsave(moves, size, j -= 8)) break;
			for (byte j = i; posFile(j) != 7;)//R
				if (checkNsave(moves, size, j += 1)) break;
			for (byte j = i; posFile(j) != 0;)//L
				if (checkNsave(moves, size, j -= 1)) break;
			switch (i) {
			case 0:
				tempB = casRight & 0b0010;
				break;
			case 7:
				tempB = casRight & 0b0001;
				break;
			case 56:
				tempB = casRight & 0b1000;
				break;
			case 63:
				tempB = casRight & 0b0100;
				break;
			default:
				tempB = 0;
			}
			for (byte j = first; j < size; j++) {
				moves[j].extra = 0;
				moves[j].casChange = tempB;
			}
			mat = 2;
			break;
		case pT::king:
			inBoard = 0b11111111;//1 bit per move
			switch (currFile) {
			case 0:
				inBoard &= 0b01101101;
				break;
			case 7:
				inBoard &= 0b10011110;
			}
			switch (currRank) {
			case 0:
				inBoard &= 0b01010111;
				break;
			case 7:
				inBoard &= 0b10101011;
			}
			for (byte j = 0; j < 8; j++)
				if ((inBoard >> j) % 2) {
					tempB = i + kAtk[j];
					if (piece[tempB].type == pT::none || piece[tempB].isW != wTurn) {
						moves[size].extra = 0;
						moves[size++].end = tempB;
					}
				}
			if (canShort(i)) {
				moves[size].extra = byte(pT::king);
				moves[size++].end = i + 2;
			}
			if (canLong(i)) {
				moves[size].extra = byte(pT::king);
				moves[size++].end = i - 2;
			}
			tempB = casRight & (wTurn ? 0b0011 : 0b1100);
			for (byte j = first; j < size; j++)
				moves[j].casChange = tempB;
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
	pT capture;
	Move tempM;
	first = 0;//non capture
	for (byte i = 0; i < size; i++) {//hande captures
		capture = piece[moves[i].end].type;
		moves[i].extra += byte(capture) << 3;
		switch (capture) {
		case pT::none:
			break;
		case pT::king:
			return byte(endState::illegal);
		case pT::rook://possible castling change
			switch (moves[i].end) {
			case 0:
				moves[i].casChange += casRight & 0b0010;
				break;
			case 7:
				moves[i].casChange += casRight & 0b0001;
				break;
			case 56:
				moves[i].casChange += casRight & 0b1000;
				break;
			case 63:
				moves[i].casChange += casRight & 0b0100;
				break;
			}
		default://swap capture with first non capture
			tempM = moves[i];
			moves[i] = moves[first];
			moves[first++] = tempM;
		}
	}
	if (mat < 2)
		return byte(endState::insuff_mat);
	return size;
}
byte ChessBoard::legalMoves(Move* moves, Move* prev) {
	byte size = possibleMoves(moves, prev);
	if (size > maxMoves)//endState
		return size;
	byte i = 0;
	bool result;
	while (i < size) {
		doMove(moves[i]);
		result = isCheck(!wTurn);
		unMove(moves[i]);
		if (result) {//remove illegal
			size--;
			moves[i] = moves[size];
		}
		else
			i++;
	}
	if (!size) {//all possibles are illegal
		if (isCheck(wTurn))
			return byte(endState::checkmate);
		else
			return byte(endState::stalemate);
	}
	return size;
}
int ChessBoard::deepEvaluation(byte depth, int prevEval, Move* prev) {
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
	hist[0].size = possibleMoves(hist[0].moves, prev);
	int eval;
	if (hist[0].size > maxMoves) {
		eval = endEval(endState(hist[0].size));
		delete[] hist;
		return eval;
	}
	byte lvl = 0;
	hist[0].index = 0;
	if (wTurn) {
		hist[0].bestEval = -maxInf;
		hist[0].minEval = -maxInf;
		hist[0].maxEval = whiteConv(prevEval);
	}
	else {
		hist[0].bestEval = maxInf;
		hist[0].minEval = blackConv(prevEval);
		hist[0].maxEval = maxInf;
	}
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
std::ostream& operator<<(std::ostream& stream, const ChessBoard& data) {
	static const char symbs[2][7] = {
		{ '-', 'p', 'n', 'b', 'r', 'q', 'k' },
		{ '-', 'P', 'N', 'B', 'R', 'Q', 'K' }
	};
	Piece current;
	for (dPos r = 7; r >= 0; r--) {
		for (dPos f = 0; f < 8; f++) {
			current = data.piece[r * 8 + f];
			stream << symbs[current.isW][byte(current.type)] << ' ';
		}
		stream << '\n';
	}
	return stream;
}