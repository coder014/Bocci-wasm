#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>
#include <set>
#include <iostream>

using namespace std;

#define GAME_OVER(v) (15000 + (int)(v))
#define GAME_OK(v) (20000 + (int)(v))
#define GAME_ERROR(v) (30000 + (int)(v))

namespace CYS {

class HoleIterator : iterator<input_iterator_tag, pair<int, int> > {
private:
	int cnt;
	int p, q;
	bool step_zero;
	mutable pair<int, int> temp_pair;

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
		cnt--;
	}
	void operator++(int) {
		operator++();
	}

	bool valid() const {
		return cnt > 0;
	}

	const pair<int, int> *operator->() const {
		temp_pair = make_pair(p, q);
		return &temp_pair;
	}
};

extern "C" int mancalaResult(int flag, int *_seq, int size) {
	// unique_ptr<int[]> seq(_seq);
	int *seq = _seq;
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
} // namespace CYS

namespace LYH {
struct situation {
	unsigned char map1[8];
	unsigned char map2[8];
	unsigned char nextrole;
	unsigned char map1count;
	unsigned char map2count;
};
struct openode {
	situation s;
	unsigned char bestpos;
	double value;
	map<unsigned char, openode *> children;
};
vector<openode *> sources;

bool checkend(openode &node){
    auto &s = node.s;
	if(s.map1count*s.map2count == 0){
        s.map1[7]+= s.map1count;
        s.map2[7]+=s.map2count;
        for(int i = 1;i<=6;i++){
            s.map1[i]=0;
            s.map2[i]=0;
        }
        s.map1count = 0;
        s.map2count = 0;
    }
    return s.map1count * s.map2count == 0;
}

openode * operate_on(int pos, openode& orinode){
    auto * node = new openode;
    sources.push_back(node);
    for(int i = 1;i<=7;i++){
        node->s.map1[i]= orinode.s.map1[i];
        node->s.map2[i] = orinode.s.map2[i];
    }
    node->s.nextrole = orinode.s.nextrole;

    int role = node->s.nextrole;
    unsigned char* rolemap = (role == 1)? node->s.map1 : node->s.map2;
    unsigned char* opemap = (role == 1)? node->s.map2 : node->s.map1;
	int take = rolemap[pos];
	rolemap[pos] = 0;

	unsigned char * curmap = rolemap;
	int posto = pos;
	while(take != 0){
		posto ++;
		if(posto == 7 && curmap == opemap){
			posto = 1;
			curmap = rolemap;
		}else if(posto == 8 && curmap == rolemap){
			posto = 1;
			curmap = opemap;
		}
		curmap[posto] ++;
		take --;
	}
	if(curmap == rolemap && posto < 7 && curmap[posto] == 1 && opemap[7-posto]>0){
		int oppo = opemap[7 - posto];
		opemap[7 - posto] = 0;
		curmap[posto] = 0;
		curmap[7] += (oppo+1);
	}
	if(posto == 7 && curmap == rolemap){
		node->s.nextrole = role;
	}else{
		node->s.nextrole = (role == 1)? 2:1;
	}  

    //post operation
    int count1 = 0, count2 = 0;
    for(int i = 1;i<=6;i++){
        count1+=node->s.map1[i];
        count2 += node->s.map2[i];
    }
    node->s.map1count = count1;
    node->s.map2count = count2;

    checkend(*node);

    return node;
}

set<int> get_valid_operate(openode & node){
    set<int> res;
    unsigned char* curmap = node.s.nextrole==1? node.s.map1 : node.s.map2;
    for(int i = 1; i <= 6; i++){
        if(curmap[i]>0){
            res.insert(i);
        }
    }
    return res;
}

openode *init_map(int flag, openode *newnode) {
	openode &node = *newnode;
	for (int j = 1; j <= 7; j++) {
		if (j != 7) {
			node.s.map1[j] = 4;
			node.s.map2[j] = 4;
		} else {
			node.s.map1[j] = 0;
			node.s.map2[j] = 0;
		}
	}
	node.s.nextrole = flag;
	node.s.map1count = 14;
	node.s.map2count = 14;
	return newnode;
}
openode *init_map(int flag) {
	auto newnode = new openode;
	sources.push_back(newnode);
	return init_map(flag, newnode);
}

extern "C" int macala_result(int flag, int *seq, int size) {
	openode node = *init_map(flag);

	for (int i = 0; i < size; i++) {
		int role = seq[i] / 10;
		int pos = seq[i] % 10;
		if (checkend(node)) {
			return 30000 + i;
		}
		auto validpos = get_valid_operate(node);
		if ((role != node.s.nextrole) || (!validpos.count(pos))) {
			for(auto ptr: sources){
				delete ptr;
			}
			sources.clear();
			return 30000 + i;
		}
		node = *operate_on(pos, node);
	}

	auto rolemap = flag == 1 ? node.s.map1 : node.s.map2;
	auto opemap = flag == 1 ? node.s.map2 : node.s.map1;

	for(auto ptr: sources){
		delete ptr;
	}
	sources.clear();
	if (checkend(node)) {
		return 15000 + rolemap[7] - opemap[7];
	} else {
		return 20000 + rolemap[7];
	}

	return 0;
}
} // namespace LYH

int main() {
	queue<unique_ptr<vector<int> > > Q;
	Q.push(move(unique_ptr<vector<int> >(new vector<int>)));
	for (;;) {
		auto v = move(Q.front());
		for (int i = 1; i <= 2; i++)
			for (int j = 1; j <= 6; j++) {
				v->push_back(i * 10 + j);
				int res1 = CYS::mancalaResult((*v)[0] / 10, v->data(), v->size()),
					res2 = LYH::macala_result((*v)[0] / 10, v->data(), v->size());
				if (res1 != res2) {
					cout << "Seq: [";
					copy(v->cbegin(), v->cend(), ostream_iterator<int>(cout, ", "));
					cout << "]\tCYS: " << res1 << "\tLYH: " << res2 << endl;
					cout << "ERROR!" << endl;
					return 0;
				}
				if (res1 / 10000 < 3)
					Q.push(unique_ptr<vector<int> >(new vector<int>(*v)));
				v->pop_back();
			}
		Q.pop();
	}
	return 0;
}
