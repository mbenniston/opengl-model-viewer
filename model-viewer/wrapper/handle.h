#pragma once
#include <optional>
#include <iostream>
#include <utility>

template<typename T> 
using CreateFunc = T(*)();

template<typename T>
using DeleteFunc = void(*)(T);

// Move only handle type
template<typename T, CreateFunc<T> creator, DeleteFunc<T> deleter>
class Handle
{
public:
	Handle() : m_handle(creator())
	{
	}

	~Handle()
	{
		release();
	}

	Handle(const Handle&) = delete;
	Handle& operator=(const Handle&) = delete;

	Handle(Handle&& other) noexcept
	{
		std::swap(m_handle, other.m_handle);
	}

	Handle& operator=(Handle&& other) noexcept
	{
		release();
		std::swap(m_handle, other.m_handle);
		return *this;
	}

	static Handle take_ownership(T handle)
	{
		return Handle(handle);
	}

	operator T() const
	{
		return m_handle.value();
	}


private:
	Handle(T handle) : m_handle(handle)
	{
	}

	void release()
	{
		if (m_handle.has_value())
		{
			deleter(m_handle.value());
			m_handle.reset();
		}
	}

	std::optional<T> m_handle;
};

