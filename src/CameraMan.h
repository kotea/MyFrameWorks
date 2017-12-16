#pragma once
#include"GameObject.h"

//カメラクラスと、対象となるほかのGameObjectへのポインタを持つカメラマン
//ターゲットから一定距離を保って動く
class CameraMan : public GameObject {
public:
	bool Initialize(GameParameters& param);
	void Finalize();
	void Run(GameParameters& param);
	void Draw(GameParameters& param);

	void SetParameter(GameObject* terget, CameraClass* camera);

private:
	GameObject* terget;
	CameraClass* camera;
	float camerarotH, camerarotV;

};