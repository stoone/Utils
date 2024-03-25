#pragma once
#include <string>

#define RELEASE_MEM_SAFE(p)    \
if (p)                         \
{                              \
	delete p;                  \
	p = nullptr;               \
}

#define RELEASE_ARRAY_SAFE(p)  \
if (p)                         \
{                              \
	delete[] p;                \
}

#define CHECK_ZERO(p)          if (p == 0) return;
#define CHECK_ZERO_FALSE(p)    if (p == 0) return false;
#define CHECK_NULL(p)          if (p == nullptr) return;
#define CHECK_NULL_FALSE(p)    if (p == nullptr) return false;

namespace Utils {
	class Utility
	{
	public:
		Utility() {};
		~Utility() {};

		static std::wstring getCurrentThreadIdText();
	};
}
