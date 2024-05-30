
#include <iostream>
#include "driver.h"


#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif


template<typename ... A>
uint64_t Driver::CallHook( const A ... arguments )
{
	std::call_once( flag , [ ] {
		HMODULE library = LoadLibraryA( "user32.dll" );
		if ( library == nullptr ) {
			DWORD error = GetLastError( );
			throw( "Failed to load user32.dll. Error code: " + error );
			std::cout << "Failed to load user32.dll. Error code: " << error << std::endl;
			// Trate o erro adequadamente, por exemplo, lançando uma exceção.
		}
		} );

	HMODULE library = LoadLibraryA( "win32u.dll" );
	if ( library == nullptr ) {
		DWORD error = GetLastError( );
		throw( "Hook: Failed loading library! Error code: " + error );
		std::cout << "Hook: Failed loading library! Error code: " << error << std::endl;
		return 0; // Return appropriate error value
	}

	FARPROC control_function = GetProcAddress( library , "NtOpenCompositionSurfaceSectionInfo" );
	if ( control_function == nullptr ) {
		DWORD error = GetLastError( );
		throw( "Hook: Failed getting function address! Error code : " + error );
		std::cout << "Hook: Failed getting function address! Error code: " << error << std::endl;
		FreeLibrary( library );
		return 0; // Return appropriate error value
	}

	const auto control = reinterpret_cast< uint64_t( __stdcall * )( A... ) >( control_function );

	if ( control == nullptr ) {
		throw( "Hook: Obtained function pointer is null! " );
		std::cout << "Hook: Obtained function pointer is null!" << std::endl;
		FreeLibrary( library );
		return 0; // Return appropriate error value
	}

	uint64_t result = control( arguments... );

	//free the library after using it
	FreeLibrary( library );

	return result;
}

AnswareManager Driver::CheckDriver( )
{
	AnswareManager DefaultAnswer = AnswareManager( true );

	_COPY_MEMORY m {};
	m.kFunction = COMMUNICATION;
	m.kPid = NULL;
	m.kAddress = NULL;
	m.kBuffer = nullptr;
	m.kSize = NULL;
	m.kModuleName = nullptr;
	m.KProcessName = nullptr;
	m.kAnswer = &DefaultAnswer;

	CallHook( &m );

	return DefaultAnswer;
}
