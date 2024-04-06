#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>
typedef unsigned char byte;
using namespace std;

#define AB_DEPTH 13
#define AB_VALUE_MIN -1000
#define AB_VALUE_MAX 1000

class HoleIterator : iterator<input_iterator_tag, pair<int, int>> {
private:
	int cnt;
	int p, q;
	bool step_zero;
	pair<int, int> internal_pair;

public:
	HoleIterator(int p, int q, int count) : cnt(count), p(p), q(q), step_zero(true) {}

	void operator++() {
		if (cnt <= 0)
			return;
		if (q > 0 && q < 6) {
			q++;
		} else if (q == 0) {
			step_zero = false;
			p = 1 - p;
			q = 1;
		} else if (q == 6 && step_zero) {
			q = 0;
		} else {
			step_zero = true;
			p = 1 - p;
			q = 1;
		}
		internal_pair = {p, q};
		cnt--;
	}
	void operator++(int) {
		operator++();
	}

	bool valid() const {
		return cnt > 0;
	}

	const pair<int, int> *operator->() const {
		return &internal_pair;
	}
};

struct GameState {
	// hole[][0] for score
	byte hole[2][7];
	int next_player;
	int winner = 0; // just mark the player whose holes are empty
	void move(int);
	bool gameover() const;
	vector<pair<int, GameState>> possibleMoves() const;
	int score() const;

	GameState(int player, int state[]) : next_player(player) {
		for (int i = 0; i < 6; i++) {
			hole[0][i + 1] = state[i];
			hole[1][i + 1] = state[i + 7];
		}
		hole[0][0] = state[6];
		hole[1][0] = state[13];
	}
};
int myid;

void GameState::move(const int choice) {
	// seeding routine
	HoleIterator hi(next_player, choice, hole[next_player][choice]);
	hole[next_player][choice] = 0;
	do {
		++hi;
		++hole[hi->first][hi->second];
	} while (hi.valid());
	// check bonus or takeover
	if (hi->first == next_player && hi->second > 0 && hole[next_player][hi->second] == 1 &&
	    hole[1 - next_player][7 - hi->second] > 0) { // takeover
		hole[next_player][0] += hole[1 - next_player][7 - hi->second] + 1;
		hole[1 - next_player][7 - hi->second] = 0;
		hole[next_player][hi->second] = 0;
		next_player = 1 - next_player;
	} else if (hi->second > 0) { // not a bonus
		next_player = 1 - next_player;
	}
	// check if game is over
	if (accumulate(hole[0] + 1, hole[0] + 7, 0) == 0) {
		winner = 1;
	} else if (accumulate(hole[0], hole[0] + 7, 0) + hole[1][0] == 48) {
		winner = 2;
	}
	if (winner) {
		// calculate final score
		hole[2 - winner][0] += accumulate(hole[2 - winner] + 1, hole[2 - winner] + 7, 0);
		// fill(hole[2 - winner] + 1, hole[2 - winner] + 7, 0);
	}
}
bool GameState::gameover() const {
	return winner;
}
vector<pair<int, GameState>> GameState::possibleMoves() const {
	vector<pair<int, GameState>> moves;
	for (int i = 1; i <= 6; i++) {
		if (hole[next_player][i]) {
			GameState ts = *this;
			ts.move(i);
			moves.push_back({i, ts});
		}
	}
	sort(moves.begin(), moves.end(),
	     [](const pair<int, GameState> &a, const pair<int, GameState> &b) {
		     return a.second.score() > b.second.score();
	     });
	return moves;
}
int GameState::score() const {
	int score = (int)hole[myid][0] - (int)hole[1 - myid][0];
	if (gameover())
		return score;
	for (int i = 1; i <= 6; i++) {
		int need = (7 - i) - hole[myid][i];
		if (need == 0) { // bonus
			score += 6;
		} else if (need > 0) { // normal case
			if (hole[myid][i] && hole[myid][7 - need] == 0 && hole[1 - myid][need]) {
				// takeover
				score += (hole[1 - myid][need] + 1) * 5 / 4;
			}
		}

		need = (7 - i) - hole[1 - myid][i];
		if (need == 0) { // rival bonus
			score -= 6;
		} else if (need > 0) { // rival normal case
			if (hole[1 - myid][i] && hole[1 - myid][7 - need] == 0 &&
			    hole[myid][need]) {
				// rival takeover
				score -= (hole[myid][need] + 1) * 5 / 4;
			}
		}
	}
	return score;
}

pair<int, int> maxAB(int, pair<int, int>, const vector<pair<int, GameState>> &);
pair<int, int> minAB(int, pair<int, int>, const vector<pair<int, GameState>> &);

// AB function: return value = <value, move>
pair<int, int> runAB(int k, const GameState &s, pair<int, int> ab) {
	if (k == 0 || s.gameover()) // stop recursive searching
		return {s.score(), -1};
	if (s.next_player == myid) // check if is my turn
		return maxAB(k - 1, ab, s.possibleMoves());
	else
		return minAB(k - 1, ab, s.possibleMoves());
}

pair<int, int> maxAB(int k, pair<int, int> ab, const vector<pair<int, GameState>> &allMoves) {
	int a = ab.first, b = ab.second;
	int v = AB_VALUE_MIN, move;

	for (const auto &am : allMoves) {
		auto v2 = runAB(k, am.second, {a, b}).first;

		if (v2 > v) {
			a = max(a, v2);
			v = v2;
			move = am.first;
		}
		if (v > b)
			return {v, move};
	}
	return {v, move};
}

pair<int, int> minAB(int k, pair<int, int> ab, const vector<pair<int, GameState>> &allMoves) {
	int a = ab.first, b = ab.second;
	int v = AB_VALUE_MAX, move;

	for (const auto &am : allMoves) {
		auto v2 = runAB(k, am.second, {a, b}).first;

		if (v2 < v) {
			b = min(b, v2);
			v = v2;
			move = am.first;
		}
		if (v <= a)
			return {v, move};
	}
	return {v, move};
}

extern "C" int mancalaOperator(int flag, int state[]) {
	if (state[6] + state[13] == 0) { // special best choice
		delete[] state;
		return flag * 10 + 3;
	}
	myid = flag - 1;
	GameState orig(myid, state);
	delete[] state;
	auto m = runAB(AB_DEPTH, orig, {AB_VALUE_MIN, AB_VALUE_MAX}).second;
	return flag * 10 + m;
}
