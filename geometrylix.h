#ifndef __GEOMETRY_LIX_H__
#define __GEOMETRY_LIX_H__

#include <cmath>
#include <cassert>

//=============================================================================
// vec
//=============================================================================

template<int n, typename T> struct vec {
    T data[n] = {0};

    T& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    T  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n, typename T> vec<n,T> operator+(const vec<n,T> &lvec, const vec<n,T> &rvec) { 
    vec<n,T> ret = lvec;
    for (int i=0; i<n; i++) {
        ret[i] += rvec[i];
    }
    return ret;
}

template<int n, typename T> vec<n,T> operator-(const vec<n,T> &lvec, const vec<n,T> &rvec) { 
    vec<n,T> ret = lvec;
    for (int i=0; i<n; i++) {
        ret[i] -= rvec[i];
    }
    return ret;
}

template<int n, typename T> T operator*(const vec<n,T> &lvec, const vec<n,T> &rvec) { 
    T ret = 0;
    for (int i=0; i<n; i++) {
        ret += lvec[i]*rvec[i];
    }
    return ret;
}

template<int n, typename T> vec<n,T> operator*(const vec<n,T> &lvec, const float &r) { 
    vec<n,T> ret = lvec;
    for (int i=0; i<n; i++) {
        ret[i] *= r;
    }
    return ret;
}

template<int n, typename T> vec<n,T> operator*(const float &r, const vec<n,T> &lvec) { 
    return lvec*r;
}

template<int n, typename T> vec<n,T> operator/(const vec<n,T> &lvec, const float &r) { 
    vec<n,T> ret = lvec;
    for (int i=0; i<n; i++) {
        ret[i] /= r;
    }
    return ret;
}

template<typename T> vec<3,T> cross(const vec<3,T> &lvec, const vec<3,T> &rvec) { 
    return {lvec[1]*rvec[2]-lvec[2]*rvec[1], lvec[2]*rvec[0]-lvec[0]*rvec[2], lvec[0]*rvec[1]-lvec[1]*rvec[0]};
}

template<int n, typename T> T norm(const vec<n,T> &v) {
    return std::sqrt(v*v);
}

template<int n, typename T> vec<n,T> normalized(const vec<n,T> &v) {
    return v / norm(v);
}

template<typename T> struct vec<2,T>
{
    T x = 0, y = 0;
    T& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    T  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
};

template<typename T> struct vec<3,T>
{
    T x = 0, y = 0, z = 0;
    T& operator[](const int i)       { assert(i>=0 && i<3); return i>1 ? z : (i ? y : x); }
    T  operator[](const int i) const { 
        if (i<0 || i>=3) {
            assert(i>=0 && i<3);
        }
        assert(i>=0 && i<3); 
        return i>1 ? z : (i ? y : x); 
    }
};

template<typename T> struct vec<4,T>
{
    T x = 0, y = 0, z = 0, w = 0;
    T& operator[](const int i)       { assert(i>=0 && i<4); return i>1 ? (i==2 ? z : w) : (i ? y : x); }
    T  operator[](const int i) const { assert(i>=0 && i<4); return i>1 ? (i==2 ? z : w) : (i ? y : x); }
};

typedef vec<2,int> Vec2i;
typedef vec<2,float> Vec2f;
typedef vec<3,int> Vec3i;
typedef vec<3,float> Vec3f;
typedef vec<4,int> Vec4i;
typedef vec<4,float> Vec4f;

template <typename T> std::ostream& operator<<(std::ostream& s, vec<2,T>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <typename T> std::ostream& operator<<(std::ostream& s, vec<3,T>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

//=============================================================================
// mat
//=============================================================================
template<int nrows, int ncols, typename T> struct mat {
    vec<ncols,T> data[nrows] = {{}};

          vec<ncols,T>& operator[](const int i)       { assert(i>=0 && i<nrows); return data[i]; }
    const vec<ncols,T>& operator[](const int i) const { assert(i>=0 && i<nrows); return data[i]; }
};

template<int nrows, int ncols, typename T> mat<nrows,ncols,T> operator+(const mat<nrows,ncols,T> &lmat, const mat<nrows,ncols,T> &rmat) {
    mat<nrows,ncols,T> ret;
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            ret[i][j] = lmat[i][j] + rmat[i][j];
        }
    }

    return ret;
}

template<int nrows, int ncols, typename T> mat<nrows,ncols,T> operator-(const mat<nrows,ncols,T> &lmat, const mat<nrows,ncols,T> &rmat) {
    mat<nrows,ncols,T> ret;
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            ret[i][j] = lmat[i][j] - rmat[i][j];
        }
    }

    return ret;
}

template<int nrows, int ncols, typename T> mat<nrows,ncols,T> operator*(const mat<nrows,ncols,T> &lmat, const float &r) {
    mat<nrows,ncols,T> ret = lmat;
    for (int i=0; i<nrows; i++) {
        for (int j=0; j<ncols; j++) {
            ret[i][j] *= r;
        }
    }

    return ret;
}

template<int nrows, int ncols, typename T> mat<nrows,ncols,T> operator*(const float &r, const mat<nrows,ncols,T> &lmat) {
    return lmat*r;
}

template<int R1,int C1,int C2, typename T> mat<R1,C2,T> operator*(const mat<R1,C1,T> &lmat, const mat<C1,C2,T> &rmat) {
    mat<R1,C2,T> ret;
    for (int i=0; i<R1; i++) {
        for (int j=0; j<C2; j++) {
            for (int k=0; k<C1; k++) {
                ret[i][j] += lmat[i][k]*rmat[k][j];
            }
        }
    }

    return ret;
}

template<int nrows, int ncols, typename T> vec<ncols,T> operator*(const mat<nrows,ncols,T> &lmat, const vec<ncols,T> &rvec) {
    vec<ncols,T> ret;
    for (int i=ncols; i<ncols; i++) {
        ret[i] = lmat[i]*rvec;
    }

    return ret;
}

#endif //__GEOMETRY_LIX_H__