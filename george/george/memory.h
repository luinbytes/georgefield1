#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>
#include <string_view>

class Memory
{
private:
	std::uintptr_t processId = 0;
	void* processHandle = nullptr;

public:
	Memory(const WCHAR* processName) noexcept
	{
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(::PROCESSENTRY32);

		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (::Process32Next(snapShot, &entry))
		{
			if (!wcscmp(processName, entry.szExeFile))
			{
				processId = entry.th32ProcessID;
				processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
				break;
			}
			
		}

		if (snapShot)
			::CloseHandle(snapShot);
	}
	Memory() noexcept
	{
		processId = 0;
		processHandle = nullptr;
	}
	~Memory()
	{
		if (processHandle)
			::CloseHandle(processHandle);
	}
	const bool Valid() const noexcept
	{
		return !!processId;
	}
	const std::uintptr_t GetModuleAddress(const WCHAR* moduleName) const noexcept
	{
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(::MODULEENTRY32);

		const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

		std::uintptr_t result = 0;

		while (::Module32Next(snapShot, &entry))
		{
			if (!wcscmp(moduleName,entry.szModule))
			{
				result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (snapShot)
			::CloseHandle(snapShot);

		return result;
	}

	template <typename T>
	constexpr const T Read(const std::uintptr_t& address,SIZE_T size = sizeof(T)) const noexcept
	{
		T value = { };
		::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, size, NULL);
		return value;
	}

	template <typename T>
	constexpr void Write(const std::uintptr_t& address, const T& value, SIZE_T size = sizeof(T)) const noexcept
	{
		::WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, size, NULL);
	}
};