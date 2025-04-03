#pragma once
#include "./typ.hpp"
#define _ang(x) ((float)M_PI/180.0f) * x

namespace mth {
  template<typename T>
  inline T xorshift() {
    static T state = 1;
    T x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state = x;
  }
  
  template<typename T>
  u8 getPO2(T value) {
    u8 power = 0;
    while(value > 1) {
      power++;
      value >>= 1;
    }
    return power;
  }
  
  static inline u16 randInRange(const u16 range) {
    u16 x = mth::xorshift<u32>();
    u32 m = (u32)x * (u32)range;
    return (m >> 16);
  }
  
  template<typename T>
  inline T abs(const T v) {
    return v < 0 ? -v : v;
  }
  
  template <typename T>
  inline Vec4<T> add(const Vec4<T> va, const Vec4<T> vb) {
    return {
      vb.x + va.x,
      vb.y + va.y,
      vb.z + va.z,
      vb.w + va.w
    };
  }
  
  template <typename T>
  inline Vec4<T> scale(const Vec4<T> v, const T s) {
    return {
      v.x * s,
      v.y * s,
      v.z * s,
      v.w * s 
    };
  }
  
  template <typename T1, typename T2>
  inline Vec3<float> add(const T1 va, const T2 vb) {
    return {
      vb.x + va.x,
      vb.y + va.y,
      vb.z + va.z
    };
  }
  
  template <typename T>
  inline Vec4<T> sub(const Vec4<T> va, const Vec4<T> vb) {
    return {
      va.x - vb.x,
      va.y - vb.y,
      va.z - vb.z,
      va.w - vb.w 
    };
  }
  
  template <typename T1, typename T2>
  inline Vec3<float> sub(const T1 va, const T2 vb) {
    return {
      va.x - vb.x,
      va.y - vb.y,
      va.z - vb.z,
    };
  }
  
  template <typename T>
  float getNorm(const T v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
  }
  
  template<typename T>
  T getNormalized4(const T v) {
    const float norm = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
    return {
      v.x / norm, v.y / norm, v.z / norm, v.w / norm
    };
  }

  template<typename T>
  T getNormalized4(const v4 v) {
    const float norm = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z/* + v.w*v.w*/);
    return {
      v.x / norm, v.y / norm, v.z / norm
    };
  }
  
  template<typename T>
  v3 tov3(const T v) {
    return {
      v.x, v.y, v.z
    };
  }
  
  template<typename T>
  inline T getConjugate(const T qa) {
    return {
      -qa.x, -qa.y, -qa.z, qa.w
    };
  }

  template<typename T>
  inline T mulQuat(const T qa, const T qb) {
    return {
      qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y,
      qa.w * qb.y + qa.y * qb.w + qa.z * qb.x - qa.x * qb.z,
      qa.w * qb.z + qa.z * qb.w + qa.x * qb.y - qa.y * qb.x,
      qa.w * qb.w - qa.x * qb.x - qa.y * qb.y - qa.z * qb.z
    };
  }
  
  template<typename T>
  inline T getSandwichProduct(const T v, const T q) {
    return mulQuat(mulQuat(q, v), getConjugate(q)); //qvq-1
  }

  template<typename T>
  inline T getConjugate(const T* const qa) {
    return {
      -qa->x, -qa->y, -qa->z, qa->w
    };
  }

  template<typename T>
  inline T mulQuat(const T* const qa, const T* const qb) {
    return {
      qa->w * qb->x + qa->x * qb->w + qa->y * qb->z - qa->z * qb->y,
      qa->w * qb->y + qa->y * qb->w + qa->z * qb->x - qa->x * qb->z,
      qa->w * qb->z + qa->z * qb->w + qa->x * qb->y - qa->y * qb->x,
      qa->w * qb->w - qa->x * qb->x - qa->y * qb->y - qa->z * qb->z
    };
  }
  
  template<typename T>
  inline T mulQuat(const T* const qa, const v3* const qb) {
    return {
      qa->w * qb->x + qa->y * qb->z - qa->z * qb->y,
      qa->w * qb->y + qa->z * qb->x - qa->x * qb->z,
      qa->w * qb->z + qa->x * qb->y - qa->y * qb->x,
      - qa->x * qb->x - qa->y * qb->y - qa->z * qb->z
    };
  }

  template<typename T>
  inline v3 mulQuat3(const T qa, const T qb) {
    return {
      qa.w * qb.x + qa.x * qb.w + qa.y * qb.z - qa.z * qb.y,
      qa.w * qb.y + qa.y * qb.w + qa.z * qb.x - qa.x * qb.z,
      qa.w * qb.z + qa.z * qb.w + qa.x * qb.y - qa.y * qb.x
    };
  }
  
  //
  /*
  template<typename T>
  inline v3 getSandwichProduct(const v3 v, const T q) {
    return mulQuat3(mulQuat(&q, &v), getConjugate(&q));
  }
  */
  
  
  template<typename T>
  inline v3 getSandwichProduct(const v3* const v, const T* const q) {
    return mulQuat3(mulQuat(q, v), getConjugate(q));
  }
  
  template<typename T>
  T getOrientedQuat(const T axis) {
    const float a = axis.w / 2.0f;
    const float s = sinf(a);
    return {
      s * axis.x,
      s * axis.y,
      s * axis.z,
      cosf(a)
    };
  }
  
  template<typename T>
  T getReoriented(const T v, const T orientation) {
    return getSandwichProduct(v, getOrientedQuat(orientation));
  }
}
