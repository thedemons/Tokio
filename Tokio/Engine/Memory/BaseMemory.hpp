#pragma once


class BaseMemory
{
protected:
	std::shared_ptr<ProcessData> m_target;

public:

	virtual auto Attach(DWORD pid) -> SafeResult(std::shared_ptr<ProcessData>) = 0;
	virtual void Detach() = 0;

	virtual auto Read(POINTER src, void* dest, size_t size) -> SafeResult(void) = 0;
	virtual auto Write(POINTER dest, const void* src, size_t size) -> SafeResult(void) = 0;

	template <typename Type>
	auto Read(POINTER address) -> SafeResult(Type)
	{
		Type value;
		auto result = Read(address, &value, static_cast<size_t>(sizeof(Type)));

		return result.has_error() ? result.error() : value;
	}

	template <typename Type>
	auto Write(POINTER address, Type value) -> SafeResult(void)
	{
		return Write(address, &value, static_cast<size_t>(sizeof(Type)));
	}

	virtual _CONSTEXPR20 DWORD GetPID()
	{
		return m_target->pid;
	}

	virtual _CONSTEXPR20 HANDLE GetHandle()
	{
		return m_target->handle;
	}

};