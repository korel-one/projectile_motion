#include "LibraryManager.h"

LibraryLoadManager::LibraryLoadManager(){}

LibraryLoadManager::~LibraryLoadManager() {
	::FreeLibrary(dll_handle);
}

bool LibraryLoadManager::load_dll(const std::wstring& i_dll) {
	dll_handle = ::LoadLibrary(i_dll.c_str());
	return dll_handle != nullptr;
}