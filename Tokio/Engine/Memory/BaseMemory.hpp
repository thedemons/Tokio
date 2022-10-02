#pragma once
#ifndef TOKIO_ENGINE_BASEMEMORY_HPP
#define TOKIO_ENGINE_BASEMEMORY_HPP


namespace Engine
{
class BaseMemory
{
protected:
	std::shared_ptr<ProcessData> m_target;

public:

	_NODISCARD virtual auto Attach(DWORD pid)->SafeResult(std::shared_ptr<ProcessData>) = 0;
	virtual void Detach() = 0;

	_NODISCARD virtual auto Read(POINTER src, void* dest, size_t size)->SafeResult(void) = 0;
	_NODISCARD virtual auto Write(POINTER dest, const void* src, size_t size)->SafeResult(void) = 0;

	template <typename Type>
	_NODISCARD auto Read(POINTER address)->SafeResult(Type)
	{
		Type value;
		auto result = Read(address, &value, static_cast<size_t>(sizeof(Type)));
		if (result.has_error()) RESULT_FORWARD(result);

		return value;
	}

	template <typename Type>
	_NODISCARD auto Write(POINTER address, Type value)->SafeResult(void)
	{
		return Write(address, &value, static_cast<size_t>(sizeof(Type)));
	}

	_NODISCARD virtual _CONSTEXPR20 DWORD GetPID()
	{
		return m_target->pid;
	}

	_NODISCARD virtual _CONSTEXPR20 HANDLE GetHandle()
	{
		return m_target->handle;
	}

};
}

#endif // !TOKIO_ENGINE_BASEMEMORY_HPP