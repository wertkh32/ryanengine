#include "quatn.h"


quatn::quatn ()
{
	s = 1;
	v.x = v.y = v.z = 0;

}

quatn::quatn ( float sca, vec3 vec )
{
	s = sca;
	v = vec;
}

quatn::quatn ( vec3 q, float angle )
{
	s = cos ( angle / 2 );
	v = q*sin ( angle / 2 );
}

quatn::quatn ( float x, float y, float z )
{
	// euler angle to quat
	// make 3 quat on each axis, compose them

	quatn xrot = quatn ( vec3 ( 1, 0, 0 ), x );
	quatn yrot = quatn ( vec3 ( 0, 1, 0 ), y );
	quatn zrot = quatn ( vec3 ( 0, 0, 1 ), z );

	*this = zrot * yrot * xrot;
}

vec3 quatn::toEuler ()
{
	return vec3 (
		atan2 ( 2 * v.x*s - 2 * v.y*v.z, 1 - 2 * v.x*v.x - 2 * v.z*v.z ),
		atan2 ( 2 * v.y*s - 2 * v.x*v.z, 1 - 2 * v.y*v.y - 2 * v.z*v.z ),
		asin ( 2 * v.x*v.y + 2 * v.z*s ) );
}

mat4x4 quatn::toRotMatrix ()
{
	float x2 = v.x * v.x;
	float y2 = v.y * v.y;
	float z2 = v.z * v.z;
	float xy = v.x * v.y;
	float xz = v.x * v.z;
	float yz = v.y * v.z;
	float wx = s * v.x;
	float wy = s * v.y;
	float wz = s * v.z;

	return mat4x4 ( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
		2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
		2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );
}


quatn::~quatn ( void )
{
}
