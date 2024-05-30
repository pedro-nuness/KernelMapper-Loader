#ifndef LAUNCH_H
#define LAUNCH_H


#include <Windows.h>
#include <string>
#include <vector>
#include "../Utils/singleton.h"

class Launch : public CSingleton<Launch>
{
	void SetupIDS( );
	void ClearCommunicationFolder( );
	void CheckDriver( );
	void CreateTarget( );
	void CheckHash( );
	void CreateJson( );
	void CreateFileBytes( std::string path , std::string FileID );
	bool IsHashAuthentic( std::string hash );
	void Injection( );
	std::vector<uint8_t> GetFileBytes( std::string file_id );
public:
	void Init( void * adress );
	void ClearApps( );
	void Idle( );
};


#endif
