#pragma once
#include <thread>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <map>
#include <vector>
#include <atomic>


constexpr int MAX_THREAD_SIZE = 10;

class TaskManager
{
public:
	TaskManager(int maxSize = MAX_THREAD_SIZE);
	~TaskManager();

	static TaskManager* getInstance();
	static void release();

	template<class F, class... Args>
	inline bool submit(F&& f, Args&&... args) {
		if (_quit) return false;

		std::function<void()> wrapper = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		addTask([wrapper]() {
			wrapper();
		});

		return true;
	}

private:
	void addTask(const std::function<void()>& func);

	void start();
	void stop();
	void clean();

	void beginThread();

	bool _quit = false;
	std::atomic_uint _taskSize;
	uint32_t _maxSize;
	std::atomic_uint _index;
	std::mutex _wmt;
	std::mutex _mt;
	std::mutex _fmt;
	std::mutex _tmt;
	std::condition_variable _cond;
	std::thread _manager;


	std::queue<std::function<void()>> _tasks;
	std::map<uint64_t, std::thread> _workers;
	std::vector<uint64_t> _finished;
};

