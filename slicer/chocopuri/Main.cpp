# include <Siv3D.hpp>  // OpenSiv3D 0.6.16
# include "Buttons.hpp"
# include "Stage.hpp"
# include "Serial_com.hpp"
# include "Pin.hpp"
using VertexID = int32;

void Main()
{
	Window::Resize(1536, 864);
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	Buttons buttons;
	int row = 32, col = 32;
	Stage stage(row, col); //マス目のサイズ設定
	int32 s = Scene::Size().y;

	Pin p(row, col, s);

	Serial_com serial;


	while (System::Update()) {
		buttons.Draw();
		stage.Draw(buttons.grid);
		p.Draw(buttons.choco_switch, buttons.choco_switch_flag);
		serial.communication(buttons.run);
	}
}
