#include <iostream>
#include "board.h"
#define maxStr 6
using namespace std;
bool strToByte(const char* inp, byte& outp);
bool strToMove(const char* inp, Move& outp);
void moveToStr(const Move& inp, char* outp);
bool userInp(char* inp);
int main() {
	char str[maxStr];
	Move currentM;
	ChessBoard* board = new ChessBoard;
	Move* legals = new Move[maxMoves];
	byte size, depth;
	bool botIsWhite;
	cout << "ChessBot v1.0 by iliasTx13" << endl;
	while (true) {
		while (true) {
			cout << "Set depth at: ";
			if (userInp(str))
				if (strToByte(str, depth))
					break;
			cout << "Declined: not an integer from 0 to 255" << endl;
		}
		while (true) {
			cout << "Bot plays as: ";
			if (userInp(str)) {
				if (!strcmp(str, "white")) {
					botIsWhite = true;
					break;
				}
				if (!strcmp(str, "black")) {
					botIsWhite = false;
					break;
				}
			}
			cout << "Declined: not white or black" << endl;
		}
		*board = ChessBoard();//starting position
		size = board->legalMoves(legals);
		while (true) {//main game loop
			if (board->whiteTurn == botIsWhite) {//bot's turn
				int bestEval = board->whiteTurn ? -maxInf : maxInf;
				int eval;
				for (byte i = 0; i < size; i++) {
					board->doMove(legals[i]);
					eval = board->deepEvaluation(depth, &legals[i]);
					board->unMove(legals[i]);
					if (eval > bestEval == board->whiteTurn) {
						bestEval = eval;
						currentM = legals[i];
					}
				}
				moveToStr(currentM, str);
				cout << "Bot plays: " << str << endl;
				cout << "Evaluation: " << bestEval << endl;
			}
			else {//opp's turn
				cout << "Opponent plays: ";
				if (!userInp(str)) {
					cout << "Declined: Move not recognised" << endl;
					continue;
				}
				if (!strcmp(str, "end")) {
					cout << "Game terminated" << endl;
					break;
				}
				if (!strToMove(str, currentM)) {
					cout << "Declined: Move not recognised" << endl;
					continue;
				}
				byte i = 0;
				for (; i < size; i++)
					if (currentM.start == legals[i].start && currentM.end == legals[i].end)
						break;
				if (i == size) {
					cout << "Declined: Move is illegal" << endl;
					continue;
				}
				switch (moveSpecial(legals[i].extra)) {//check if promotion status matches
				case pType::none:
				case pType::pawn:
				case pType::king:
					if (moveSpecial(currentM.extra) != pType::none) {
						cout << "Declined: Move is illegal" << endl;
						continue;
					}
					currentM.extra = legals[i].extra;//both are non promotions
					break;
				default:
					if (moveSpecial(currentM.extra) == pType::none) {
						cout << "Declined: Move is illegal" << endl;
						continue;
					}
					currentM.extra += legals[i].extra & 0b11111000;//both are promotions
				}
			}
			board->doMove(currentM);
			size = board->legalMoves(legals, &currentM);
			if (size > maxMoves) {//game ended
				switch (endState(size)) {
				case endState::checkmate:
					cout << (board->whiteTurn == botIsWhite ? "Opp" : "Bot") << " wins by checkmate" << endl;
					break;
				case endState::stalemate:
					cout << "Draw by stalemate" << endl;
					break;
				case endState::insuff_mat:
					cout << "Draw by insufficient material" << endl;
					break;
				default://illegal, should never occur, exists only for debugging
					cout << "Error illegal position" << endl;
				}
				break;
			}
		}
		while (true) {
			cout << "Play new game? ";
			if (userInp(str)) {
				if (!strcmp(str, "yes"))
					break;
				if (!strcmp(str, "no")) {
					delete board;
					delete[] legals;
					return 0;
				}
			}
			cout << "Declined: not yes or no" << endl;
		}
	}
}
bool strToMove(const char* inp, Move& outp) {
	if (inp[0] < 'a' || inp[0] > 'h' || inp[1] < '1' || inp[1] > '8')
		return false;
	outp.start = inp[0] - 'a' + (inp[1] - '1') * 8;
	if (inp[2] < 'a' || inp[2] > 'h' || inp[3] < '1' || inp[3] > '8')
		return false;
	outp.end = inp[2] - 'a' + (inp[3] - '1') * 8;
	switch (inp[4]) {
	case '\0':
		outp.extra = byte(pType::none);
		return true;
	case 'n':
		outp.extra = byte(pType::knight);
		break;
	case 'b':
		outp.extra = byte(pType::bishop);
		break;
	case 'r':
		outp.extra = byte(pType::rook);
		break;
	case 'q':
		outp.extra = byte(pType::queen);
		break;
	default:
		return false;
	}
	return inp[5] == '\0';
}
void moveToStr(const Move& inp, char* outp) {
	outp[0] = posFile(inp.start) + 'a';
	outp[1] = posRank(inp.start) + '1';
	outp[2] = posFile(inp.end) + 'a';
	outp[3] = posRank(inp.end) + '1';
	switch (moveSpecial(inp.extra)) {
	case pType::knight:
		outp[4] = 'n';
		break;
	case pType::bishop:
		outp[4] = 'b';
		break;
	case pType::rook:
		outp[4] = 'r';
		break;
	case pType::queen:
		outp[4] = 'q';
		break;
	default:
		outp[4] = '\0';
		return;
	}
	outp[5] = '\0';
}
bool userInp(char* inp) {
	if (cin.getline(inp, maxStr, '\n'))
		return true;
	if (!(cin.eof() || cin.bad())) {
		cin.clear();
		if (cin.ignore(numeric_limits<streamsize>::max(), '\n'))
			return false;
	}
	exit(1);
}
bool strToByte(const char* inp, byte& outp) {
	//-todo
}
