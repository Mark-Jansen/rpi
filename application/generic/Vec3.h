#ifndef GENERIC_VEC3_H
#define GENERIC_VEC3_H

#include <ostream>

// very incomplete vector class, just implemented what i needed...

template<typename VEC_T>
struct Vec3
{
	Vec3() : x( VEC_T() ), y( VEC_T() ), z( VEC_T() ) {;}
	Vec3( VEC_T X, VEC_T Y, VEC_T Z ) : x( X ), y( Y ), z( Z ) {;}

	Vec3 operator + ( const Vec3 &o ) const
	{
		return Vec3( x + o.x, y + o.y, z + o.z );
	}

	Vec3& operator += ( const Vec3 &o )
	{
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}

	Vec3 operator - ( const Vec3 &o ) const
	{
		return Vec3( x - o.x, y - o.y, z - o.z );
	}

	template<class O_T> Vec3 operator * ( const O_T &o ) const
	{
		return Vec3( x * (VEC_T)o, y * (VEC_T)o, z * (VEC_T)o );
	}

	template<class O_T> Vec3& operator /= ( const O_T &o )
	{
		x /= (VEC_T)o;
		y /= (VEC_T)o;
		z /= (VEC_T)o;
		return *this;
	}

	VEC_T x, y, z;
};

typedef Vec3<float> Vec3f;


template<typename VEC_T>
std::ostream& operator <<( std::ostream& strm, const Vec3<VEC_T>& v )
{
	strm << "[x=" << v.x << ",y=" << v.y << ",z=" << v.z << "]";
	return strm;
}


#endif // GENERIC_VEC3_H
