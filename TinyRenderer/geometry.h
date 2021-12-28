//
//  geometry.h
//  TinyRenderer
//
//  Created by Sherwin Rad on 12/27/21.
//

#ifndef geometry_h
#define geometry_h

#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

template<size_t DimCols, size_t DimRows, typename T> class Matrix;

template <size_t DIM, typename T>
struct Vec{
    Vec()
    {
        for(size_t i = DIM; i--; data_[i] = T()); //data_[i] = T() calls constructor for the type? Aka float or int?
    }
    
    T& operator[](const size_t i)
    {
        assert(i < DIM);
        return data_[i];
    }
    
    const T& operator[](const size_t i) const
    {
        assert(i < DIM);
        return data_[i];
    }
private:
    T data_[DIM];
};

template <typename T>
struct Vec<2, T>
{
    Vec() : x(T()), y(T()) {}
    Vec(T _x, T _y) : x(_x), y(_y){}
    template <class U> Vec<2, T>(const Vec<2,U> &v);
    
    T& operator[](const size_t i)
    {
        assert(i<2);
        return i <= 0 ? x : y;
    }
    
    const T& operator[](const size_t i) const
    {
        assert(i < 2);
        return i <=0 ? x : y;
    }
    
    T x,y;
};

template <typename T>
struct Vec<3,T>
{
    Vec() : x(T()), y(T()), z(T()) {}
    Vec(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    template <class U> Vec<3, T>(const Vec<3,U> &v);
    
    T& operator[](const size_t i)
    {
        assert(i<3);
        return i <= 0 ? x : (i==1 ? y : z);
    }
    
    const T& operator[](const size_t i) const
    {
        assert(i < 2);
        return i <= 0 ? x : (i==1 ? y : z);
    }
    float norm()
    {
        return std::sqrt(x*x+y*y+z*z);
    }
    
    Vec<3,T>& normalize(T l=1)
    {
        *this = (*this)*(l/norm());
        return *this;
    }
    
    T x,y,z;
};

//---------------------------------------------------------------------------------------------

// Multiplication operator
template<size_t vectorSize, typename T>
T operator*(const Vec<vectorSize, T> &lhs, const Vec<vectorSize, T> &rhs)
{
    T ret = T();
    for(size_t i = vectorSize; i--; ret+= lhs[i]*rhs[i]);
    return ret;
}

// Addition operator
template<size_t vectorSize, typename T>
T operator+(const Vec<vectorSize, T> &lhs, const Vec<vectorSize, T> &rhs)
{
    for(size_t i = vectorSize; i--; lhs[i]+=rhs[i]);
    return lhs;
}

// Subtraction operator
template<size_t vectorSize, typename T>
T operator-(const Vec<vectorSize, T> &lhs, const Vec<vectorSize, T> &rhs)
{
    for(size_t i = vectorSize; i--; lhs[i]-=rhs[i]);
    return lhs;
}

// *=
template<size_t vectorSize, typename T, typename U>
Vec<vectorSize, T> operator*(Vec<vectorSize,T> lhs, const U &rhs)
{
    for(size_t i = vectorSize; i--; lhs[i]*=rhs);
    return lhs;
}

// /=
template<size_t vectorSize, typename T, typename U>
Vec<vectorSize, T> operator/(Vec<vectorSize,T> lhs, const U &rhs)
{
    for(size_t i = vectorSize; i--; lhs[i]/=rhs);
    return lhs;
}

//Defaults extra size to 1
template<size_t len, size_t vecSize, typename T>
Vec<len, T> embed(const Vec<vecSize, T> &v, T fill=1)
{
    Vec<len, T> ret;
    for(size_t i = len; i--; ret[i]=(i < vecSize ? v[i] : fill));
    return ret;
}

//To call this we go proj<len, vecSize, T>(v);
template<size_t len, size_t vecSize, typename T>
Vec<len, T> proj(const Vec<vecSize, T> &v)
{
    Vec<len,T> ret;
    for(size_t i = len; i--; ret[i]=v[i]);
    return ret;
}

template<typename T>
Vec<3,T> cross(Vec<3,T> v1, Vec<3,T> v2)
{
    return Vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y * v2.x);
}

template<size_t vecSize, typename T>
std::ostream& operator<<(std::ostream &out, Vec<vecSize, T> &v)
{
    for(unsigned int i =0; i < vecSize; i++)
    {
        out << v[i] << " ";
    }
    
    return out;
}

//---------------------------------------------------------------------------------------------
//Matrix stuff
template<size_t size, typename T>
struct dt
{
    static T det(const Matrix<size, size, T> &src)
    {
        T ret = 0;
        for(size_t i=size; i--; ret+= src[0][i]*src.cofactor(0,i));
        return ret;
    }
};

