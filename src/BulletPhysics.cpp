#include"BulletPhysics.h"

void printVec3(btVector3& vec) {
	printf("%+.4f, %+.4f, %+.4f\n", vec.x(), vec.y(), vec.z());
}

BulletPhysics::BulletPhysics() {

}
BulletPhysics::~BulletPhysics() {
	btCollisionObjectArray a;
	for (int i = this->bulletWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject *obj = this->bulletWorld->getCollisionObjectArray()[i];
		btRigidBody       *rigid = btRigidBody::upcast(obj);
		if (rigid && rigid->getMotionState()) {
			delete rigid->getMotionState();
		}
		this->bulletWorld->removeCollisionObject(obj);
		delete obj;
	}
	for (int i = 0; i < this->shapeArray.size(); ++i) {
		delete this->shapeArray[i];
	}

	delete this->bulletWorld;
	delete this->solver;
	delete this->broadphase;
	delete this->dispatcher;
	delete this->config;

	delete this->shader;
}

//bulletWorld�̏�����
bool BulletPhysics::Initialize() {
	this->config      = new btDefaultCollisionConfiguration;
	this->dispatcher  = new btCollisionDispatcher(this->config);
	this->broadphase  = new btDbvtBroadphase;
	this->solver      = new btSequentialImpulseConstraintSolver;
	this->bulletWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->broadphase, this->solver, this->config);
	
	this->debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints);
	this->bulletWorld->setDebugDrawer(&this->debugDrawer);

	this->bulletWorld->setGravity(btVector3(0.0f, -10.0f, 0.0f));

	this->shader = new ShaderClass;
	if (!this->shader->Initialize("SimpleShader.vs", "SimpleShader.ps")) {
		return false;
	}
	debugDrawer.SetShader(this->shader);
	return true;
}
//�V�~�����[�V������i�߂�
void BulletPhysics::Run() {
	this->bulletWorld->stepSimulation(1 / 60.0f, 10, 1 / 60.0f);
}
//�f�o�b�O�`��
void BulletPhysics::DebugDraw(Matrix4f& world, Matrix4f& view, Matrix4f& projection) {
	this->shader->UseShader();

	Matrix4f mat = projection * view * world;
	this->shader->SetMatrix(mat);
	this->bulletWorld->debugDrawWorld();
}

void BulletPhysics::AddRigidBody(btRigidBody *rigidbody, int confrictMask, int hitMask) {
	this->shapeArray.push_back(rigidbody->getCollisionShape());
	this->bulletWorld->addRigidBody(rigidbody, confrictMask, confrictMask | hitMask);
	rigidbody->setUserIndex(confrictMask);
	rigidbody->setUserIndex2(hitMask);
}
void BulletPhysics::AddCollisionObject(btCollisionObject *obj) {
	this->shapeArray.push_back(obj->getCollisionShape());
	this->bulletWorld->addCollisionObject(obj);
}
void BulletPhysics::RemoveRigidBody(btRigidBody *rigidbody) {

}
void BulletPhysics::RemoveCollisionObject(btCollisionObject *obj) {

}

