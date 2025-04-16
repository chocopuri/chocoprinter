#pragma once

class cmdGenerater {
	public:
		std::string info;
		String posx;
		String posy;
		Array<String> commandList;
		bool flag = 0;

	void cmd_go(Vec2 pos, bool choco) {
		posx = Format(pos.x);
		posy = Format(pos.y);
		commandList.push_back(U"go " + posx + U" " + posy);
		if (choco) {
			commandList.push_back(U"choco white 10 inject>>");
			flag = 1;
		}
		if (!choco && flag) {
			commandList.push_back(U"choco white 20 stop>>");
			flag = 0;
		}
	}
};
