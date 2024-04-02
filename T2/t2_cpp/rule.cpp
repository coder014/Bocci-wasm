#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

using namespace std;

#define GAME_OVER(v) (15000 + (int)(v))
#define GAME_OK(v) (20000 + (int)(v))
#define GAME_ERROR(v) (30000 + (int)(v))

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
			p = 3 - p;
			q = 1;
		} else if (q == 6 && step_zero) {
			q = 0;
		} else {
			step_zero = true;
			p = 3 - p;
			q = 1;
		}
		internal_pair = make_pair(p, q);
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

extern "C" int mancalaResult(int flag, int *_seq, int size) {
	unique_ptr<int[]> seq(_seq);
	// hole[][0] for score
	int hole[2][7] = {{0, 4, 4, 4, 4, 4, 4}, {0, 4, 4, 4, 4, 4, 4}};
	int offensive = flag, gameover = 0; // gameover indicates player whose hole's empty

	for (auto i = 0; i < size; i++) {
		// check exceeding moves
		if (gameover)
			return GAME_ERROR(i);
		const int player = seq[i] / 10, choice = seq[i] % 10;
		// check if is the next player
		if (player != flag)
			return GAME_ERROR(i);
		// check if the hole is empty
		if (hole[player - 1][choice] <= 0)
			return GAME_ERROR(i);

		// seeding routine
		HoleIterator hi(player, choice, hole[player - 1][choice]);
		hole[player - 1][choice] = 0;
		do {
			++hi;
			++hole[hi->first - 1][hi->second];
		} while (hi.valid());

		// check bonus or takeover
		if (hi->first == player && hi->second > 0 && hole[player - 1][hi->second] == 1 &&
		    hole[2 - player][7 - hi->second] > 0) { // takeover
			hole[player - 1][0] += hole[2 - player][7 - hi->second] + 1;
			hole[2 - player][7 - hi->second] = 0;
			hole[player - 1][hi->second] = 0;
			flag = 3 - player;
		} else if (hi->second > 0) { // not a bonus
			flag = 3 - player;
		}
		// check if game is over
		if (accumulate(hole[0] + 1, hole[0] + 7, 0) == 0) {
			gameover = 1;
		} else if (accumulate(hole[0], hole[0] + 7, 0) + hole[1][0] == 48) {
			gameover = 2;
		}
	}
	// check insufficient moves
	if (!gameover)
		return GAME_OK(hole[offensive - 1][0]);
	// calculate final score
	hole[2 - gameover][0] += accumulate(hole[2 - gameover] + 1, hole[2 - gameover] + 7, 0);
	fill(hole[2 - gameover] + 1, hole[2 - gameover] + 7, 0);
	return GAME_OVER(hole[offensive - 1][0] - hole[2 - offensive][0]);
}
