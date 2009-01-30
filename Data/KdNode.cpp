/*
 * KdNode.cpp
 *
 *  Created on: Jan 29, 2009
 *      Author: st
 */

#include "KdNode.h"
#include <math.h>
#include "../Data/constants.h"
void KdNode::computeMedian(const TriangleList &triangles, int size){
		int dimension = level%3;
		int testRatio = (int)(size/sqrt(size));
		std::list<double> decisionSet;
		int decisionSetSize = 0;
		TriangleList::const_iterator iter = triangles.begin();
		while(iter != triangles.end()){
			decisionSet.push_back((*iter)->v[0]->position[dimension]);
			decisionSetSize++;
			for(int j = 0; j<testRatio && iter != triangles.end(); j++){
				iter++;
			}
		}
		decisionSet.sort();
		std::list<double>::iterator median = decisionSet.begin();
		for(int i = 0; i < decisionSetSize/2; i++) median++;
		splitValue = (*median);
}

KdNode::KdNode(const TriangleList &triangles, int size, int level):
	level(level),
	left(NULL),
	right(NULL)
{
	if(size < (1 << level)){
		items = triangles;
		//we are a leaf
	}else{
		int dimension = level%3;
		computeMedian(triangles, size);
		int leftSize = 0;
		int rightSize = 0;
		TriangleList leftT;
		TriangleList rightT;

		for(TriangleList::const_iterator iter = triangles.begin();iter != triangles.end(); iter++){
			Triangle &t = *(*iter);

			if(t.min[dimension] < splitValue+EPSILON){
				leftT.push_back(&t);
				leftSize++;
				if(t.max[dimension] > splitValue - EPSILON){
					rightT.push_back(&t);
					rightSize++;
				}
			}else{
				rightT.push_back(&t);
				rightSize++;
				if(t.min[dimension] < splitValue){
					leftT.push_back(&t);
					leftSize++;
				}
			}
		}
		left = new KdNode(leftT, leftSize, level +1);
		right = new KdNode(rightT, rightSize, level +1);
	}

}

KdNode::~KdNode() {
	// TODO delete items
}

bool KdNode::intersect(Ray &ray){
	bool result = false;
	if(left){
		if(ray.origin[level%3] < splitValue)
		{
			if(ray.direction[level%3] < 0){
				result = left->intersect(ray);
			}else{
				if(left->intersect(ray)) result = true;
				if(right->intersect(ray)) result = true;
			}
		}else{
			if(ray.direction[level%3] < 0){
				if(right->intersect(ray)) result = true;
				if(left->intersect(ray)) result = true;
			}else{
				result = right->intersect(ray);
			}
		}
	}else{
		for(TriangleList::const_iterator iter = items.begin();iter != items.end(); iter++){
			Triangle &t = *(*iter);
			if(ray.intersect(t) == 1) result = true;
		}
	}
	return result;
}
