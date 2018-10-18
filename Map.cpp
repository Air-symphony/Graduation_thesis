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
	/*��̉����ꂽCDFD�Ȃǂ̕ێ�*/
	vector<CDFD> map;
	/*���ۉ�����ĕ\�������CDFD��ID��ێ��AscopeOffset�ƘA��*/
	vector<int> abstractCDFD_ids;
	/*���ۉ������Offset��ێ�*/
	OffsetList scopeOffset;
	int scope;

	/*��͒��̃G���[*/
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

	/*���ۉ�����͈͂�ۑ��A�u��*/
	CDFD* AddCDFD(int begin, int end) {
		scope = scopeOffset.CheckOffset(begin, end) + 1;
		scopeOffset.AddOffset(begin, end);
		int id = (int)map.size();
		abstractCDFD_ids.push_back(id);
		CDFD cdfd(id);
		map.push_back(cdfd);
		return &map[id];
	}

	/*abstractCDFD_ids�̍Ō����ID���擾*/
	int GetAbstractCDFD_id() {
		return (int)map.size();
	}

	/*abstractCDFD_ids�̍Ō��ID����CDFD�̎擾*/
	CDFD* ChangeBeforeCDFD() {
		//���ۉ����ꂽCDFD
		int id = abstractCDFD_ids[abstractCDFD_ids.size() - 1];
		return &map[id];
	}

	void SetCondition() {
		//���ۉ����ꂽCDFD
		int id = abstractCDFD_ids[abstractCDFD_ids.size() - 2];
		//��̉����ꂽCDFD
		CDFD correntCDFD = map[map.size() - 1];
		CDFD::CopyVariableInOut(&map[id], correntCDFD);
		CDFD::CopyConditionText(&map[id], correntCDFD);
	}

	/*���ۉ�����ׂ��͈͂𔲂��Ă��邩�ǂ���*/
	bool CheckNodeInOffset(int begin, int end) {
		Offset offset(begin, end);
		int this_scope = scopeOffset.CheckOffset(offset);
		int scope_distance = scope - this_scope;
		/*�󂭂Ȃ����ꍇ*/
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