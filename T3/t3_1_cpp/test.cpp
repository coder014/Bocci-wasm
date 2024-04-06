#include <set>
#include <vector>
#include <chrono>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <time.h>
using namespace std;

struct situation
{
    unsigned char map1[8];
    unsigned char map2[8];
    unsigned char nextrole;
    unsigned char map1count;
    unsigned char map2count;
    unsigned char map1void;
    unsigned char map2void;
};

#define MAX 1
#define CHANCE 2
#define RANDOM 3

struct openode
{
    situation s;
    unsigned char bestpos;
    double value;
    map<unsigned char, openode *> children;
};

vector<openode *> sources;

void free_sources()
{
    for (auto &ptr : sources)
    {
        delete ptr;
    }
    sources.clear();
}

bool checkend(openode &node)
{
    int count1 = 0, count2 = 0;
    int void1 = 0, void2 = 0;
    for (int i = 1; i <= 6; i++)
    {
        count1 += node.s.map1[i];
        count2 += node.s.map2[i];
        if (node.s.map1[i] == 0)
            void1++;
        if (node.s.map2[i] == 0)
            void2++;
    }
    node.s.map1count = count1;
    node.s.map2count = count2;
    node.s.map1void = void1;
    node.s.map2void = void2;

    auto &s = node.s;
    if (s.map1count * s.map2count == 0)
    {
        s.map1[7] += s.map1count;
        s.map2[7] += s.map2count;
        for (int i = 1; i <= 6; i++)
        {
            s.map1[i] = 0;
            s.map2[i] = 0;
        }
        s.map1count = 0;
        s.map2count = 0;
    }
    return s.map1count * s.map2count == 0;
}

openode *operate_on(int pos, openode &orinode)
{
    auto *node = new openode;
    sources.push_back(node);
    for (int i = 1; i <= 7; i++)
    {
        node->s.map1[i] = orinode.s.map1[i];
        node->s.map2[i] = orinode.s.map2[i];
    }
    node->s.nextrole = orinode.s.nextrole;

    int role = node->s.nextrole;
    unsigned char *rolemap = (role == 1) ? node->s.map1 : node->s.map2;
    unsigned char *opemap = (role == 1) ? node->s.map2 : node->s.map1;
    int take = rolemap[pos];
    rolemap[pos] = 0;

    unsigned char *curmap = rolemap;
    int posto = pos;
    while (take != 0)
    {
        posto++;
        if (posto == 7 && curmap == opemap)
        {
            posto = 1;
            curmap = rolemap;
        }
        else if (posto == 8 && curmap == rolemap)
        {
            posto = 1;
            curmap = opemap;
        }
        curmap[posto]++;
        take--;
    }
    if (curmap == rolemap && posto < 7 && curmap[posto] == 1 && opemap[7 - posto] > 0)
    {
        int oppo = opemap[7 - posto];
        opemap[7 - posto] = 0;
        curmap[posto] = 0;
        curmap[7] += (oppo + 1);
    }
    if (posto == 7 && curmap == rolemap)
    {
        node->s.nextrole = role;
    }
    else
    {
        node->s.nextrole = (role == 1) ? 2 : 1;
    }

    checkend(*node);

    return node;
}

set<int> get_valid_operate(openode &node)
{
    set<int> res;
    unsigned char *curmap = node.s.nextrole == 1 ? node.s.map1 : node.s.map2;
    for (int i = 1; i <= 6; i++)
    {
        if (curmap[i] > 0)
        {
            res.insert(i);
        }
    }
    return res;
}

openode *init_map(int flag, openode *newnode)
{
    openode &node = *newnode;
    for (int j = 1; j <= 7; j++)
    {
        if (j != 7)
        {
            node.s.map1[j] = 4;
            node.s.map2[j] = 4;
        }
        else
        {
            node.s.map1[j] = 0;
            node.s.map2[j] = 0;
        }
    }
    node.s.nextrole = flag;
    node.s.map1count = 24;
    node.s.map2count = 24;
    return newnode;
}

openode *init_map(int flag)
{
    auto newnode = new openode;
    sources.push_back(newnode);
    return init_map(flag, newnode);
}

#include <emscripten/val.h>
#include <emscripten/bind.h>
using namespace emscripten;

int retarr[15];

void set_return(openode &node, unsigned char data)
{
    for (int i = 0; i <= 6; i++)
    {
        retarr[i] = (int)node.s.map1[i + 1];
        retarr[i + 7] = (int)node.s.map2[i + 1];
    }
    retarr[14]= (int)data;
}

extern "C" int* mancala_board(int flag, int seqptr, int size)
{
    int *seq = (int *)seqptr;
    openode node = *init_map(seq[0] / 10);
    for (int i = 0; i < size - 1; i++)
    {
        int role = seq[i] / 10;
        int pos = seq[i] % 10;
        node = *operate_on(pos, node);
    }
    int role = seq[size - 1] / 10;
    int pos = seq[size - 1] % 10;
    auto validpos = get_valid_operate(node);
    if (checkend(node) ||
        ((role != node.s.nextrole) || (!validpos.count(pos))))
    {
        free_sources();
        set_return(node,
                   flag == 1 ? 200 + 2 * node.s.map1[7] - 48 : 200 + 48 - 2 * node.s.map2[7]);
        return retarr;
    }

    node = *operate_on(pos, node);
    free_sources();
    if (checkend(node))
    {
        cout << "end" << endl;
        set_return(node, 200 + node.s.map1[7] - node.s.map2[7]);
    }
    else
    {
        set_return(node, node.s.nextrole);
    }
    return retarr;
}
