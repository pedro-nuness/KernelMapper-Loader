#pragma once
#include <malloc.h>
#pragma warning(disable:4996)
enum FUNCTIONS
{
	WRITE,
	READ,
	GETPID,
	GETMODULEPOINTER,
	GETMODULEADRESS,
	GETPROCESSPEB,
	COMMUNICATION
};

class AnswareManager
{

	void SetupText(char* MyChar, char* text)
	{
		*MyChar = *text;
	}

public:

	bool bAnswer, kStatus = false;
	char* kResponse = (char*)malloc(10 * sizeof(char));

	AnswareManager(bool wants) {
		this->bAnswer = wants;
	}

	bool WantsAnswer() {
		return this->bAnswer;
	}

	void SetAnswer(char name[256]) {
		strcpy(this->kResponse, name);
	}

	void SetStatus(bool status) {
		this->kStatus = status;
	}

	void finish() {
		free(this->kResponse);
	}

	const char* GetAnswer() {
		return this->kResponse;
	}
	bool GetStatus() {
		return this->kStatus;
	}

};

typedef struct _COPY_MEMORY
{
	int kFunction;
	void* kBuffer;

	ULONG64		kAddress;
	ULONG		kSize;
	HANDLE		kPid;

	const char* kModuleName;
	const char* KProcessName;

	AnswareManager* kAnswer;
}COPY_MEMORY;
