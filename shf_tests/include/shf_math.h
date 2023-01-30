#if !defined(SHF_MATH_H)
#define SHF_MATH_H

// API Decls
// ================================================
#if defined(_WIN32)
#if defined(BUILD_LIBYTPE_SHARED)
#define SHF_MATH_API __declspec(dllexport)
#elif defined(USE_LIBTYPE_SHARED)
#define SHF_MATH_API __declspec(dllimport)
#else
#define SHF_MATH_API 
#endif // dll check
#else
#define SHF_MATH_API 
#endif // API decls
// ================================================

#include <stdint.h>
#include <math.h>

namespace shf {
	namespace math {
		#define SHF_EPSILON 0.000001f
		#define SHF_PI      3.14159265358979323846f

		SHF_MATH_API float clamp(float value, float min, float max);
		SHF_MATH_API float lerp(float start, float end, float step);

		struct Vec2 {
			Vec2();
			Vec2(float x, float y);

			union {
				float _[2] = {0, 0};

				struct {
					float x;
					float y;
				};
			};

			Vec2& operator+(Vec2& other);
			Vec2& operator-(Vec2& other);
			Vec2& operator*(Vec2& other);
			Vec2& operator/(Vec2& other);

			static float SHF_MATH_API angle(Vec2 v1, Vec2 v2);
			static float SHF_MATH_API distance(Vec2 v1, Vec2 v2);
			static float SHF_MATH_API dot(Vec2 v1, Vec2 v2);
			static float SHF_MATH_API length(Vec2 v1);
			static float SHF_MATH_API length_squared(Vec2 v1);
			static Vec2  SHF_MATH_API normalize(Vec2 v1);
			static Vec2  SHF_MATH_API scale(Vec2 v1, float scale);

		};

		struct Vec3 {
			Vec3();
			Vec3(float x, float y, float z);

			union {
				float _[3] = {0, 0, 0};

				struct {
					float x;
					float y;
					float z;
				};
			};

			Vec3& operator+(Vec3& other);
			Vec3& operator-(Vec3& other);
			Vec3& operator*(Vec3& other);
			Vec3& operator/(Vec3& other);

			static Vec3  SHF_MATH_API cross(Vec3 v1, Vec3 v2);
			static float SHF_MATH_API distance(Vec3 v1, Vec3 v2);
			static float SHF_MATH_API dot(Vec3 v1, Vec3 v2);
			static float SHF_MATH_API length(Vec3 v1);
			static float SHF_MATH_API length_squared(Vec3 v1);
			static Vec3  SHF_MATH_API normalize(Vec3 v1);
			static Vec3  SHF_MATH_API scale(Vec3 v1, float scale);
		};

		struct Vec4 {
			Vec4();
			Vec4(float x, float y, float z, float w);

			union {
				float _[4] = {0, 0, 0, 0};

				struct {
					float x;
					float y;
					float z;
					float w;
				};
			};

			Vec4& operator+(Vec4& other);
			Vec4& operator-(Vec4& other);
			Vec4& operator*(Vec4& other);
			Vec4& operator/(Vec4& other);
		};

		typedef Vec4 Quaternion;

		// Column major for ease of use with GL
		struct Matrix {
			union {
				float _[16];

				struct {
					float m0, m4, m8,  m12;
					float m1, m5, m9,  m13;
					float m2, m6, m10, m14;
					float m3, m7, m11, m15;
				};
			};

			Matrix& operator+(Matrix& other);
			Matrix& operator-(Matrix& other);
			Matrix& operator*(Matrix& other);

			static Matrix SHF_MATH_API identity();
			static Matrix SHF_MATH_API perspective(double fov, double aspect, double near, double far);
			static Matrix SHF_MATH_API orthographic(double left, double right, double bottom, double top, double near, double far);
			static Matrix SHF_MATH_API rotate(Vec3 axis, float angle);
			static Matrix SHF_MATH_API scale(Vec3 scale);
			static Matrix SHF_MATH_API translate(Vec3 translation);
		};
	}
}

#endif // SHF_MATH_H

#if defined(SHF_MATH_IMPL)

namespace shf {
	namespace math {
		float clamp(float value, float min, float max) {
			float f = (value < min) ? min : value;
			if (f > max) f = max;

			return f;
		}

		float lerp(float start, float end, float step) {
			float f = start + step * (end - start);

			return f;
		}

		Matrix& Matrix::operator+(Matrix& other) {
			Matrix m;

			m.m0  = m0  + other.m0;
			m.m1  = m1  + other.m1;
			m.m2  = m2  + other.m2;
			m.m3  = m3  + other.m3;
			m.m4  = m4  + other.m4;
			m.m5  = m5  + other.m5;
			m.m6  = m6  + other.m6;
			m.m7  = m7  + other.m7;
			m.m8  = m8  + other.m8;
			m.m9  = m9  + other.m9;
			m.m10 = m10 + other.m10;
			m.m11 = m11 + other.m11;
			m.m12 = m12 + other.m12;
			m.m13 = m13 + other.m13;
			m.m14 = m14 + other.m14;
			m.m15 = m15 + other.m15;

			return m;
		}

