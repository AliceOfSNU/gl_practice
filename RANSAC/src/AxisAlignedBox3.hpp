#ifndef __AxisAlignedBox3_hpp__
#define __AxisAlignedBox3_hpp__

#include "Common.hpp"
class AxisAlignedBox3 {
public:
	Vector3d low;
	Vector3d high;
	bool isNull;

	// constructor
	AxisAlignedBox3() : low(Vector3d::Zero()), high(Vector3d::Zero()), isNull(true){}
	AxisAlignedBox3(Vector3d _low, Vector3d _high) : low(_low), high(_high), isNull(false) {}
	
	

	// public methods

	void AddPoint(const Vector3d& pt);

	// RangeT methods
	bool ContainsPoint(const Vector3d& pt) const;
	bool Intersects(const AxisAlignedBox3& box) const;

	// misc
	void SetNull() {
		low = Vector3d::Zero();
		high = Vector3d::Zero();
		isNull = true;
	}

	void Print() {
		cout << "[AABB] from: \n" << low << ", to: \n" << high << endl;
	}
};

#endif