#pragma once

template <typename T>
struct TVec4
{
	union
	{
		T m[4];

		struct
		{
			T x, y, z, w;
		};

		struct
		{
			T r, g, b, a;
		};
	};

	TVec4() { }
	TVec4(T f) { x = y = z = w = f; }
	TVec4(T a, T b) { x = a; y = b; z = 0.0f; w = 0.0f; }
	TVec4(T a, T b, T c) { x = a; y = b; z = c; w = 0.0f; }
	TVec4(T a, T b, T c, T d) { x = a; y = b; z = c; w = d; }

	inline void setAll(T f) { x = y = z = w = f; }

	// simple menthods: operates on x, y
	inline void add2(const TVec4 & a) { x += a.x; y += a.y; }
	inline void sub2(const TVec4 & a) { x -= a.x; y -= a.y; }
	inline void mul2(const T &v)   { x *= v; y *= v; }
	inline void set2(T a, T b) { x = a; y = b; }

	static inline void mix2(const TVec4 &a, const TVec4 &b, T t, TVec4 &c)
	{
		c.x = a.x*(1.0f - t) + b.x*t;
		c.y = a.y*(1.0f - t) + b.y*t;
	}

	// simple menthods: operates on x, y, z
	inline void add3(const TVec4 & a) { x += a.x; y += a.y; z += a.z; }
	inline void sub3(const TVec4 & a) { x -= a.x; y -= a.y; z -= a.z; }
	inline void mul3(const T &v)   { x *= v; y *= v; z *= v; }
	inline void set3(T a, T b, T c) { x = a; y = b; z = c; }
	
	static inline void mix3(const TVec4 &a, const TVec4 &b, T t, TVec4 &c)
	{
		c.x = a.x*(1.0f - t) + b.x*t;
		c.y = a.y*(1.0f - t) + b.y*t;
		c.z = a.z*(1.0f - t) + b.z*t;
	}

	// simple menthods: operates on x, y, z, w
	inline void add4(const TVec4 & a) { x += a.x; y += a.y; z += a.z; w += a.w; }
	inline void sub4(const TVec4 & a) { x -= a.x; y -= a.y; z -= a.z; w -= a.w; }
	inline void mul4(const T &v)   { x *= v; y *= v; z *= v; w *= v; }
	inline void set4(T a, T b, T c, T d) { x = a; y = b; z = c; w = d; }

	static inline void mix4(const TVec4 &a, const TVec4 &b, T t, TVec4 &c)
	{
		c.x = a.x*(1.0 - t) + b.x*t;
		c.y = a.y*(1.0 - t) + b.y*t;
		c.z = a.z*(1.0 - t) + b.z*t;
		c.w = a.w*(1.0 - t) + b.w*t;
	}
};
