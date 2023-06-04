#include "AxisAlignedBox3.hpp"


void AxisAlignedBox3::AddPoint(const Vector3d& pt)
{

	if (isNull) {
		low = pt; high = pt;
		isNull = false;
		return;
	}
	Vector3d new_high, new_low;

	for (int i = 0; i < 3; ++i) {
		new_high[i] = max(high[i], pt[i]);
		new_low[i] = min(low[i], pt[i]);
	}
	low = new_low; high = new_high;
}

bool AxisAlignedBox3::ContainsPoint(const Vector3d& pt) const
{
	bool ret = true;
	for (int i = 0; i < 3; ++i) {
		ret &= (pt[i] > low[i]) && (pt[i] < high[i]);
	}
	return ret;
}

bool AxisAlignedBox3::Intersects(const AxisAlignedBox3& box) const {
	bool res = true;
	for (int ax = 0; ax < 3; ++ax) {
		res &= !(low[ax] > box.high[ax] || high[ax] < box.low[ax]);
	}
	return res;
}