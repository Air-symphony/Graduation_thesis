#pragma once
#include "DxLib.h"

class ReadFiles {
private:
	static char filepath[100];// = "TestCode\\test.cpp";
	//int fileHandle;

public:
	static int SetFilePath(char* _filepath) {
		return strcpy_s(filepath, _filepath);
	}

	static char *GetFilepath() {
		return filepath;
	}

	static bool PrintFile(HWND textbox) {
		SetWindowText(textbox, "");
		int fileHandle = FileRead_open(filepath);

		if(fileHandle == 0) {
			return false;
		}

		int fileLength = (int)(FileRead_size(filepath) * 2);

		char* text = (char*)malloc(fileLength);
		*text = '\0';

		while (FileRead_eof(fileHandle) == 0) {
			char string[100];
			FileRead_gets(string, 100, fileHandle);

			strcat_s(text, fileLength, string);
			strcat_s(text, fileLength, "\r\n");
		}
		SetWindowText(textbox, text);
		free(text);

		FileRead_close(fileHandle);
		return true;
	}

	static bool SaveFile(){
		FILE *fp;
		int err_no = fopen_s(&fp, filepath, "w");
		if (err_no != 0) return false;
    
		//fprintf(fp, "%f\n", "");

		fclose(fp);
		return true;
	}
};