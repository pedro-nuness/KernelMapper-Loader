#include <iostream>
#include <direct.h>

class TempFiles
{
	bool bSetuped = false;

	std::string Temp;
	std::string RandomFolder;
	std::string CommunicationFolder;

	void CreatePaths()
	{
		this->CommunicationFolder = CommunicationFolder;
		this->RandomFolder = Temp + RandomFolder + crypt_str("\\");

		_mkdir(this->CommunicationFolder.c_str());
		_mkdir(this->RandomFolder.c_str());
	}


public:

	TempFiles(std::string Temp, std::string RandomFolder, std::string CommunicationFolder) {
		this->bSetuped = true;
		this->Temp = Temp;
		this->RandomFolder = RandomFolder;
		this->CommunicationFolder = CommunicationFolder;
		this->CreatePaths();
	}

	std::string GetTempFolder() {
		if (bSetuped)
			return Temp;

		return std::string("");
	}

	std::string GetRandomFolder() {
		if (bSetuped)
			return RandomFolder;

		return std::string("");
	}

	std::string GetCommunicationFolder() {
		if (bSetuped)
			return CommunicationFolder;

		return std::string("");
	}

};

std::string RandomLetter()
{
	std::string letters[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i",
					"j", "k", "l", "m", "n", "o", "p", "q", "r",
					"s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C"
					"D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O"
					"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::shuffle(std::begin(letters), std::end(letters),
		std::mt19937(seed));

	for (auto c : letters)
		return c;

}

std::string GetRandomWord(int size)
{
	std::string name;

	for (int b = 0; b < size; b++)
		name += RandomLetter();

	return name;
}

TempFiles SetupDirectory()
{
	auto TempPath = std::filesystem::temp_directory_path().string();
	auto RandomFolder = GetRandomWord(17);

	return TempFiles(TempPath, RandomFolder, crypt_str("c:\\Windows\\Communication\\"));
}


