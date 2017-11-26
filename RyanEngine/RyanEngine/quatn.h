#pragma once
#include "vec3_math.h"
#include "mat4x4_math.h"

class quatn
{
public:
	float s;
	vec3 v;
	quatn ( void );
	quatn ( float, vec3 );

	quatn ( vec3 v, float angle );
	quatn ( float x, float y, float z );

	//vec3 rotate(vec3& v);

	quatn conjugate ();
	quatn inverse ();
	quatn unit ();
	float mag ();
	vec3 toEuler ();

	quatn operator+( quatn& q );//add
	quatn operator-( quatn& q );//subtract 
	quatn operator*( quatn& q );//multiply
	vec3 operator*( vec3& vec );//multiply vec
	quatn operator*( float k );//multiply
	quatn& operator*=( quatn& q );//multiply(=)

	quatn lerp ( quatn& end, float t );
	mat4x4 toRotMatrix ();
	~quatn ( void );
};

inline
quatn quatn::conjugate ()
{
	return quatn ( s, -v );
}

inline
quatn quatn::inverse ()
{
	float m = mag ();
	return quatn ( s / m, vec3 ( -v.x / m, -v.y / m, -v.z / m ) );
}

inline
quatn quatn::unit ()
{
	float m = mag ();
	return quatn ( s / m, vec3 ( v.x / m, v.y / m, v.z / m ) );
}

inline
float quatn::mag ()
{
	return sqrt ( s*s + v.x*v.x + v.y*v.y + v.z*v.z );
}

inline
quatn quatn::operator+( quatn& q )
{
	return quatn ( s + q.s, vec3 ( v.x + q.v.x, v.y + q.v.y, v.z + q.v.z ) );
}

inline
quatn quatn::operator-( quatn& q )
{
	return quatn ( s - q.s, vec3 ( v.x - q.v.x, v.y - q.v.y, v.z - q.v.z ) );
}

inline
quatn quatn::operator*( quatn& q )
{
	return quatn ( s*q.s - v.dot ( q.v ), vec3 ( q.v*s + v*q.s + v.cross ( q.v ) ) );
}

inline
quatn quatn::operator*( float k )
{
	return quatn ( s*k, vec3 ( v.x*k, v.y*k, v.z*k ) );
}

inline
quatn& quatn::operator*=( quatn& q )
{
	return *this = ((*this) * q);
}

inline
quatn quatn::lerp ( quatn& dest, float t )
{
	return ((*this)*(1 - t) + dest*t).unit ();
}

inline
vec3 quatn::operator*( vec3& vec )
{
	vec3 t = v.cross ( vec ) * 2;
	return vec + t * s + v.cross ( t );
}