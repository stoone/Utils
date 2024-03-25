#pragma once

constexpr int defaultSize = 64;

namespace Utils {
	template<typename T>
	class NBuffer {
	public:
		NBuffer(size_t size = defaultSize);
		virtual ~NBuffer();

		void Init(size_t size);
		void Clear();
		bool Seek(int pos);

		bool Write(T* buf, size_t len, size_t offset);
		bool Read(T* buf, size_t len, size_t offset);

	private:
		void free();

		T * _p{ nullptr };
		size_t _size;
		size_t _pos;
	};

	template<typename T>
	NBuffer<T>::NBuffer(size_t size)
	{
		Init(size);
	}

	template<typename T>
	NBuffer<T>::~NBuffer()
	{
		if (_p)
		{
			free();
		}
	}

	template<typename T>
	void NBuffer<T>::Init(size_t size)
	{
		_p = nullptr;
		_size = size;
		_pos = 0;
		if (_size > 0)
		{
			_p = new T[_size];
			memset(_p, 0, _size * sizeof(T));
		}
	}


	template<typename T>
	void NBuffer<T>::Clear()
	{
		if (_p)
		{
			memset(_p, 0, _size * sizeof(T));
			_pos = 0;
		}
	}


	template<typename T>
	void NBuffer<T>::free()
	{
		if (_p)
		{
			delete _p;
			_p = nullptr;
		}
	}

	template<typename T>
	bool NBuffer<T>::Seek(int pos)
	{
		if (pos < 0 || pos >= _size)
			return false;

		_pos = pos;
	}

	template<typename T>
	bool NBuffer<T>::Write(T* buf, size_t len, size_t offset)
	{
		if (buf == nullptr || _p == nullptr) return false;

		if (offset < 0 || offset >= _size) return false;

		if (offset + len >= _size) return false;

		memcpy(_p + offset * sizeof(T), buf, len * sizeof(T));
		_pos = (len + offset) * sizeof(T);

		return true;
	}

	template<typename T>
	bool NBuffer<T>::Read(T* buf, size_t len, size_t offset)
	{
		if (buf == nullptr || _p == nullptr) return false;

		if (offset < 0 || offset >= _size) return false;

		if (len + offset >= _size) return false;

		memcpy(buf, _p + offset * sizeof(T), len * sizeof(T));

		return true;
	}

}

