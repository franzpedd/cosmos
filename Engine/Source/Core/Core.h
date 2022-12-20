#pragma once

// shared library export and import

#ifdef _EXPORT_SHARED_LIB
	#ifdef _MSC_VER
		#define API __declspec(dllexport)
	#else
		#define API __attribute__((visibility("default")))
	#endif
#endif

#ifdef _IMPORT_SHARED_LIB
	#ifdef _MSC_VER
		#define API __declspec(dllimport)
	#else
		#define API
	#endif
#endif