		Matrix& Matrix::operator-(Matrix& other) {
			Matrix m;

			m.m0  = m0  - other.m0;
			m.m1  = m1  - other.m1;
			m.m2  = m2  - other.m2;
			m.m3  = m3  - other.m3;
			m.m4  = m4  - other.m4;
			m.m5  = m5  - other.m5;
			m.m6  = m6  - other.m6;
			m.m7  = m7  - other.m7;
			m.m8  = m8  - other.m8;
			m.m9  = m9  - other.m9;
			m.m10 = m10 - other.m10;
			m.m11 = m11 - other.m11;
			m.m12 = m12 - other.m12;
			m.m13 = m13 - other.m13;
			m.m14 = m14 - other.m14;
			m.m15 = m15 - other.m15;

			return m;
		}

		Matrix& Matrix::operator*(Matrix& other) {
			Matrix m;

			m.m0 = (m0 * other.m0) + (m1 * other.m4) + (m2 * other.m8) + (m3 * other.m12);

			return m;
		}

		Matrix Matrix::identity() {
			Matrix m = { 
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			return m;
		}

		Matrix Matrix::perspective(double fov, double aspect, double near, double far) {
			Matrix m = { 0 };

			double top    = near * tan(fov * 0.5);
			double bottom = -top;
			double right  = top * aspect;
			double left   = -right;

			float rl = (float)(right - left);
			float tb = (float)(top - bottom);
			float fn = (float)(far - near);

			m.m0  = ((float)near * 2.0f) / rl;
			m.m5  = ((float)near * 2.0f) / tb;
			m.m8  = ((float)right + (float)left) / rl;
			m.m9  = ((float)top + (float)bottom) / tb;
			m.m10 = -((float)far + (float)near) / fn;
			m.m11 = -1.0f;
			m.m14 = -((float)far * (float)near * 2.0f) / fn;

			return m;
		}

		Matrix Matrix::orthographic(double left, double right, double bottom, double top, double near, double far) {
			Matrix m = { 0 };

			float rl = (float)(right - left);
			float tb = (float)(top - bottom);
			float fn = (float)(far - near);

			m.m0  = 2.0f / rl;
			m.m1  = 0.0f;
			m.m2  = 0.0f;
			m.m3  = 0.0f;
			m.m4  = 0.0f;
			m.m5  = 2.0f / tb;
			m.m6  = 0.0f;
			m.m7  = 0.0f;
			m.m8  = 0.0f;
			m.m9  = 0.0f;
			m.m10 = -2.0f / fn;
			m.m11 = 0.0f;
			m.m12 = -((float)left + (float)right) / rl;
			m.m13 = -((float)top + (float)bottom) / tb;
			m.m14 = -((float)far + (float)near) / fn;
			m.m15 = 1.0f;


			return m;
		}

		Matrix Matrix::rotate(Vec3 axis, float angle) {
			Matrix m = { 0 };

			float axis_sq_length = Vec3::length_squared(axis);

			Vec3 arot(0, 0, 0);
			if (axis_sq_length != 1.0f && axis_sq_length != 0.0f) {
				arot = Vec3::normalize(axis);
			}

			float sinres = sinf(angle);
			float cosres = cosf(angle);
			float t      = 1.0f - cosres;

			m.m0  = arot.x * arot.x * t + cosres;
			m.m1  = arot.y * arot.x * t + arot.z * sinres;
			m.m2  = arot. z * arot.x * t - arot.y * sinres;
			m.m3  = 0.0f;

			m.m4  = arot.x * arot.y * t - arot.z *sinres;
			m.m5  = arot.y * arot.y *t + cosres;
			m.m6  = arot.z * arot.y * t + arot.x * sinres;
			m.m7  = 0.0f;

			m.m8  = arot.x * arot.z * t + arot.y * sinres;
			m.m9  = arot.y * arot.z * t - arot.x * sinres;
			m.m10 = arot.z * arot.z * t + cosres;
			m.m11 = 0.0f;

			m.m12 = 0.0f;
			m.m13 = 0.0f;
			m.m14 = 0.0f;
			m.m15 = 1.0f;

			return m;
		}

		Matrix Matrix::scale(Vec3 scale) {
			Matrix m = { 0 };

			m.m0  = scale.x;
			m.m5  = scale.y;
			m.m10 = scale.z;
			m.m15 = 1.0f;

			return m;
		}

		Matrix Matrix::translate(Vec3 translation) {
			Matrix m = Matrix::identity();

			m.m12 = translation.x;
			m.m13 = translation.y;
			m.m14 = translation.z;

			return m;
		}

		Vec2::Vec2() {}
		Vec2::Vec2(float x, float y) {
			this->x = x;
			this->y = y;
		}

		Vec2& Vec2::operator+(Vec2& other) {
			Vec2 v = { x + other.x, y + other.y };

			return v;
		}

		Vec2& Vec2::operator-(Vec2& other) {
			Vec2 v = { x - other.x, y - other.y };

			return v;
		}

		Vec2& Vec2::operator*(Vec2& other) {
			Vec2 v = { x * other.x, y * other.y };

			return v;
		}

		Vec2& Vec2::operator/(Vec2& other) {
			Vec2 v = { x / other.x, y / other.y };

			return v;
		}

		float Vec2::angle(Vec2 v1, Vec2 v2) {
			float angle = 0.0f;
			float dot = Vec2::dot(v1, v2);
			float clamped_dot = clamp(dot, -1.0f, 1.0f);

			angle = acosf(clamped_dot);

			return angle;
		}

		float Vec2::distance(Vec2 v1, Vec2 v2) {
			float dx = v1.x - v2.x;
			float dy = v1.y - v2.y;

			float dist = sqrtf((dx * dx) + (dy * dy));

			return dist;
		}

		float Vec2::dot(Vec2 v1, Vec2 v2) {
			float dot = (v1.x * v2.x) + (v1.y * v2.y);

			return dot;
		}

		float Vec2::length(Vec2 v1) {
			float length = sqrtf(length_squared(v1));

			return length;
		}

		float Vec2::length_squared(Vec2 v1) {
			float length_sq = (v1.x * v1.x) + (v1.y * v1.y);

			return length_sq;
		}

		Vec2 Vec2::normalize(Vec2 v1) {
			Vec2 v(0, 0);

			float length = Vec2::length(v1);
			if (length > 0) {
				float ratio = 1.0f / length;
				v = Vec2::scale(v1, ratio);
			}

			return v;
		}

		Vec2 Vec2::scale(Vec2 v1, float scale) {
			Vec2 v = { v1.x * scale, v1.y * scale };

			return v;
		}

		Vec3::Vec3() {}
		Vec3::Vec3(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vec3& Vec3::operator+(Vec3& other) {
			Vec3 v = { x + other.x, y + other.y, z + other.z };

			return v;
		}

		Vec3& Vec3::operator-(Vec3& other) {
			Vec3 v = { x - other.x, y - other.y, z - other.z };

			return v;
		}

		Vec3& Vec3::operator*(Vec3& other) {
			Vec3 v = { x * other.x, y * other.y, z * other.z };

			return v;
		}

		Vec3& Vec3::operator/(Vec3& other) {
			Vec3 v = { x / other.x, y / other.y, z / other.z};

			return v;
		}

		Vec3 Vec3::cross(Vec3 v1, Vec3 v2) {
			Vec3 v;

			v.x = (v1.y * v2.z) - (v1.z * v2.y);
			v.y = (v1.z * v2.x) - (v1.x * v2.z);
			v.z = (v1.x * v2.y) - (v1.y * v2.x);

			return v;
		}

		float Vec3::distance(Vec3 v1, Vec3 v2) {
			float dx = v1.x - v2.x;
			float dy = v1.y - v2.y;
			float dz = v1.z - v2.z;

			float dist = sqrtf((dx * dx) + (dy * dy) + (dz * dz));

			return dist;
		}

		float Vec3::dot(Vec3 v1, Vec3 v2) {
			float dot = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

			return dot;
		}

		float Vec3::length(Vec3 v1) {
			float length = sqrtf(length_squared(v1));

			return length;
		}

		float Vec3::length_squared(Vec3 v1) {
			float length = (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);

			return length;
		}

		Vec3 Vec3::normalize(Vec3 v1) {
			Vec3 v;

			float length = Vec3::length(v1);
			if (length > 0) {
				float ratio = 1.0f / length;
				v = Vec3::scale(v1, ratio);
			}

			return v;
		}

		Vec3 Vec3::scale(Vec3 v1, float scale) {
			Vec3 v = { v1.x * scale, v1.y * scale, v1.z * scale };

			return v;
		}

		Vec4::Vec4() {}
		Vec4::Vec4(float x, float y, float z, float w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		Vec4& Vec4::operator+(Vec4& other) {
			Vec4 v = { x + other.x, y + other.y, z + other.z, w + other.w };

			return v;
		}

		Vec4& Vec4::operator-(Vec4& other) {
			Vec4 v = { x - other.x, y - other.y, z - other.z, w - other.w };

			return v;
		}

		Vec4& Vec4::operator*(Vec4& other) {
			Vec4 v = { x * other.x, y * other.y, z * other.z , w * other.w};

			return v;
		}

		Vec4& Vec4::operator/(Vec4& other) {
			Vec4 v = { x / other.x, y / other.y, z / other.z, w / other.w};

			return v;
		}
	}
}

#endif // SHF_MATH_IMPL