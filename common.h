#ifndef __COMMON_H__
#define __COMMON_H__

struct Load_File_Result {
	char *data = nullptr;
	int length = 0;
};

Load_File_Result load_file(const char *filename);

#endif