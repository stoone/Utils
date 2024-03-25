#include "TaskManager.h"
#include "Utils.h"
#include "win.h"

TaskManager::TaskManager(int maxSize)
{
	_taskSize = 0;
	_maxSize = maxSize;
	_index = 0;

	start();
}


TaskManager::~TaskManager()
{
	stop();
}


TaskManager* g_task = nullptr;
std::mutex g_mt;
TaskManager* TaskManager::getInstance() {
	if (g_task == nullptr)
	{
		std::lock_guard<std::mutex> lk(g_mt);
		if (g_task == nullptr)
		{
			g_task = new TaskManager();
		}
	}
	return g_task;
}

void TaskManager::release() {
	std::lock_guard<std::mutex> lk(g_mt);
	if (g_task)
	{
		delete g_task;
		g_task = nullptr;
	}
}

void TaskManager::addTask(const std::function<void()>& func) {
	std::lock_guard<std::mutex> lk(_mt);
	_tasks.emplace(func);
	_taskSize++;
	_cond.notify_one();
}

void TaskManager::start() {
	_manager = std::thread([this] {
		LOGD("manager thread begin %d");
		while (!_quit)
		{
			if (_quit) break;

			std::unique_lock<std::mutex> lock(_wmt);
			_cond.wait(lock, [this] {
//				std::lock_guard<std::mutex> lk(_mt);
				return _taskSize > 0 || _quit;
			});

			if (_quit) break;

			{
				std::unique_lock<std::mutex> lk(_fmt);
				for (auto& item : _finished)
				{
					std::unique_lock<std::mutex> lk(_tmt);
					auto iter = _workers.find(item);
					if (iter != _workers.end())
					{
						auto& thread = iter->second;
						if (thread.joinable())
						{
							thread.join();
						}
						_workers.erase(iter);
					}


 					//for (auto iter = _workers.begin(); iter != _workers.end(); )
 					//{
 					//	if (iter->first == item)
 					//	{
 					//		auto& thread = iter->second;
 					//		if (thread.joinable())
 					//		{
 					//			thread.join();
 					//		}
 					//		iter = _workers.erase(iter);
 					//		break;
 					//	}
 					//	else {
 					//		iter++;
 					//	}
 					//}
				}
				_finished.clear();
			}

//			std::lock_guard<std::mutex> lk(_mt);
			if (_workers.size() < _maxSize && _taskSize > 0) {
				beginThread();
				_taskSize--;
			}
		}
		LOGD("manager thread end");
	});
}

void TaskManager::stop() {
	_quit = true;
	_cond.notify_all();
	clean();
}

void TaskManager::clean() {
	for (auto& item : _workers)
	{
		if (item.second.joinable())
		{
			item.second.join();
		}
	}

	if (_manager.joinable())
	{
		_manager.join();
	}
}

void TaskManager::beginThread() {
	_index++;
	uint64_t id = _index;
	auto work = [this, id]() {
		std::wstring id_text = Utils::Utility::getCurrentThreadIdText();
		id_text = L"1234567890";
		std::wstring s = L"abcdefg";
		int index = (int)id;
		LOGD(L"thread begin, id=%s, s=%s, index=%d", id_text.c_str(), s.c_str(), id);
		std::function<void()> task;
		{
			std::lock_guard<std::mutex> lk(_mt);
			if (!_tasks.empty())
			{
				task = _tasks.front();
				_tasks.pop();
			}
		}

		if (task)
		{
			task();
		}

		{
			std::unique_lock<std::mutex> lk(_fmt);
			_finished.push_back(id);
		}
		LOGD(L"thread end index=%d", (int)id);
	};

//	auto thread = std::thread(std::move(work));
	std::unique_lock<std::mutex> lk(_tmt);
	_workers.insert(std::make_pair(id, std::thread(std::move(work))));
}


