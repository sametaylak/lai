#pragma once

#include "defines.h"
#include "math/math_types.h"

#define LAI_PI 3.14159265358979323846f
#define LAI_PI_2 2.0f * LAI_PI
#define LAI_HALF_PI 0.5f * LAI_PI
#define LAI_QUARTER_PI 0.25f * LAI_PI
#define LAI_ONE_OVER_PI 1.0f / LAI_PI
#define LAI_ONE_OVER_TWO_PI 1.0f / LAI_PI_2
#define LAI_SQRT_TWO 1.41421356237309504880f
#define LAI_SQRT_THREE 1.73205080756887729352f
#define LAI_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define LAI_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define LAI_DEG2RAD_MULTIPLIER LAI_PI / 180.0f
#define LAI_RAD2DEG_MULTIPLIER 180.0f / LAI_PI
#define LAI_SEC_TO_MS_MULTIPLIER 1000.0f
#define LAI_MS_TO_SEC_MULTIPLIER 0.001f
#define LAI_INFINITY 1e30f
#define LAI_FLOAT_EPSILON 1.192092896e-07f

f32 lai_sin(f32 x);
f32 lai_cos(f32 x);
f32 lai_tan(f32 x);
f32 lai_acos(f32 x);
f32 lai_sqrt(f32 x);
f32 lai_abs(f32 x);

i32 lai_random();
i32 lai_random_in_range(i32 min, i32 max);

f32 lai_frandom();
f32 lai_frandom_in_range(f32 min, f32 max);

static inline bool is_power_of_two(u64 value) {
  return (value != 0) && ((value & (value - 1)) == 0);
}

static inline vec2 vec2_create(f32 x, f32 y) {
  vec2 out_vector;
  out_vector.x = x;
  out_vector.y = y;
  return out_vector;
}

static inline vec2 vec2_zero() { return (vec2){0.0f, 0.0f}; }

static inline vec2 vec2_one() { return (vec2){1.0f, 1.0f}; }

static inline vec2 vec2_up() { return (vec2){0.0f, 1.0f}; }

static inline vec2 vec2_down() { return (vec2){0.0f, -1.0f}; }

static inline vec2 vec2_left() { return (vec2){-1.0f, 0.0f}; }

static inline vec2 vec2_right() { return (vec2){1.0f, 0.0f}; }

static inline vec2 vec2_add(vec2 vector_0, vec2 vector_1) {
  return (vec2){vector_0.x + vector_1.x, vector_0.y + vector_1.y};
}

static inline vec2 vec2_sub(vec2 vector_0, vec2 vector_1) {
  return (vec2){vector_0.x - vector_1.x, vector_0.y - vector_1.y};
}

static inline vec2 vec2_mul(vec2 vector_0, vec2 vector_1) {
  return (vec2){vector_0.x * vector_1.x, vector_0.y * vector_1.y};
}

static inline vec2 vec2_div(vec2 vector_0, vec2 vector_1) {
  return (vec2){vector_0.x / vector_1.x, vector_0.y / vector_1.y};
}

static inline f32 vec2_length_squared(vec2 vector) {
  return vector.x * vector.x + vector.y * vector.y;
}

static inline f32 vec2_length(vec2 vector) {
  return lai_sqrt(vec2_length_squared(vector));
}

static inline void vec2_normalize(vec2 *vector) {
  const f32 length = vec2_length(*vector);
  vector->x /= length;
  vector->y /= length;
}

static inline bool vec2_compare(vec2 vector_0, vec2 vector_1, f32 tolerance) {
  if (lai_abs(vector_0.x - vector_1.x) > tolerance) {
    return false;
  }

  if (lai_abs(vector_0.y - vector_1.y) > tolerance) {
    return false;
  }

  return true;
}

static inline f32 vec2_distance(vec2 vector_0, vec2 vector_1) {
  vec2 d = (vec2){vector_0.x - vector_1.x, vector_0.y - vector_1.y};
  return vec2_length(d);
}

static inline vec3 vec3_create(f32 x, f32 y, f32 z) {
  vec3 out_vector;
  out_vector.x = x;
  out_vector.y = y;
  out_vector.z = z;
  return out_vector;
}

static inline vec3 vec3_from_vec4(vec4 vector) {
  return (vec3){vector.x, vector.y, vector.z};
}

static inline vec4 vec3_to_vec4(vec3 vector, f32 w) {
  return (vec4){vector.x, vector.y, vector.z, w};
}

static inline vec3 vec3_zero() { return (vec3){0.0f, 0.0f, 0.0f}; }

static inline vec3 vec3_one() { return (vec3){1.0f, 1.0f, 1.0f}; }

static inline vec3 vec3_up() { return (vec3){0.0f, 1.0f, 0.0f}; }

static inline vec3 vec3_down() { return (vec3){0.0f, -1.0f, 0.0f}; }

static inline vec3 vec3_left() { return (vec3){-1.0f, 0.0f, 0.0f}; }

static inline vec3 vec3_right() { return (vec3){1.0f, 0.0f, 0.0f}; }

static inline vec3 vec3_forward() { return (vec3){0.0f, 0.0f, -1.0f}; }

static inline vec3 vec3_back() { return (vec3){0.0f, 0.0f, 1.0f}; }

static inline vec3 vec3_add(vec3 vector_0, vec3 vector_1) {
  return (vec3){vector_0.x + vector_1.x, vector_0.y + vector_1.y,
                vector_0.z + vector_1.z};
}

