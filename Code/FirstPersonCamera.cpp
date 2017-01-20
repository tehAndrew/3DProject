#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera() {
	float mStrafeDir  = 0;
	float mWalkDir    = 0;
	float mPitchAngle = 0;
	float mTurnAngle  = 0;

	XMStoreFloat3(&mPos, XMVectorZero());
	XMStoreFloat3(&mRight, XMVectorZero());
	XMStoreFloat3(&mUp, XMVectorZero());
	XMStoreFloat3(&mLook, XMVectorZero());

	float mNearPlane   = 0;
	float mFarPlane    = 0;
	float mAspectRatio = 0;
	float mFovY        = 0;

	XMStoreFloat4x4(&mViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&mProjMatrix, XMMatrixIdentity());
}

void FirstPersonCamera::setView(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up) {
	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(pos, target, up));

	mRight.x = mViewMatrix(0, 0);
	mRight.y = mViewMatrix(1, 0);
	mRight.z = mViewMatrix(2, 0);
	mPos.x   = mViewMatrix(3, 0);

	mUp.x  = mViewMatrix(0, 1);
	mUp.y  = mViewMatrix(1, 1);
	mUp.z  = mViewMatrix(2, 1);
	mPos.y = mViewMatrix(3, 1);

	mLook.x = mViewMatrix(0, 2);
	mLook.y = mViewMatrix(1, 2);
	mLook.z = mViewMatrix(2, 2);
	mPos.z  = mViewMatrix(3, 2);
}

void FirstPersonCamera::setProj(float fovY, float aspectRatio, float nearPlane, float farPlane) {
	mFovY        = fovY;
	mAspectRatio = aspectRatio;
	mNearPlane   = nearPlane;
	mFarPlane    = farPlane;

	XMStoreFloat4x4(&mProjMatrix, XMMatrixPerspectiveFovLH(mFovY, mAspectRatio, mNearPlane, mFarPlane));
}

XMMATRIX FirstPersonCamera::getViewMatrix() const {
	return XMMatrixTranspose(XMLoadFloat4x4(&mViewMatrix));
}

XMMATRIX FirstPersonCamera::getProjMatrix() const {
	return XMMatrixTranspose(XMLoadFloat4x4(&mProjMatrix));
}

// Move the cameras position by a distance dist along the look axis.
void FirstPersonCamera::walk(float dist) {
	XMVECTOR d    = XMVectorReplicate(dist);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR pos  = XMLoadFloat3(&mPos);

	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(d, look, pos));
}

// Move the cameras position by a distance dist along the right axis.
void FirstPersonCamera::strafe(float dist) {
	XMVECTOR d     = XMVectorReplicate(dist);
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR pos   = XMLoadFloat3(&mPos);

	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(d, right, pos));
}

void FirstPersonCamera::pitch(float angle) {
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), rot));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), rot));
}

void FirstPersonCamera::turn(float angle) {
	XMMATRIX rot = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), rot));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), rot));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), rot));
}

void FirstPersonCamera::update(float dt) {
	if (!(Input::keyHold(Input::W_KEY) && Input::keyHold(Input::S_KEY))) {
		if (Input::keyHold(Input::W_KEY)) {
			walk(30.f * dt);
		}
		if (Input::keyHold(Input::S_KEY)) {
			walk(-30.f * dt);
		}
	}

	if (!(Input::keyHold(Input::A_KEY) && Input::keyHold(Input::D_KEY))) {
		if (Input::keyHold(Input::A_KEY)) {
			strafe(-30.f * dt);
		}
		if (Input::keyHold(Input::D_KEY)) {
			strafe(30.f * dt);
		}
	}

	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR up    = XMLoadFloat3(&mUp);
	XMVECTOR look  = XMLoadFloat3(&mLook);
	XMVECTOR pos   = XMLoadFloat3(&mPos);

	// We have to reorthonormalize the view matrix because numerical errors
	// may make the matrix non-orthonormal.
	look  = XMVector3Normalize(look);
	up    = XMVector3Normalize(XMVector3Cross(look, right));
	right = XMVector3Cross(up, look);

	// Fill matrix
	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3(&mRight, right); 
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);

	mViewMatrix(0, 0) = mRight.x; 
	mViewMatrix(1, 0) = mRight.y;
	mViewMatrix(2, 0) = mRight.z;
	mViewMatrix(3, 0) = x;
	
	mViewMatrix(0, 1) = mUp.x;
	mViewMatrix(1, 1) = mUp.y;
	mViewMatrix(2, 1) = mUp.z;
	mViewMatrix(3, 1) = y;
	
	mViewMatrix(0, 2) = mLook.x;
	mViewMatrix(1, 2) = mLook.y;
	mViewMatrix(2, 2) = mLook.z;
	mViewMatrix(3, 2) = z;
	
	mViewMatrix(0, 3) = 0.0f;
	mViewMatrix(1, 3) = 0.0f;
	mViewMatrix(2, 3) = 0.0f;
	mViewMatrix(3, 3) = 1.0f;
}

void FirstPersonCamera::onMouseMove(WPARAM button, int x, int y, int lastX, int lastY) {
	if (button == MK_LBUTTON) {
		float dx = XMConvertToRadians(0.50f * (float)(lastX - x));
		float dy = XMConvertToRadians(0.50f * (float)(lastY - y));

		pitch(dy);
		turn(dx);
	}
}