//�ʌ`��̃R���W�����𓮂����B���̍ۂɁA�Փ˂�����������true��Ԃ�(���^)
btVector3 BulletPhysics::MoveCharacterObject(btCollisionObject *obj, btVector3 &moveVector, float limitAngle) {
	btCollisionShape *shape = obj->getCollisionShape();
	if (!shape->isConvex()) {
		return btVector3(0, 0, 0);
	}

	btTransform from = obj->getWorldTransform();
	btTransform to = from;
	to.setOrigin(to.getOrigin() + moveVector);
	//from��to���������Ƃ��ɃG���[��f���̂ŋA���Ă�������
	if (from == to || moveVector.isZero()) {
		return btVector3(0, 0, 0);
	}

	
	//���R�Ȃ��玩�����g�Ƃ͔�������Ȃ��悤�ȃR�[���o�b�N������Ă���
	MyConvexClosestCallBack cb(obj);
	this->bulletWorld->convexSweepTest((btConvexShape*)shape, from, to, cb);
	//std::cout << cb.count << std::endl;


	if (!cb.hasHit()) {
		obj->setWorldTransform(to);
		return btVector3(0, 0, 0);
	}


	//�Փ˓_���Z�o
	btVector3 objPos;
	//�G���[�h�~�Ń[�������� �ibtVector3��w�̒l�͎g���Ȃ��͂��Ȃ̂ɁA��r���ɂ����Ǝg����Ƃ���㩁j
	objPos.setZero();
	objPos.setInterpolate3(from.getOrigin(), to.getOrigin(), cb.m_closestHitFraction - 0.01f);

	float angle_cos = (float)cb.m_hitNormalWorld.dot(-moveVector.normalized());
	float limit_cos = (float)btCos(btRadians(limitAngle));
	angle_cos = (int)(angle_cos * 10000.0f) / 10000.0f;
	limit_cos = (int)(limit_cos * 10000.0f) / 10000.0f;
	if (abs(angle_cos - limit_cos) < 0.0001f) {
		angle_cos = limit_cos;
	}
	//�w��̊p�x���₪�ɂ₩�Ȃ犊��Ȃ�
	if (angle_cos > limit_cos) {
		to.setOrigin(objPos);
		obj->setWorldTransform(to);
		return cb.m_hitNormalWorld;
	}

	//�ǂ���x�N�g�����Z�o
	btVector3 goVec;
	goVec.setZero();
	goVec = to.getOrigin() - objPos;
	goVec += cb.m_hitNormalWorld * (-goVec.dot(cb.m_hitNormalWorld) + 0.01f);

	float norm = goVec.norm();


	MyConvexClosestCallBack cb2(obj);
	btVector3 result = btVector3(0, 0, 0);
	//�ǂ���̏Փ˔���
	from.setOrigin(objPos);
	to.setOrigin(from.getOrigin() + goVec);
	//from��to���������Ƃ��ɃG���[��f���̂ŋA���Ă�������
	if (!(from == to)) {
		this->bulletWorld->convexSweepTest((btConvexShape*)shape, from, to, cb2, 1.0f);
		//�ǂ���x�N�g�����ǂɂԂ���Ȃ���ΏՓ˖@���͕Ԃ��Ȃ�
		if (cb2.hasHit()) {
			objPos.setInterpolate3(from.getOrigin(), to.getOrigin(), cb2.m_closestHitFraction - 0.01f);
			from.setOrigin(objPos);
			to.setOrigin(from.getOrigin());
			result = cb2.m_hitNormalWorld;
		}
	}

	obj->setWorldTransform(to);
	return result;


}

bool BulletPhysics::FindConfrictionObjects(std::vector<btCollisionObject*>& resultArray, btCollisionObject* myself) {
	return true;
}

//�V�^�Փ˃V�~�����[�V����
void BulletPhysics::MoveCharacterObject(btCollisionObject *obj, btVector3 &moveVector, btVector3& xAxis, btVector3& yAxis, btVector3& zAxis) {
	if (!obj->getCollisionShape()->isConvex()) {
		return;
	}

	//�ړ��͏c�Ɖ��ŕ���
	btVector3 normal;
	btTransform prevTrans = obj->getWorldTransform();
	//�܂��@�����擾���Ă݂�
	normal = MoveConvexObject(obj, -yAxis * 0.1f, 90.0f);
	if (normal.norm() >= 0.001f) {
		//����̕�����
		xAxis = normal.cross(zAxis);
		zAxis = -normal.cross(xAxis);
		yAxis = -xAxis.cross(zAxis);

	}
	obj->setWorldTransform(prevTrans);
	btVector3 virtical = moveVector.y() * yAxis;
	//�c
	normal = MoveConvexObject(obj, virtical, 40.0f);
	//��
	btVector3 horizontal = (moveVector.x()) * xAxis + (moveVector.z()) * zAxis;
	MoveConvexObject(obj, horizontal, 0.0f);
}
//�w������Ɉړ��i�ǂ���t���j
btVector3 BulletPhysics::MoveConvexObject(btCollisionObject *obj, btVector3 &moveVector, float limitAngle) {
	if (moveVector.norm() < 0.001f) {
		return btVector3(0, 0, 0);
	}

	float hitFraction = 1.0f;
	btVector3 normal;

	//�ړ�����
	normal = MoveSimulation(obj, moveVector, hitFraction);

	//�ǂ�������p�x�����m�F
	float angle_cos = (float)-normal.dot(moveVector.normalized());
	float limit_cos = (float)btCos(btRadians(limitAngle));
	if (abs(angle_cos - limit_cos) < 0.0001f) {
		angle_cos = limit_cos;
	}
	if (angle_cos > limit_cos || normal.norm() < 0.001f) {
		return normal;
	}

	//�ǂ�������
	btVector3 goVec;
	goVec.setZero();
	goVec = moveVector * (1.0f - hitFraction);
	goVec += normal * (-goVec.dot(normal));

	if (goVec.norm() < 0.001f) {
		return btVector3(0, 0, 0);
	}
	//�ړ�����
	MoveSimulation(obj, goVec, hitFraction);

	return normal;
}

