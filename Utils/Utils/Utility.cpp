#include "Utility.h"
#include <sstream>
#include <thread>

namespace Utils {

	std::wstring Utility::getCurrentThreadIdText() {
		std::thread::id id = std::this_thread::get_id();
		std::wstringstream sin;
		sin << id;
		return sin.str();
	}


}
