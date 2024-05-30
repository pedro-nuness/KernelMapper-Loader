#pragma once
#include <Windows.h>
#include <malloc.h>
#pragma warning(disable:4996)
enum FUNCTIONS
{
    WRITE ,
    READ ,
    GETPID ,
    GETMODULEPOINTER ,
    GETMODULEADRESS ,
    GETPROCESSPEB ,
    COMMUNICATION ,
    GETMODULESIZE ,
    ISPROCESSWOW64
};


class AnswareManager {
private:
    char * kResponse;

public:
    bool bAnswer , kStatus = false;

    ~AnswareManager( ) {
        // Libera a memória alocada no destrutor
        delete[ ] kResponse;
    }

    AnswareManager( bool wants ) : bAnswer( wants ) {
        kResponse = new char[ 256 ];
    }

    bool WantsAnswer( ) {
        return bAnswer;
    }

    void SetAnswer( const char * name ) {
        strncpy( kResponse , name , 255 );
        kResponse[ 255 ] = '\0';
    }

    void SetStatus( bool status ) {
        kStatus = status;
    }

    char * GetAnswer( ) {
        return kResponse;
    }

    bool GetStatus( ) {
        return kStatus;
    }
};

typedef struct _COPY_MEMORY
{
    int kFunction;
    void * kBuffer;

    ULONG64		kAddress;
    ULONG		kSize;
    HANDLE		kPid;

    const char * kModuleName;
    const char * KProcessName;

    AnswareManager * kAnswer;
}COPY_MEMORY;
