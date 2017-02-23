#pragma once
#include "Math.h"

struct LimitedList {
	struct Node {
		vec2f data;
		Node *mNext, *mPrev;
		Node(vec2f _d) : data(_d) {}
	};

private:
	Node *mHead, *mTail;
	unsigned int mSize, mLimit = 9;

public:
	bool IsFull();
	unsigned int GetSize();
	unsigned int GetLimit();
	void SetLimit(unsigned int _size);
	void AddHead(vec2f _node);
	vec2f& operator[](unsigned int _index);
	void Clear();
	~LimitedList();
};