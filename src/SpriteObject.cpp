#include"SpriteObject.h"


SpriteObject::SpriteObject(Texture* texture, float controlPointX, float controlPointY) {
	if (!this->drawModel) {
		ModelDataFactory factory;
		MeshModel* model = new MeshModel(factory.CreateSquareModel(1.0f, 1.0f, texture, false));
		if (model == nullptr) {
			throw("SpriteObjectError ModelData Initialize Failed");
		}
		this->drawModel = model;
	}
	this->controlPoint = Vector2f(controlPointX, controlPointY);
	SetTexture(texture);
}
SpriteObject::~SpriteObject() {
	if (this->drawModel != nullptr) {
		delete this->drawModel;
		this->drawModel = nullptr;
	}
}


bool SpriteObject::SetTexture(Texture* texture) {
	this->cullentTexture = texture;
	this->drawModel->SetTexture(texture, 0, 0);
	return true;
}

void SpriteObject::Draw2D(CameraClass* camera, ShaderClass* shader, M::Box2D& src, M::Box2D& draw, float rotation) {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vector3f position(draw.x - camera->GetScreenWidth() / 2, -(draw.y - camera->GetScreenHeight() / 2), 0.0f);
	Vector3f scale(draw.w, draw.h, 1.0f);
	Vector3f rot(0.0f, 0.0f, M::DegToRad(rotation));
	SetMatrix(camera, shader, position, rot, scale);
	if (this->cullentTexture != nullptr) {
		shader->SetValue("textureSize", Vector2f(this->cullentTexture->GetWidth(), this->cullentTexture->GetHeight()));
	}
	shader->SetValue("spriteUV", Vector4f(src.x, src.y, src.w, src.h));
	this->drawModel->Draw(shader);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void SpriteObject::Draw3D(CameraClass* camera, ShaderClass* shader, M::Box2D& src, Vector3f& position, Vector3f& rotation, Vector3f& scale) {
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	scale.x() *= src.w;
	scale.y() *= src.h;

	SetMatrix(camera, shader, position, rotation, scale);
	if (this->cullentTexture != nullptr) {
		shader->SetValue("textureSize", Vector2f(this->cullentTexture->GetWidth(), this->cullentTexture->GetHeight()));
	}
	shader->SetValue("spriteUV", Vector4f(src.x, src.y, src.w, src.h));
	this->drawModel->Draw(shader);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}


////////
//protected
////

void SpriteObject::SetMatrix(CameraClass* camera, ShaderClass* shader, Vector3f& position, Vector3f& rotation, Vector3f& scaling) {
	//�ړ�
	Translation<float, 3> trans = Translation<float, 3>(position.x() + this->controlPoint.x(), position.y() - this->controlPoint.y(), position.z());
	Translation<float, 3> controlTrans = Translation<float, 3>(-this->controlPoint.x(), this->controlPoint.y(), 0.0f);
	//��]����YXZ
	Quaternionf rot;
	rot = AngleAxisf(0, Vector3f::Zero());
	rot = rot * AngleAxisf(rotation.y(), Vector3f::UnitY());
	rot = rot * AngleAxisf(rotation.x(), Vector3f::UnitX());
	rot = rot * AngleAxisf(rotation.z(), Vector3f::UnitZ());
	//�X�P�[��
	DiagonalMatrix<float, 3> scale = DiagonalMatrix<float, 3>(Vector3f(scaling.x(), scaling.y(), scaling.z()));

	Matrix3f cameraMat = camera->GetCameraMatrix().block(0, 0, 3, 3);
	

	Affine3f world = trans * cameraMat * rot * controlTrans * scale;

	Matrix4f view = camera->GetViewMatrix();
	Matrix4f projection = camera->GetProjectionMatrix();

	shader->SetMatrix(projection * view * world.matrix());
}