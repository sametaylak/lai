#pragma once

#include "defines.h"

typedef union vec2_u {
  f32 elements[2];

  struct {
    union {
      f32 x, r, s, u;
    };
    union {
      f32 y, g, t, v;
    };
  };
} vec2;

typedef union vec3_u {
  f32 elements[3];

  struct {
    union {
      f32 x, r, s, u;
    };
    union {
      f32 y, g, t, v;
    };
    union {
      f32 z, b, p, w;
    };
  };
} vec3;

typedef union vec4_u {
#ifdef LAI_USE_SIMD
  // used for SIMD operationgs;
  alignas(16) __m128 data;
#endif

  alignas(16) f32 elements[4];
  struct {
    union {
      f32 x, r, s;
    };
    union {
      f32 y, g, t;
    };
    union {
      f32 z, b, p;
    };
    union {
      f32 w, a, q;
    };
  };
} vec4;

typedef vec4 quat;

typedef union mat4_u {
  alignas(16) f32 data[16];
#ifdef LAI_USE_SIMD
  // used for SIMD operations;
  alignas(16) f32 rows[16];
#endif
} mat4;