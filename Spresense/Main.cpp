# include <Siv3D.hpp>

void Main()
{
	// シリアルポートの一覧を取得
	const Array<SerialPortInfo> infos = System::EnumerateSerialPorts();
	const Array<String> options = infos.map([](const SerialPortInfo& info)
	{
		return U"{} ({})"_fmt(info.port, info.description);
	}) << U"none";

	Serial serial;
	serial = Serial(U"COM8", 2000000);
	size_t index = (options.size() - 1);
	double timer = 0;//delayの役割
	bool be_write = false;

	static int WIDTH = 320;
	static int HEIGHT = 240;

	while (System::Update())
	{
		const bool isOpen = serial.isOpen(); // OpenSiv3D v0.4.2 以前は serial.isOpened()

		if (be_write == false) {
			if (SimpleGUI::Button(U"Start", Vec2(200, 20), 120, isOpen))
			{

				//S を書き込む
				serial.write('S');
				serial.clear();
				be_write = true;
				timer = 0;
			}
		}
		if (be_write) {
			timer += Scene::DeltaTime();
		}
	
	
		if (timer >= 0.2) {
			
			if (const size_t available = serial.available())
			{
				int total = 0;
				// シリアル通信で受信したデータを読み込んで表示
				Array<uint8> lbyte = serial.readBytes();
				Array<uint8> ubyte = serial.readBytes();

				
				Console << lbyte.size();
				for (int i = 0; i < lbyte.size();i++) {
					
					int value = (ubyte[i] << 8) | (lbyte[i]); // RGB565 format
					
					uint8 r = (((value & 0xf800) >> 11) << 3);
					uint8 g = (((value & 0x07E0) >> 5) << 2);
					uint8 b = ((value & 0x001f) << 3);
					Color c = Color(r, g, b);
					int x = total % WIDTH;
					int y = total / WIDTH;
					total++;
					RectF(x, y, 1, 1).draw(c);
					if (total >= WIDTH * HEIGHT) {
						serial.clear();

						total = 0;
						break;
					}
				}
			}
		}
	}
}
