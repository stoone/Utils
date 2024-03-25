#pragma once

#include <vector>
#include <map>

namespace Utils {
	namespace STLUtils {

		template<typename T>
		int removeVector(std::vector<T>& v, T value)
		{
			int count = 0;
			for (auto& iter = v.begin(); iter != v.end();)
			{
				if (*iter == value)
				{
					iter = v.erase(iter);
					count++;
				}
				else
				{
					iter++;
				}
			}

			return count;
		}

		template<typename T, typename N>
		int removeMapByKey(std::map<T, N>& m, T key)
		{
			return m.erase(key);
		}

		template<typename T, typename N>
		int removeMapByValue(std::map<T, N>& m, N value)
		{
			int count = 0;
			for (auto iter = m.begin(); iter != m.end();)
			{
				if (iter->second == value)
				{
					iter = m.erase(iter);
					count++;
					//iter++;
				}
				else
				{
					iter++;
				}
			}

			return count;
		}


	}
}
