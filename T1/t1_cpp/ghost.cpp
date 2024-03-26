#include <unordered_map>
using namespace std;

extern "C" int bocciShutUp(int flag, int *_seq, int size) {
	unique_ptr<int[]> seq(_seq);
	unordered_map<int, int> stat;
	for (auto i = 0; i < size; i++) {
		int num = seq[i];
		if (num / 10 != flag) {
			continue;
		}
		stat[num]++;
	}

	int max_num = 0, max_v = 0;
	for (auto &v : stat) {
		if (v.second > max_v) {
			max_num = v.first;
			max_v = v.second;
		} else if (v.second == max_v) {
			max_num = 10;
		}
	}
	return max_num;
}
