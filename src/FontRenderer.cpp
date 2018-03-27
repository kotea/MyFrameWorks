#include"FontRenderer.h"

FontRenderer::FontRenderer() {
	//ロケールを設定し、フォント生成クラスを初期化
	try {
		setlocale(LC_ALL, "japanese");
		this->sprite = new SpriteObject(nullptr);
		this->cullentFont = nullptr;
	}
	catch(std::string& errorText){
		throw(errorText);
	}
}
FontRenderer::~FontRenderer() {
	for (int i = 0; i < this->drawData2D.size(); ++i) {
		delete[] this->drawData2D[i].buffer;
	}
	for (auto i : this->generators) {
		if(i.second != nullptr)
		delete i.second;
	}
	if (this->sprite != nullptr) {
		delete this->sprite;
		this->sprite = nullptr;
	}
}

bool FontRenderer::LoadFont(const char* fontName, const char* filePass) {
	//既にある名前では登録できない
	if (this->generators.find(fontName) != this->generators.end()) {
		return false;
	}
	//読み込みに失敗してもこの関数では例外を投げない
	try {
		FontGenerator* font = new FontGenerator(filePass);
		this->generators[fontName] = font;
	}
	catch (std::string& errorText) {
		printf("%s\n", errorText.data());
		return false;
	}
	SetCullentFont(fontName);
	return true;
}
bool FontRenderer::SetCullentFont(const char* fontName) {
	if (this->generators.find(fontName) == this->generators.end()) {
		return false;
	}
	this->cullentFont = this->generators[fontName];
	return true;
}

void FontRenderer::Draw(CameraClass* camera, ShaderClass* shader) {
	if (this->cullentFont == nullptr) {
		for (int i = 0; i < this->drawData2D.size(); ++i) {
			delete this->drawData2D[i].buffer;
		}
		this->drawData2D.clear();
		return;
	}

	//2D
	for (int i = 0; i < this->drawData2D.size(); ++i) {
		//サイズに合ったテクスチャを選択
		Texture* texture = this->cullentFont->GetFontAtlas(this->drawData2D[i].fontSize);
		this->sprite->SetTexture(texture); 

		//位置をずらしながら一文字づつ描画
		int position = 0;
		if (this->drawData2D[i].buffer == nullptr) {
			continue;
		}
		for (int j = 0; this->drawData2D[i].buffer[j] != L'\0'; ++j) {
			FontGenerator::FontData data;
			this->cullentFont->GetFontData(data, this->drawData2D[i].buffer[j], this->drawData2D[i].fontSize);
			this->sprite->Draw2D(camera, shader, data.locationUV, M::Box2D(this->drawData2D[i].posX + position, this->drawData2D[i].posY + data.fixPositionY, data.locationUV.w, data.locationUV.h));
			position += data.advance;
		}
		delete[] this->drawData2D[i].buffer;
	}
	this->drawData2D.clear();

	//3D
	for (int i = 0; i < this->drawData3D.size(); ++i) {
		//サイズに合ったテクスチャを選択
		Texture* texture = this->cullentFont->GetFontAtlas(this->drawData3D[i].fontSize);
		this->sprite->SetTexture(texture);

		//位置をずらしながら一文字づつ描画
		int stringSize = 0;
		float size = 0.03f;
		Vector3f position(0.0f, 0.0f, 0.0f);
		Vector3f advanceDir = camera->GetAxisX();
		if (this->drawData3D[i].buffer == nullptr) {
			continue;
		}
		for (int j = 0; this->drawData3D[i].buffer[j] != L'\0'; ++j) {
			FontGenerator::FontData data;
			this->cullentFont->GetFontData(data, this->drawData3D[i].buffer[j], this->drawData3D[i].fontSize);
			stringSize += data.advance;
		}
		for (int j = 0; this->drawData3D[i].buffer[j] != L'\0'; ++j) {
			FontGenerator::FontData data;
			this->cullentFont->GetFontData(data, this->drawData3D[i].buffer[j], this->drawData3D[i].fontSize);
			Vector3f pos3D;
			pos3D.x() = this->drawData3D[i].posX;
			pos3D.y() = this->drawData3D[i].posY - data.fixPositionY * size;
			pos3D.z() = this->drawData3D[i].posZ;
			pos3D += position - (advanceDir * stringSize / 2.0f * size);

			this->sprite->Draw3D(camera, shader, data.locationUV, pos3D, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(size, size, size));
			position += advanceDir * data.advance * size;
		}
		delete[] this->drawData3D[i].buffer;
	}
	this->drawData3D.clear();
}

void FontRenderer::DrawString2D(const std::string& text, int size, int posX, int posY) {
	if (this->cullentFont == nullptr) {
		return;
	}

	//ワイド文字列に変換
	wchar_t* buffer = new wchar_t[text.size() + 1];
	if (!CreateFontBuffer(buffer, text.data(), text.size() + 1)) {
		delete[] buffer;
		return;
	}
	//必要なら新規テクスチャ作成
	for (int i = 0; buffer[i] != L'\0'; ++i) {
		this->cullentFont->SetFont(buffer[i], size);
	}

	//posZは2Dでは利用しない
	FontDrawData drawData;
	drawData.buffer = buffer;
	drawData.fontSize = size;
	drawData.posX = posX;
	drawData.posY = posY;

	this->drawData2D.push_back(drawData);
}

void FontRenderer::DrawString3D(const std::string& text, int size, float posX, float posY, float posZ) {
	if (this->cullentFont == nullptr) {
		return;
	}

	//ワイド文字列に変換
	wchar_t* buffer = new wchar_t[text.size() + 1];
	if (!CreateFontBuffer(buffer, text.data(), text.size() + 1)) {
		delete[] buffer;
		return;
	}
	//必要なら新規テクスチャ作成
	for (int i = 0; buffer[i] != L'\0'; ++i) {
		this->cullentFont->SetFont(buffer[i], size);
	}
	FontDrawData drawData;
	drawData.buffer = buffer;
	drawData.fontSize = size;
	drawData.posX = posX;
	drawData.posY = posY;
	drawData.posZ = posZ;

	this->drawData3D.push_back(drawData);
}


void FontRenderer::ClearFontTexture(int size) {
	this->cullentFont->ClearFontAtlas(size);
}

void FontRenderer::ClearAllFontTexture() {
	this->cullentFont->ClearAllFontAtlas();
}

////////
//private
////

bool FontRenderer::CreateFontBuffer(wchar_t* result, const char* src, int numChar) {
	size_t bufferSize;
	mbstowcs_s(&bufferSize, result, numChar, src, numChar);
	//不正なワイド文字による変換エラー
	if (bufferSize == -1) {
		//なんかエラー処理を書く
		return false;
	}
	return true;
}


