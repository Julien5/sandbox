#!/usr/bin/env python3

import pickle;
import os;
from board import Board;

visits=0;
def walk_children(b,tree=None):
    if tree is None:
        tree=dict();
        
    global visits;
    visits = visits + 1;
    if visits % 1000 == 0:
        percent=100*len(tree)/765;
        print('progress:{percent:{width}.1f}% visits:{visits:{widthv}}'.format(percent=percent, width=3, visits=visits, widthv=6),end="\r");
    if not b in tree:
        tree[b.normalize()]=set(); 
    for c in b.children():
        tree[b.normalize()].add(c.normalize());
        walk_children(c,tree);
    return tree;

def walk_score(b,tree,score=None):
    if score is None:
        score=dict();
    assert(b.normalize()==b);
    if b in score:
        return score;
     
    s=b.getscore();
    if not s is None:
        score[b]=s;
        return score;
 
    C=tree[b];
    assert(C);
    S = [walk_score(c,tree,score)[c] for c in C];
    if b.xturn():
        score[b]=max(S);
    else:
        score[b]=min(S);
    return score;

treetxt = "tree.pickle";
scoretxt = "score.pickle";

def build():
    if not os.path.exists(treetxt):
        print("compute tree");
        tree=walk_children(Board());
        pickle.dump(tree,open(treetxt,'wb'));

    print("load tree");
    tree = pickle.load(open(treetxt,'rb'));
    print("loaded tree");
    print("number of nodes:",len(tree));
   
    if not os.path.exists(scoretxt):
        print("compute scores");
        score=walk_score(Board(),tree);
        pickle.dump(score,open(scoretxt,'wb'));
             
    score = pickle.load(open(scoretxt,'rb'));
    print("loaded score");
    print("number of scores:",len(score));
    return tree,score;
    
if __name__ == '__main__':
    build();
