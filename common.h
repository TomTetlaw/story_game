#ifndef __COMMON_H__
#define __COMMON_H__

static const int MAX_TOKEN_LENGTH = 2048;
const char *parse_token(const char *text, char token[MAX_TOKEN_LENGTH]);

struct Load_File_Result {
	char *data = nullptr;
	int length = 0;
};

Load_File_Result load_file(const char *filename);

#endif