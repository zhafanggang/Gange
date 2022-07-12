#ifndef _GG_Prerequisites_H_
#define _GG_Prerequisites_H_

#include <stdint.h>

namespace Gange {
class Angle;
class Degree;
class Vector2;
class Vector3;
class Vector4;
class Matrix3;
class Matrix4;
class Quaternion;
class Radian;

#ifdef _WIN32
#define _GGDllexport __declspec(dllexport)
#define FORCEINLINE __forceinline
#else
#define _GGDllexport
#define FORCEINLINE
#endif

#if OGRE_DOUBLE_PRECISION == 1
/** Software floating point type.
@note Not valid as a pointer to GPU buffers / parameters
*/
typedef double Real;
typedef uint64 RealAsUint;
#else
typedef float Real;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#endif
}  // namespace Gange

#endif
