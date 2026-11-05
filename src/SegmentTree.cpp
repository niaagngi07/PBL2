#include "SegmentTree.h"
#include <algorithm>

using namespace std;

SegmentTree::SegmentTree(const vector<double>& data) {
    size = data.size();
    if (size > 0) {
        tree.assign(4 * size, 0.0);
        buildTree(data, 1, 0, size - 1);
    }
}

void SegmentTree::buildTree(const vector<double>& data, int node, int start, int end) {
    if (start == end) {
        tree[node] = data[start];
    } else {
        int mid = (start + end) / 2;
        buildTree(data, 2 * node, start, mid);
        buildTree(data, 2 * node + 1, mid + 1, end);
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }
}

double SegmentTree::queryRange(int node, int start, int end, int l, int r) {
    if (r < start || end < l) {
        return 0.0;
    }
    if (l <= start && end <= r) {
        return tree[node];
    }
    int mid = (start + end) / 2;
    double p1 = queryRange(2 * node, start, mid, l, r);
    double p2 = queryRange(2 * node + 1, mid + 1, end, l, r);
    return p1 + p2;
}

double SegmentTree::query(int left, int right) {
    if (size == 0 || left > right || left < 0 || right >= size) return 0.0;
    return queryRange(1, 0, size - 1, left, right);
}

void SegmentTree::updateValue(int node, int start, int end, int idx, double value) {
    if (start == end) {
        tree[node] = value;
    } else {
        int mid = (start + end) / 2;
        if (start <= idx && idx <= mid) {
            updateValue(2 * node, start, mid, idx, value);
        } else {
            updateValue(2 * node + 1, mid + 1, end, idx, value);
        }
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }
}

void SegmentTree::update(int index, double value) {
    if (index >= 0 && index < size) {
        updateValue(1, 0, size - 1, index, value);
    }
}
