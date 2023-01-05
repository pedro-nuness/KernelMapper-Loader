enum FUNCTIONS
{
	WRITE = 1,
	READ = 2,
	GETPID = 3,
	GETMODULEBASE = 4,
	GETPROCESSPEB = 5,
	COMMUNICATION = 6
};

class AnswareManager
{
	bool GetAnswer = false;
	bool kStatus = false;
	char kResponse[256];

	void SetupText(char MyChar[256], const char text[256])
	{
		for (int i = 0; i < 256; i++)
		{
			auto t = text[i];

			if (t == NULL) {
				break;
			}
			MyChar[i] = t;
		}
	}

public:

	bool WantsAnsware() {
		return this->GetAnswer;
	}

	void SetAnswer(char text[256], bool status) {
		SetupText(this->kResponse, text);
		this->kStatus = status;
	}

	void GetAnswer(char Response[256], bool& Status) {
		SetupText(Response, this->kResponse);
		Status = this->kStatus;
	}
};

typedef struct _COPY_MEMORY
{
	int kFunction;
	void* kBuffer;
	bool kSucess;
	AnswareManager Answer;

	ULONG64		kAddress;
	ULONG		kSize;
	HANDLE		kPid;

	const char* kModuleName;
	const char* KProcessName;
}COPY_MEMORY;