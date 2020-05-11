#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#ifndef M_PI_F
#define M_PI_F ((float)(M_PI))
#endif

inline float deg2rad(float x) {
	return x * (M_PI_F / 180.0f);
}

inline float rad2deg(float x) {
	return x * (180.0f / M_PI_F);
}

inline float approach(float current, float goal, float dt) {
	float dist = goal - current;
	if (dist > dt) {
		return current + dt;
	}
	else if (dist < -dt) {
		return current - dt;
	}
	else {
		return goal;
	}
}

inline bool within_tolerance(float x, float y, float tolerance) {
	if (x > y + tolerance) {
		return false;
	}
	else if (x < y - tolerance) {
		return false;
	}
	else {
		return true;
	}
}

template<typename T>
inline T clamp(T x, T min_value, T max_value) {
	if (x < min_value) {
		return min_value;
	}
	else if (x > max_value) {
		return max_value;
	}
	else {
		return x;
	}
}

inline float map_range(float x, float min1, float max1, float min2, float max2) {
	return (max2 - min2) * (x / (max1 - min1));
}

inline float random_float(float min_value, float max_value) {
	return min_value + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_value - min_value)));
}

#define V4PARMS(v) v.x,v.y,v.z,v.w
struct Vec4 {
	float x = 0, y = 0, z = 0, w = 0;

	Vec4() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vec4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	float distance_to(Vec4 &other) {
		Vec4 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	Vec4 operator +(Vec4 &other) {
		return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
	}
	Vec4 operator -(Vec4 &other) {
		return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
	}
	Vec4 operator *(Vec4 &other) {
		return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
	}
	Vec4 operator +(float other) {
		return Vec4(x + other, y + other, z + other, w + other);
	}
	Vec4 operator -(float other) {
		return Vec4(x - other, y - other, z - other, w - other);
	}
	Vec4 operator *(float other) {
		return Vec4(x * other, y * other, z * other, w * other);
	}
};

#define V3PARMS(v) v.x,v.y,v.z
struct Vec3 {
	float x = 0, y = 0, z = 0;

	Vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float distance_to(Vec3 &other) {
		Vec3 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y + z*z);
	}

	Vec3 normalize() {
		float len = length();
		return Vec3(x / len, y / len, z / len);
	}

	float dot(Vec3 &other) {
		return x*other.x + y*other.y + z*other.z;
	}

	Vec3 cross(Vec3 &other) {
		Vec3 out;
		out.x = y * other.z - z * other.y;
      	out.y = z * other.x - x * other.z;
    	out.z = x * other.y - y * other.x;
		return out;
	}

	Vec3 negate() {
		return Vec3(-x, -y, -z);
	}

	Vec3 operator +(Vec3 &other) {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}
	Vec3 operator -(Vec3 &other) {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
	Vec3 operator *(Vec3 &other) {
		return Vec3(x * other.x, y * other.y, z * other.z);
	}
	Vec3 operator +(float other) {
		return Vec3(x + other, y + other, z + other);
	}
	Vec3 operator -(float other) {
		return Vec3(x - other, y - other, z - other);
	}
	Vec3 operator *(float other) {
		return Vec3(x * other, y * other, z * other);
	}
}; 

#define V2PARMS(v) v.x, v.y
struct Vec2 {
	float x = 0, y = 0;

	Vec2() {
		x = 0;
		y = 0;
	}

	Vec2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	float distance_to(Vec2 &other) {
		Vec2 delta = *this - other;
		return delta.length();
	}

	float length() {
		return sqrtf(x*x + y*y);
	}

	float dot(Vec2 &other) {
		return x*other.x + y*other.y;
	}

	float determinant(Vec2 &other) {
		return x*other.y - y*other.x;
	}

	Vec2 normalize() {
		float len = length();
		return Vec2(x / len, y / len);
	}

	float angle_to(Vec2 &other) {
		Vec2 up, right, dir;
		float dot_product, side, angle;
		
		up = normalize();
		right = Vec2(-up.y, up.x);
		dir = other.normalize();
		dot_product = up.dot(dir);
		side = right.dot(dir);
		angle = acosf(dot_product);
		
		if (side < 0.0f) {
			angle *= -1.0f;
		}
		
		return angle;
	}

