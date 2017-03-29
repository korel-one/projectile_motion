#pragma once

#include <Windows.h>
#include <string>

class IModel;

/*--------------------------------------------------------------------
Acquires dll and transfers interface to the client
--------------------------------------------------------------------*/
class LibraryLoadManager {
public:

	LibraryLoadManager();
	LibraryLoadManager(const LibraryLoadManager&) = delete;
	LibraryLoadManager& operator=(const LibraryLoadManager&) = delete;
	~LibraryLoadManager();

	bool load_dll(const std::wstring& i_dll);

	template <typename FUNC>
	IModel* get_instance(const std::string& proc, FUNC p_func) {
		// Get the function from the DLL
		FUNC flight_func = reinterpret_cast<FUNC>(
			::GetProcAddress(dll_handle, proc.c_str()));

		if (!flight_func) {
			::FreeLibrary(dll_handle);
			return nullptr;
		}

		return &flight_func();
	}
	

private:
	HMODULE dll_handle;
	IModel* model = nullptr;
};