//�ړ��������܂Ƃ�
btVector3 BulletPhysics::MoveSimulation(btCollisionObject *obj, btVector3 &moveVector, float &resultHitFraction){
	const float allowDistance = 0.1f;
	btConvexShape* shape = (btConvexShape*)obj->getCollisionShape();

	btTransform from = obj->getWorldTransform();
	btTransform to = from;
	to.setOrigin(to.getOrigin() + moveVector);

	//sweepTest
	MyConvexClosestCallBack convex_cb(obj);
	this->bulletWorld->convexSweepTest(shape, from, to, convex_cb, allowDistance);
	if (convex_cb.hasHit()) {
		btVector3 objPos;
		objPos.setZero();
		objPos.setInterpolate3(from.getOrigin(), to.getOrigin(), convex_cb.m_closestHitFraction);
		to.setOrigin(objPos);
		resultHitFraction = convex_cb.m_closestHitFraction;
	}
	obj->setWorldTransform(to);

	//�Փ˂����������܂Ŗ@�������ɉ����o��
	btVector3 prevPos = obj->getWorldTransform().getOrigin();
	MyContactCallBack contact_cb(obj);
	do {
		contact_cb.isHit = false;
		this->bulletWorld->contactTest(obj, contact_cb);
	} while (contact_cb.isHit);

	//�����_��l�ʈȉ���؂�̂�
	btVector3 normal = obj->getWorldTransform().getOrigin() - prevPos;
	normal.setX((int)(normal.x() * 1000.0f) / 1000.0f);
	normal.setY((int)(normal.y() * 1000.0f) / 1000.0f);
	normal.setZ((int)(normal.z() * 1000.0f) / 1000.0f);

	if (normal.norm() < 0.001f) {
		return btVector3(0, 0, 0);
	}
	else {
		normal.normalize();
	}

	return normal;
#if 0
	//�߂荞�݉������̃Y����␳�i�����o�����������u�@���v����u�ړ������̋t�v�Ɂj
	btVector3 nowPosition     = obj->getWorldTransform().getOrigin();//���݈ʒu
	btVector3 sweepMoveVector = prevPos - from.getOrigin();//sweepTest�ł̈ړ��x�N�g��
	btVector3 hPos            = prevPos + normal * -sweepMoveVector.dot(normal);//�Փ˕��ʂƕ��s�ȁAfrom�̈ʒu���܂ޓ��ꕽ�ʏ�ɉ��낵�����̑�

	btVector3 bh          = nowPosition - prevPos;
	btVector3 bhDash      = hPos - prevPos;
	btVector3 hDashp      = from.getOrigin() - hPos;

	printVec3(normal);
	if (hDashp.norm() < 0.001f) {
		return normal;
	}
	if (bhDash.norm() < 0.001f) {
		return normal;
	}
	btVector3 ha          = hDashp.normalized() * (bh.norm() * hDashp.norm()) / bhDash.norm();
	btVector3 fixPosition = nowPosition + ha;


	to.setOrigin(fixPosition);
	obj->setWorldTransform(to);

	return normal;
#endif
}
