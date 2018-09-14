#pragma once
#include "DxLib.h"
#include<fstream>
#include<iostream>

class FileController {
private:
	static char filepath[MAX_PATH];// = "TestCode\\test.cpp";
	static char currentDirctory[MAX_PATH];// = "TestCode\\test.cpp";
	static char tempDirName[7];
	static char tempFileName[15];
	static char tempFilePath[7 + 15];

	//int fileHandle;

	bool static checkFileExistence(const std::string& str)
	{
		std::ifstream ifs(str);
		return ifs.is_open();
	}
public:
	static char log[1000];

	static bool init() {
		SetFilePath("TestCode\\Sample.cpp");
		strcpy_s(tempDirName, "./temp");
		strcpy_s(tempFileName, "/tempfile.cpp");

		*tempFilePath = '\0';
		strcat_s(tempFilePath, 100, tempDirName);
		strcat_s(tempFilePath, 100, tempFileName);

		//ディレクトリ作成
		CreateDirectory(tempDirName, NULL);

		char cdir[255];
		GetCurrentDirectory(255, cdir);
		strcpy_s(currentDirctory, cdir);
		CreateTemporaryFile();
		return 1;
	}

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
	
	static bool CreateTemporaryFile() {
		SetCurrentDirectory(currentDirctory);

		strcpy_s(log, currentDirctory);
		strcat_s(log, "\n");
		strcat_s(log, "start to create ");
		strcat_s(log, tempFilePath);
		strcat_s(log, "\n");
		//存在したら消す。
		if (checkFileExistence(tempFilePath)) {
			DeleteFile(tempFilePath);
			strcat_s(log, "Delete. \n");
			strcat_s(log, tempFilePath);
			strcat_s(log, "\n");
		}

		//ファイルをtestという名前で同じ階層にコピー
		if (!CopyFile(filepath, tempFilePath, FALSE)) {
			strcpy_s(log, "can't copy file.\n");
			return 0;
		}
		strcat_s(log, "copy file.\n");
		strcat_s(log, filepath);
		strcat_s(log, "=>");
		strcat_s(log, tempFilePath);

		//if (!MoveFile("test.txt", "./temp/test.txt")) {}//ファイルの移動

		return 1;
	}

	static bool RemoveTemporaryFiles() {
		DeleteFile(tempFilePath);
		return RemoveDirectory(tempDirName);
	}
};