	Vec2 operator +(Vec2 &other) {
		return Vec2(x + other.x, y + other.y);
	}
	Vec2 operator -(Vec2 &other) {
		return Vec2(x - other.x, y - other.y);
	}
	Vec2 operator *(Vec2 &other) {
		return Vec2(x * other.x, y * other.y);
	}
	Vec2 operator +(float other) {
		return Vec2(x + other, y + other);
	}
	Vec2 operator -(float other) {
		return Vec2(x - other, y - other);
	}
	Vec2 operator *(float other) {
		return Vec2(x * other, y * other);
	}
};

struct Mat4 {
	union {
		float e[16];
		float m[4][4];
		Vec4 v[4];
		struct {
			float _00, _01, _02, _03, 
				_10, _11, _12, _13,
				_20, _21, _22, _23,
				_30, _31, _32, _33;
		};
	};

	Mat4() {
		for(int i = 0; i < 16; i++) {
			e[i] = 0;
		}
	}
};

inline Mat4 glm_mat4x4_to_mat4(glm::mat4x4 mat) {
	Mat4 out;
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++ )	{
			out.m[i][j] = mat[i][j];
		}
	}
	return out;
}

inline Mat4 create_identity_matrix() {
	return glm_mat4x4_to_mat4(glm::identity<glm::mat4x4>());
}

inline Mat4 create_ortho_matrix(float left, float right, float bottom, float top) {
	return glm_mat4x4_to_mat4(glm::ortho(left, right, bottom, top, -1.0f, 1.0f));
}

inline Mat4 create_perspective_matrix(float fov, float width, float height, float z_near, float z_far) {
	return glm_mat4x4_to_mat4(glm::perspectiveFov(fov, width, height, z_near, z_far));
}

inline Mat4 create_translation_matrix(Vec3 t) {
	return glm_mat4x4_to_mat4(glm::translate(glm::mat4x4(), glm::vec3(t.x, t.y, t.z)));
}

inline Mat4 create_x_rotation_matrix(float angle) {
	return glm_mat4x4_to_mat4(glm::rotate(glm::mat4x4(), angle, glm::vec3(1.0f, 0.0f, 0.0f)));
}

inline Mat4 create_y_rotation_matrix(float angle) {
	return glm_mat4x4_to_mat4(glm::rotate(glm::mat4x4(), angle, glm::vec3(0.0f, 1.0f, 0.0f)));
}

inline Mat4 create_z_rotation_matrix(float angle) {
	return glm_mat4x4_to_mat4(glm::rotate(glm::mat4x4(), angle, glm::vec3(0.0f, 0.0f, 1.0f)));
}

inline Mat4 create_look_at_matrix(Vec3 position, Vec3 direction, Vec3 up) {
	return glm_mat4x4_to_mat4(glm::lookAt(
		glm::vec3(position.x, position.y, position.z),
		glm::vec3(direction.x, direction.y, direction.z),
		glm::vec3(up.x, up.y, up.z)
	));
}

inline Mat4 multiply_matrix(Mat4 a, Mat4 b) {
	glm::mat4x4 _a(a.e[0], a.e[1], a.e[2], a.e[3], a.e[4], a.e[5], a.e[6], a.e[7], a.e[8], a.e[9], a.e[10], a.e[11], a.e[12], a.e[13], a.e[14], a.e[15]);
	glm::mat4x4 _b(b.e[0], b.e[1], b.e[2], b.e[3], b.e[4], b.e[5], b.e[6], b.e[7], b.e[8], b.e[9], b.e[10], b.e[11], b.e[12], b.e[13], b.e[14], b.e[15]);
	return glm_mat4x4_to_mat4(_a * _b);
}

inline Mat4 transpose_matrix(Mat4 m) {
	glm::mat4x4 _m(m.e[0], m.e[1], m.e[2], m.e[3], m.e[4], m.e[5], m.e[6], m.e[7], m.e[8], m.e[9], m.e[10], m.e[11], m.e[12], m.e[13], m.e[14], m.e[15]);
	return glm_mat4x4_to_mat4(glm::transpose(_m));
}

#endif
