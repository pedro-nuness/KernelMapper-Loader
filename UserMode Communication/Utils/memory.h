#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

#define SAFE_DELETE_VECTOR(vec)	if (!vec.empty()) { \
									for (auto& slot : vec) { \
										delete slot; \
										slot = nullptr; \
									} \
								}
class memory 
{
public:
	DWORD_PTR FindPattern2(ULONG64 mod, const char* pattern)
	{
		if (!mod)
			return 0;

		static auto compare_bytes = [](const byte* bytes, const char* pattern) -> bool
		{
			for (; *pattern; *pattern != ' ' ? ++bytes : bytes, ++pattern) {
				if (*pattern == ' ' || *pattern == '?')
					continue;
				if (*bytes != getByte(pattern))
					return false;
				++pattern;
			}
			return true;
		};

		auto get_text_section = [&](uintptr_t& start, size_t& size)
		{
			auto header = (byte*)malloc(0x1000);
			Global->K->Read(mod, header, 0x1000);

			auto pDosHdr = (const IMAGE_DOS_HEADER*)(header);
			if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
			{
				::free(header);
				return false;
			}
			const IMAGE_NT_HEADERS* pImageHdr = (const IMAGE_NT_HEADERS*)((uint8_t*)pDosHdr + pDosHdr->e_lfanew);

			if (pImageHdr->Signature != IMAGE_NT_SIGNATURE)
			{
				::free(header);
				return false;
			}

			auto pSection = (const IMAGE_SECTION_HEADER*)((uint8_t*)pImageHdr + sizeof(IMAGE_NT_HEADERS));
			for (int i = 0; i < pImageHdr->FileHeader.NumberOfSections; ++i, pSection++)
			{
				if (_stricmp((LPCSTR)pSection->Name, ".text") == 0)
				{
					start = (uintptr_t)(mod + pSection->VirtualAddress);
					size = pSection->Misc.VirtualSize;
					::free(header);
					return true;
				}
			}

			::free(header);
			return false;
		};

		uintptr_t base; size_t size;
		if (!get_text_section(base, size))
		{
			base = mod;
			size = sizeof(mod);
		}

		auto pb = (byte*)malloc(size);
		auto max = size;
		Global->K->Read(base, pb, size);

		uintptr_t offset = 0;
		for (auto off = 0UL; off < max; ++off) {
			if (compare_bytes(pb + off, pattern)) {
				offset = base + off;
				break;
			}
		}

		return offset;
	}


	BOOL CheckMask(PCHAR base, PCHAR pattern, PCHAR mask) {
		for (; *mask; ++base, ++pattern, ++mask) {
			if (*mask == 'x' && *base != *pattern) {
				return FALSE;
			}
		}

		return TRUE;
	}
	PVOID FindPattern(PCHAR base, DWORD length, PCHAR pattern, PCHAR mask) {
		length -= (DWORD)strlen(mask);
		auto buffer = new char[length];
		Global->K->Read((uintptr_t)base, (LPVOID)(uintptr_t)buffer, length);
		for (DWORD i = 0; i <= length; ++i) {
			PVOID addr = &buffer[i];
			if (CheckMask((PCHAR)addr, pattern, mask)) {
				delete[] buffer;
				return addr;
			}
		}
		delete[] buffer;
		return 0;
	}
#define IMAGE_FIRST_SECTION_CUSTOM( ntheader, pntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(pntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader)).FileHeader.SizeOfOptionalHeader   \
    ))
	PVOID FindPatternImage(uintptr_t base, PCHAR pattern, PCHAR mask) {
		PVOID match = 0;

		IMAGE_DOS_HEADER dos_header = { 0 };
		IMAGE_NT_HEADERS64 nt_headers = { 0 };

		Global->K->Read(base, (LPVOID)(uintptr_t)&dos_header, sizeof(dos_header));

		if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
			return 0;

		Global->K->Read(base + dos_header.e_lfanew, (LPVOID)(uintptr_t)&nt_headers, sizeof(nt_headers));

		if (nt_headers.Signature != IMAGE_NT_SIGNATURE)
			return 0;

		for (DWORD i = 0; i < nt_headers.FileHeader.NumberOfSections; ++i) {
			IMAGE_SECTION_HEADER section = { 0 };
			Global->K->Read((uintptr_t)IMAGE_FIRST_SECTION_CUSTOM(nt_headers, (PIMAGE_NT_HEADERS)(base + dos_header.e_lfanew)) + (i * sizeof(IMAGE_SECTION_HEADER)), (LPVOID)(uintptr_t)&section, sizeof(section));
			if (*(PINT)section.Name == 'EGAP' || memcmp(section.Name, ".text", 5) == 0) {
				match = FindPattern((PCHAR)base + section.VirtualAddress, section.Misc.VirtualSize, pattern, mask);
				if (match) {
					break;
				}
			}
		}

		return match;
	}
};

extern memory* Memory;