//if matrix size is 1
template<typename T>
struct dt<1, T>
{
    static T det(const Matrix<1,1,T> &src)
    {
        return src[0][0];
    }
};

//---------------------------------------------------------------------------------------------
template<size_t rowSize, size_t colSize, typename T>
class Matrix
{
    Vec<colSize, T> rows[rowSize];
public:
    Matrix(){}
    
    //set
    Vec<colSize, T>& operator[] (const size_t idx)
    {
        assert(idx < rowSize);
        return rows[idx];
    }
    //get
    const Vec<colSize, T>& operator[] (const size_t idx) const
    {
        assert(idx < rowSize);
        return rows[idx];
    }
    
    Vec<rowSize, T> col(const size_t idx) const
    {
        assert(idx < colSize);
        Vec<rowSize, T> ret;
        for(size_t i = rowSize; i--; ret[i]=rows[i][idx]);
        return ret;
    }
    
    void setCol(size_t idx, Vec<rowSize, T> v)
    {
        assert(idx < colSize);
        for(size_t i = rowSize; i--; rows[i][idx] = v[i]);
    }
    
    static Matrix<rowSize, colSize, T> identity()
    {
        Matrix<rowSize, colSize, T> ret;
        for(size_t i = rowSize; i--;)
        {
            for(size_t j= colSize; j--; ret[i][j] = (i==j));
        }
        return ret;
    }
    
    T det() const
    {
        return dt<colSize, T>::det(*this);
    }
    
    Matrix<rowSize-1, colSize-1, T> getMinor(size_t row, size_t col) const
    {
        Matrix<rowSize-1, colSize-1, T> ret;
        for(size_t i = rowSize-1; i--;)
        {
            for(size_t j=colSize-1; j--; ret[i][j]=rows[i<row?i : i+1][j < col ? j : j+1]);
        }
        return ret;
    }
    
    T cofactor(size_t row, size_t col) const
    {
        return getMinor(row, col).det() * ((row+col)%2 ? -1 : 1);
    }
    
    Matrix<rowSize, colSize, T> adjugate() const
    {
        Matrix<rowSize, colSize, T> ret;
        for(size_t i = rowSize;i--;)
        {
            for(size_t j=colSize;j--; ret[i][j]=cofactor(i,j));
        }
        return ret;
    }
    
    Matrix<rowSize, colSize, T> invertTranspose()
    {
        Matrix<rowSize, colSize, T> ret = adjugate();
        
        T tmp = ret[0]*rows[0];
        return ret/tmp;
    }
    
    Matrix<rowSize, colSize, T> invert()
    {
        return invertTranspose().transpose();
    }
    
    Matrix<rowSize, colSize, T> transpose()
    {
        Matrix<rowSize, colSize, T> ret;
        for(size_t i = colSize; i--; ret[i]=this->col(i));
        return ret;
    }
};

//---------------------------------------------------------------------------------------------

template<size_t rowSize, size_t colSize, typename T>
Vec<rowSize, T> operator*(const Matrix<rowSize, colSize, T> &lhs, const Vec<colSize, T> &rhs)
{
    Vec<rowSize, T> ret;
    for(size_t i = rowSize; i--; ret[i] = lhs[i]*rhs);
    return ret;
}

template<size_t R1,size_t C1,size_t C2,typename T>
Matrix<R1,C2,T> operator*(const Matrix<R1,C1,T>& lhs, const Matrix<C1,C2,T>& rhs)
{
    Matrix<R1,C2,T> result;
    for (size_t i=R1; i--; )
        for (size_t j=C2; j--; result[i][j]=lhs[i]*rhs.col(j));
    return result;
}

template<size_t rowSize,size_t colSize,typename T>
Matrix<colSize,rowSize,T> operator/(Matrix<rowSize,colSize,T> lhs, const T& rhs)
{
    for (size_t i=rowSize; i--; lhs[i]=lhs[i]/rhs);
    return lhs;
}

template <size_t rowSize,size_t colSize,class T>
std::ostream& operator<<(std::ostream& out, Matrix<rowSize,colSize,T>& m)
{
    for (size_t i=0; i<rowSize; i++) out << m[i] << std::endl;
    return out;
}

//---------------------------------------------------------------------------------------------
typedef Vec<2,      float>  Vec2f;
typedef Vec<2,      int>    Vec2i;
typedef Vec<3,      float>  Vec3f;
typedef Vec<3,      int>    Vec3i;
typedef Vec<4,      float>  Vec4f;
typedef Matrix<4, 4,float>  Matrix4f;

#endif /* geometry_h */