static inline vec3 vec3_sub(vec3 vector_0, vec3 vector_1) {
  return (vec3){vector_0.x - vector_1.x, vector_0.y - vector_1.y,
                vector_0.z - vector_1.z};
}

static inline vec3 vec3_mul(vec3 vector_0, vec3 vector_1) {
  return (vec3){vector_0.x * vector_1.x, vector_0.y * vector_1.y,
                vector_0.z * vector_1.z};
}

static inline vec3 vec3_mul_scalar(vec3 vector, f32 scalar) {
  return (vec3){vector.x * scalar, vector.y * scalar, vector.z * scalar};
}

static inline vec3 vec3_div(vec3 vector_0, vec3 vector_1) {
  return (vec3){vector_0.x / vector_1.x, vector_0.y / vector_1.y,
                vector_0.z / vector_1.z};
}

static inline f32 vec3_length_squared(vec3 vector) {
  return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

static inline f32 vec3_length(vec3 vector) {
  return lai_sqrt(vec3_length_squared(vector));
}

static inline void vec3_normalize(vec3 *vector) {
  const f32 length = vec3_length(*vector);
  vector->x /= length;
  vector->y /= length;
  vector->z /= length;
}

static inline f32 vec3_dot(vec3 vector_0, vec3 vector_1) {
  f32 p = 0;
  p += vector_0.x * vector_1.x;
  p += vector_0.y * vector_1.y;
  p += vector_0.z * vector_1.z;
  return p;
}

static inline f32 vec3_cross(vec3 vector_0, vec3 vector_1) {
  return (vec3){vector_0.y * vector_1.z - vector_0.z * vector_1.y,
                vector_0.z * vector_1.x - vector_0.x * vector_1.z,
                vector_0.x * vector_1.y - vector_0.y * vector_1.x};
}

static inline bool vec3_compare(vec3 vector_0, vec3 vector_1, f32 tolerance) {
  if (lai_abs(vector_0.x - vector_1.x) > tolerance) {
    return false;
  }

  if (lai_abs(vector_0.y - vector_1.y) > tolerance) {
    return false;
  }

  if (lai_abs(vector_0.z - vector_1.z) > tolerance) {
    return false;
  }

  return true;
}

static inline f32 vec3_distance(vec3 vector_0, vec3 vector_1) {
  vec3 d = (vec3){vector_0.x - vector_1.x, vector_0.y - vector_1.y,
                  vector_0.z - vector_1.z};
  return vec3_length(d);
}

static inline vec4 vec4_create(f32 x, f32 y, f32 z, f32 w) {
  vec4 out_vector;
#ifdef LAI_USE_SIMD
  out_vector.data = _mm_setr_ps(x, y, z, w);
#else
  out_vector.x = x;
  out_vector.y = y;
  out_vector.z = z;
  out_vector.w = w;
#endif
  return out_vector;
}

static inline vec3 vec4_to_vec3(vec4 vector) {
  return (vec3){vector.x, vector.y, vector.z};
}

static inline vec4 vec4_from_vec3(vec3 vector, f32 w) {
#ifdef LAI_USE_SIMD
  vec4 out_vector;
  out_vector.data =
      _mm_setr_ps(vector.x, vector.y, vector.z, w) return out_vector;
#else
  return (vec4){vector.x, vector.y, vector.z, w};
#endif
}

static inline vec4 vec4_zero() { return (vec4){0.0f, 0.0f, 0.0f, 0.0f}; }

static inline vec4 vec4_one() { return (vec4){1.0f, 1.0f, 1.0f, 1.0f}; }

static inline vec4 vec4_add(vec4 vector_0, vec4 vector_1) {
  vec4 result;
  for (u64 i = 0; i < 4; ++i) {
    result.elements[i] = vector_0.elements[i] + vector_1.elemets[i];
  }
  return result;
}

static inline vec4 vec4_sub(vec4 vector_0, vec4 vector_1) {
  vec4 result;
  for (u64 i = 0; i < 4; ++i) {
    result.elements[i] = vector_0.elements[i] - vector_1.elemets[i];
  }
  return result;
}

static inline vec4 vec4_mul(vec4 vector_0, vec4 vector_1) {
  vec4 result;
  for (u64 i = 0; i < 4; ++i) {
    result.elements[i] = vector_0.elements[i] * vector_1.elemets[i];
  }
  return result;
}

static inline vec4 vec4_div(vec4 vector_0, vec4 vector_1) {
  vec4 result;
  for (u64 i = 0; i < 4; ++i) {
    result.elements[i] = vector_0.elements[i] / vector_1.elemets[i];
  }
  return result;
}

static inline f32 vec4_length_squared(vec4 vector) {
  return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z +
         vector.w * vector.w;
}

static inline f32 vec4_length(vec4 vector) {
  return lai_sqrt(vec4_length_squared(vector));
}

static inline void vec4_normalize(vec4 *vector) {
  const f32 length = vec4_length(*vector);
  vector->x /= length;
  vector->y /= length;
  vector->z /= length;
  vector->w /= length;
}

static inline vec4 vec4_normalized(vec4 vector) {
  vec4_normalize(&vector);
  return vector;
}

static inline vec4_dot_f32(f32 a0, f32 a1, f32 a2, f32 a3, f32 b0, f32 b1,
                           f32 b2, f32 b3, ) {
  f32 p;
  p = a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
  return p;
}