#include"GameParameters.h"

GameParameters::GameParameters() {

}
GameParameters::~GameParameters() {

}
bool GameParameters::Initialize(int screenWidth, int screenHeight) {
	this->screenWidth   = screenWidth;
	this->screenHeight  = screenHeight;
	this->physicsSystem = new BulletPhysics;
	this->input         = new InputClass(0);
	this->textureList   = new TextureList;
	this->shaderList    = new ShaderList;
	this->cameraList    = new CameraList;
	this->lightList     = new LightList;
	return true;
}
void GameParameters::Finalize() {
	if (this->input) {
		delete this->input;
		this->input = 0;
	}
	if (this->physicsSystem) {
		delete this->physicsSystem;
		this->physicsSystem = 0;
	}
	if (this->textureList) {
		delete this->textureList;
		this->textureList = 0;
	}
	if (this->shaderList) {
		delete this->shaderList;
		this->shaderList = 0;
	}
	if (this->lightList) {
		delete this->lightList;
		this->lightList = 0;
	}
	if (this->cameraList) {
		delete this->cameraList;
		this->cameraList = 0;
	}
}
void GameParameters::Run() {
	this->input->Run();
	this->physicsSystem->Run();
}

void GameParameters::UseAmbient(const std::string& name) {
	this->lightList->SetAmbient(name, this->currentShader);
}
void GameParameters::UseDirectional(const std::string& name) {
	this->lightList->SetDirectional(name, this->currentShader);
}
void GameParameters::UseShader(const std::string& name) {
	this->currentShader = this->shaderList->UseShader(name);
}
void GameParameters::UseCamera(const std::string& name) {
	this->currentCamera = this->cameraList->GetCamera(name);
}

InputClass* GameParameters::GetInput() {
	return this->input;
}
BulletPhysics* GameParameters::GetPhysics() {
	return this->physicsSystem;
}
TextureList* GameParameters::GetTextureList() {
	return this->textureList;
}
ShaderList* GameParameters::GetShaderList() {
	return this->shaderList;
}
LightList* GameParameters::GetLightList() {
	return this->lightList;
}
CameraList* GameParameters::GetCameraList() {
	return this->cameraList;
}