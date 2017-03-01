//#include "stdafx.h"
#include "RenderSet.h"
#include "RenderNode.h"
#include "RenderContext.h"
#include "RenderShape.h"
#include "../Common/Breakpoint.h"
#include "../Common/Logger.h"
#include <algorithm>

namespace Epoch {
	void RenderSet::AddNode(RenderNode *_node, RenderContext* _rc) {
		if (_node->mType == RenderNode::RenderNodeType::Shape) {
			if (((RenderShape*)_node)->mIndexCount == 0) {
				SystemLogger::Error() << "Attempting to set render shape with 0 indices!" << std::endl;
				Debug::SetBreakpoint();
			}
		}
		if (mTail == nullptr) {
			mContexts.push_back(_rc);
			mHead = (RenderNode*)_rc;
			mTail = _node;
			_rc->mNext = _node;
			_rc->mPrevious = nullptr;
			_node->mNext = nullptr;
			_node->mPrevious = _rc;
		} else {
			bool foundContext = false;
			for (auto it = mContexts.begin(); it != mContexts.end(); ++it) {
				if (*(*it) == *_rc) {
					foundContext = true;
					_node->mNext = (*it)->mNext;
					_node->mPrevious = (*it);
					_node->mNext->mPrevious = _node;
					(*it)->mNext = _node;
					break;
				}
			}
			if (!foundContext) {
				// Insert a new context and mesh.
				_node->mNext = nullptr;
				_node->mPrevious = _rc;
				_rc->mNext = _node;
				_rc->mPrevious = mTail;
				mTail->mNext = _rc;
				mTail = _node;
				mContexts.push_back(_rc);
			}
		}
		mSize++;
	}

	void RenderSet::RemoveNode(RenderNode * _node) {
		// The render set does NOT delete anything, it is not technically a container.
		// It simply
		RenderNode *cur = mHead;
		while (cur != nullptr) {
			if (cur == _node) {
				if (cur->mPrevious) {
					cur->mPrevious->mNext = cur->mNext;
					if (cur->mNext) {
						cur->mNext->mPrevious = cur->mPrevious;
					}
				} else {
					mHead = cur->mNext;
					if (mHead) {
						mHead->mPrevious = nullptr;
					} else {
						mTail = nullptr;
					}
				}
				break;
			}
			cur = cur->mNext;
		}
	}

	void RenderSet::RemoveContext(std::vector<RenderContext*>::iterator _it) {
		RenderContext* rc = (*_it);
		if (rc->mNext && rc->mNext->mNext) {
			// This node has two valid children.
			if(rc->mNext->mType == RenderNode::RenderNodeType::Shape && rc->mNext->mNext->mType == RenderNode::RenderNodeType::Shape) {
				// Both children are shapes.
				RenderShape *child = (RenderShape*)rc->mNext, *grandchild = (RenderShape*)rc->mNext->mNext;
				if (rc != &child->mContext) {
					(*_it) = &child->mContext;
				} else if (rc != &grandchild->mContext) {
					(*_it) = &grandchild->mContext;
				} else {
					// Something's fucky.
					Debug::SetBreakpoint();
				}
			} else {
				// Both children are not shapes
				mContexts.erase(_it);
			}
		} else {
			// This context does not have two children
			mContexts.erase(_it);
		}
		RemoveNode(rc);
	}

	void RenderSet::RemoveShape(RenderShape *_node) {
		auto it = std::find(mContexts.begin(), mContexts.end(), &_node->mContext);
		if (it != mContexts.end()) {
			// This shape's context is in the list
			RemoveContext(it);
		}
		RemoveNode(_node);
		mSize--;
	}
	
	void RenderSet::ClearSet() {
		mHead = mTail = nullptr;
		mContexts.clear();
	}
	
	const RenderNode * RenderSet::GetHead() {
		return mHead;
	}

	RenderSet::~RenderSet() {
		mContexts.clear();
		while (mHead) {
			SystemLogger::Warn() << "Nodes are being removed from the RenderSet in its destructor. This means nodes are no longer being rendered, but are still in memory, likely implying a leak somewhere." << std::endl;
			RenderNode *n = mHead;
			mHead = mHead->mNext;
			//delete n;
		}
	}

}