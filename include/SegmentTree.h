#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <string>

using namespace std;

class SegmentTree {
private:
    vector<double> tree;
    int size;

    void buildTree(const vector<double>& data, int node, int start, int end);
    double queryRange(int node, int start, int end, int l, int r);
    void updateValue(int node, int start, int end, int idx, double value);

public:
    SegmentTree(const vector<double>& data);
    double query(int left, int right);
    void update(int index, double value);
};

#endif // SEGMENT_TREE_H
