#include <iostream>
#include <string>
#include "board.h"
using namespace std;
bool strToMove(const string inp, Move& outp);
string moveToStr(const Move& move);
int main() {
	string textInp;
	int depth;
	Move currentM;
	ChessBoard* board = new ChessBoard;
	Move* legals = new Move[maxMoves];
	byte size;
	bool botIsWhite;
	cout << "ChessBot v1.0 by tx13" << endl;
	do {
		cout << "Set depth at: ";
		cin >> depth;
		std::cout << "Bot plays as (w/b): ";
		cin >> textInp;
		botIsWhite = textInp == "w";
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
				cout << "Bot plays: " << moveToStr(currentM) << endl;
				cout << "Evaluation: " << bestEval << endl;
			}
			else {//opp's turn
				cout << "Opponent plays: ";
				cin >> textInp;
				if (textInp == "end") {
					cout << "Game terminated" << endl;
					break;
				}
				if (!strToMove(textInp, currentM)) {
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
						cout << "Declined: Invalid promotion" << endl;
						continue;
					}
					currentM.extra = legals[i].extra;//both are non promotions
					break;
				default:
					if (moveSpecial(currentM.extra) == pType::none) {
						cout << "Declined: Promotion missing" << endl;
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
		cout << "Play new game? (y/n)" << endl;
		cin >> textInp;
	} while (textInp == "y");
	delete board;
	delete[] legals;
	return 0;
}
bool strToMove(const string inp, Move& outp) {
	if (inp.size() != 4 && inp.size() != 5)
		return false;
	if (inp[0] < 'a' || inp[0] > 'h' || inp[1] < '1' || inp[1] > '8')
		return false;
	outp.start = inp[0] - 'a' + (inp[1] - '1') * 8;
	if (inp[2] < 'a' || inp[2] > 'h' || inp[3] < '1' || inp[3] > '8')
		return false;
	outp.end = inp[2] - 'a' + (inp[3] - '1') * 8;
	if (inp.size() == 4) {
		outp.extra = byte(pType::none);
		return true;
	}
	switch (inp[4]) {
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
	return true;
}
string moveToStr(const Move& move) {
	string result;
	result += posFile(move.start) + 'a';
	result += posRank(move.start) + '1';
	result += posFile(move.end) + 'a';
	result += posRank(move.end) + '1';
	switch (moveSpecial(move.extra)) {
	case pType::knight:
		result += 'n';
		break;
	case pType::bishop:
		result += 'b';
		break;
	case pType::rook:
		result += 'r';
		break;
	case pType::queen:
		result += 'q';
	}
	return result;
}