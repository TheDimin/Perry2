#pragma once

// Hash function for glm::ivec2
namespace std {
	template <>
	struct hash<glm::ivec2> {
		std::size_t operator()(const glm::ivec2& v) const {
			// Simple hash combining function
			return ((std::hash<int>()(v.x)
				^ (std::hash<int>()(v.y) << 1)) >> 1);
		}
	};
}

// Equality function for glm::ivec2
namespace std {
	template <>
	struct equal_to<glm::ivec2> {
		bool operator()(const glm::ivec2& v1, const glm::ivec2& v2) const {
			return v1.x == v2.x && v1.y == v2.y;
		}
	};
}
