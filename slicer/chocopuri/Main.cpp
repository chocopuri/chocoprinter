# include <Siv3D.hpp>  // OpenSiv3D 0.6.16
#include <ranges>
# include "Buttons.hpp"
# include "Stage.hpp"
# include "Serial_com.hpp"
# include "Pin.hpp"
# include "command_generater.hpp"

void Main() {
	Window::Resize(1536, 864);
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Buttons buttons;
	int row = 10, col = 10;
	Stage stage(row, col); //マス目のサイズ設定
	int32 s = Scene::Size().y;

	Pin p(row, col, s);

	Serial_com serial; 

	cmdGenerater c;

	while (System::Update()) {
		buttons.Draw();
		stage.Draw(buttons.grid);
		p.Draw(buttons.choco_switch, buttons.choco_switch_flag);
		if (buttons.run) {
			int tmp = p.enabled.size() - 2;
			for (auto& a : p.vertices | std::views::reverse) {
				c.cmd_go(a.pos, p.enabled[tmp]);
				//Print << p.enabled[tmp] << U" " << tmp;
				tmp--;
			}
			serial.communication(1);
		}
		//Print << p.enabled.size();
		//Print << p.enabled;
		//Print << p.vertices;
		Print << c.commandList;
	}
}
