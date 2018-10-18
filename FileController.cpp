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
	static char log[USHRT_MAX];

	static bool init() {
		SetFilePath("TestCode\\templateStmt.cpp");
		strcpy_s(tempDirName, "./temp");
		strcpy_s(tempFileName, "/tempfile.cpp");

		*tempFilePath = '\0';
		strcat_s(tempFilePath, 100, tempDirName);
		strcat_s(tempFilePath, 100, tempFileName);

		//ディレクトリ作成
		CreateDirectory(tempDirName, NULL);

		char cdir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, cdir);
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

		FILE *fp;
		int err_no = fopen_s(&fp, filepath, "r");
		if (err_no != 0) {
			fclose(fp);
			strcpy_s(log, "Can't print.\n");
			strcat_s(log, filepath);
			return false;
		}
		//DWORD size = GetFileSize(fp, NULL);
		int filesize = (int)FileRead_size(filepath);
		char* text = (char*)malloc(filesize * 2);
		*text = '\0';

		char buf[USHRT_MAX] = "";
		while (fgets(buf, filesize, fp) != NULL) {
			std::string destStr = "\0";

			const char CR = '\r';
			const char LF = '\n';
			const char* CRLF = "\r\n";
			int count = 0;
			for (auto it : std::string(buf)) {
				if (it != CR && it != LF) {
					destStr += it;
					count = 0;
				}
				else if (count == 0) {
					if (it == CR || it == LF) {
						destStr += CRLF;
						count = 1;
					}
				}
			}
			strcat_s(text, filesize * 2, destStr.c_str());
		}
		fclose(fp);
		if (strlen(text) > 0) {
			SetWindowText(textbox, text);
			free(text);
		}
		return true;
	}

	static bool SaveTempFile(HWND textbox) {
		/*
		char* text = (char*)malloc(USHRT_MAX);
		*text = '\0';
		GetWindowText(textbox, text, USHRT_MAX);
		FILE *fp;
		int err_no = fopen_s(&fp, tempFilePath, "w");
		if (err_no != 0) return false;

		fprintf(fp, text);

		fclose(fp);
		free(text);
		*/
		return true;
	}

	static bool SaveFile(HWND textbox) {
		FILE *fp;
		int err_no = fopen_s(&fp, filepath, "w");
		if (err_no != 0) {
			fclose(fp);
			strcpy_s(log, "Can't save.\n");
			strcat_s(log, filepath);
			return false;
		}

		char* text = (char*)malloc(USHRT_MAX);
		*text = '\0';
		GetWindowText(textbox, text, USHRT_MAX);

		const char CR = '\r', LF = '\n';
		int count = 0;
		std::string destStr = "\0";
		for (auto it : std::string(text)) {
			if (it != CR && it != LF) {
				destStr += it;
				count = 0;
			}
			else if (count == 0) {
				if (it == CR || it == LF) {
					destStr += LF;
					count = 1;
				}
			}
		}
		strcpy_s(text, USHRT_MAX, destStr.c_str());
		if (strlen(text) > 0) {
			fprintf(fp, text);
			free(text);
		}
		fclose(fp);

		strcpy_s(log, "Complete to save.\n");
		strcat_s(log, filepath);
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

	static int RemoveTemporaryFiles() {
		DeleteFile(tempFilePath);
		return RemoveDirectory(tempDirName);
	}
};