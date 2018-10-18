#pragma once
#include "CDFD.cpp"

class Map {
private:
	/*Debug*/
	int debugID;
	/*Debug*/
	int getDebugdebugID() {
		if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
			if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
				if (debugID > 0) {
					debugID--;
				}
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
				if (debugID < map.size() - 1) {
					debugID++;
				}
			}
		}
		printfDx("[%d/%d]\n", map[debugID].cdfd_id, map.size() - 1);

		return 1;
	}
public:
	/*具体化されたCDFDなどの保持*/
	vector<CDFD> map;
	/*抽象化されて表示されるCDFDのIDを保持、scopeOffsetと連動*/
	vector<int> abstractCDFD_ids;
	/*抽象化されるOffsetを保持*/
	OffsetList scopeOffset;
	int scope;

	/*解析中のエラー*/
	bool error;

	Map() {
		init();
	}

	CDFD* init() {
		debugID = scope = 0;
		int i = 0;
		for (i = (int)map.size(); i > 0; i--) {
			map.pop_back();
		}
		for (i = (int)abstractCDFD_ids.size(); i > 0; i--) {
			abstractCDFD_ids.pop_back();
		}
		scopeOffset.init();
		abstractCDFD_ids.push_back(0);
		CDFD cdfd(0);
		map.push_back(cdfd);
		return &map[0];
	}

	/*抽象化する範囲を保存、隔離*/
	CDFD* AddCDFD(int begin, int end) {
		scope = scopeOffset.CheckOffset(begin, end) + 1;
		scopeOffset.AddOffset(begin, end);
		int id = (int)map.size();
		abstractCDFD_ids.push_back(id);
		CDFD cdfd(id);
		map.push_back(cdfd);
		return &map[id];
	}

	/*abstractCDFD_idsの最後尾のIDを取得*/
	int GetAbstractCDFD_id() {
		return (int)map.size();
	}

	/*abstractCDFD_idsの最後尾IDからCDFDの取得*/
	CDFD* ChangeBeforeCDFD() {
		//抽象化されたCDFD
		int id = abstractCDFD_ids[abstractCDFD_ids.size() - 1];
		return &map[id];
	}

	void SetCondition() {
		//抽象化されたCDFD
		int id = abstractCDFD_ids[abstractCDFD_ids.size() - 2];
		//具体化されたCDFD
		CDFD correntCDFD = map[map.size() - 1];
		CDFD::CopyVariableInOut(&map[id], correntCDFD);
		CDFD::CopyConditionText(&map[id], correntCDFD);
	}

	/*抽象化するべき範囲を抜けているかどうか*/
	bool CheckNodeInOffset(int begin, int end) {
		Offset offset(begin, end);
		int this_scope = scopeOffset.CheckOffset(offset);
		int scope_distance = scope - this_scope;
		/*浅くなった場合*/
		if (scope_distance > 0) {
			for (int i = 0; i < scope_distance; i++) {
				abstractCDFD_ids.pop_back();
			}
			scope = this_scope;
			return true;
		}
		return false;
	}

	void SetPosition() {
		for (int i = 0; i < map.size(); i++) {
			map[i].SetPosition();
		}
	}

	bool Draw() {
		getDebugdebugID();
		return map[debugID].Draw();
	}
};