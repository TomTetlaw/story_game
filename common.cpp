#include "include.h"

Load_File_Result load_file(const char *filename) {
	FILE *f = nullptr;
	Load_File_Result result;

	fopen_s(&f, filename, "rb");

	if (!f) {
		return result;
	}

	int len = 0;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buffer = new char[len + 1];
	fread_s((void*)buffer, len + 1, len, 1, f);
	buffer[len] = 0;

	fclose(f);

	result.data = buffer;
	result.length = len;
	return result;
}