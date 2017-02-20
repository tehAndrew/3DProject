/* Code by Andreas Palmqvist
Based on code by Frank D Luna*/

#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera() {
	mFlying = false;

	XMStoreFloat3(&mPos, XMVectorZero());
	XMStoreFloat3(&mRight, XMVectorZero());
	XMStoreFloat3(&mUp, XMVectorZero());
	XMStoreFloat3(&mLook, XMVectorZero());

	float mNearPlane   = 0.1f;
	float mFarPlane    = 1.f;
	float mAspectRatio = 1.f;
	float mFovY        = 0.1f;

	XMStoreFloat4x4(&mViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&mProjMatrix, XMMatrixIdentity());
}

void FirstPersonCamera::setView(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up) {
	// Create a view matrix.
	XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(pos, target, up));

	// Get the view vectors from the view matrix.
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

	// The walk plane is supposed to be parallell with floor plane, therefore the lookat vector has to be parallell
	// with the floor plane for the line of code below to work.
	mWalkDir = mLook;
}

void FirstPersonCamera::setProj(float fovY, float aspectRatio, float nearPlane, float farPlane) {
	// Set the projection matrix.
	mFovY        = fovY;
	mAspectRatio = aspectRatio;
	mNearPlane   = nearPlane;
	mFarPlane    = farPlane;

	XMStoreFloat4x4(&mProjMatrix, XMMatrixPerspectiveFovLH(mFovY, mAspectRatio, mNearPlane, mFarPlane));
}

XMVECTOR FirstPersonCamera::getPos() const {
	return XMLoadFloat3(&mPos);
}

XMMATRIX FirstPersonCamera::getViewMatrix() const {
	return XMMatrixTranspose(XMLoadFloat4x4(&mViewMatrix));
}

XMMATRIX FirstPersonCamera::getProjMatrix() const {
	return XMMatrixTranspose(XMLoadFloat4x4(&mProjMatrix));
}

void FirstPersonCamera::walk(float dist) {
	// Move the cameras position by a distance dist along the look axis or the walkDir axis depending on if flying is
	// enabled or not.
	XMVECTOR d    = XMVectorReplicate(dist);
	XMVECTOR pos  = XMLoadFloat3(&mPos);
	XMVECTOR walkDir;

	if (mFlying)
		walkDir = XMLoadFloat3(&mLook);
	else
		walkDir = XMLoadFloat3(&mWalkDir);

	// Multiply the distance to the directional vector and add that value to the position.
	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(d, walkDir, pos));
}

void FirstPersonCamera::strafe(float dist) {
	// Move the cameras position by a distance dist along the right axis.
	XMVECTOR d     = XMVectorReplicate(dist);
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR pos   = XMLoadFloat3(&mPos);

	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(d, right, pos));
}

void FirstPersonCamera::pitch(float angle) {
	// Rotate the up and the look value around the right vector by a certain angle.
	XMMATRIX rot = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), rot));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), rot));
}

void FirstPersonCamera::turn(float angle) {
	// Rotate all camera vectors around the world y axis.
	XMMATRIX rot = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), rot));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), rot));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), rot));
	XMStoreFloat3(&mWalkDir, XMVector3TransformNormal(XMLoadFloat3(&mWalkDir), rot));
}

void FirstPersonCamera::update(float dt) {
	// Walking
	if (!(Input::keyHold(Input::W_KEY) && Input::keyHold(Input::S_KEY))) {
		if (Input::keyHold(Input::W_KEY)) {
			walk(200.f * dt);
		}
		if (Input::keyHold(Input::S_KEY)) {
			walk(-200.f * dt);
		}
	}

	// Strafing
	if (!(Input::keyHold(Input::A_KEY) && Input::keyHold(Input::D_KEY))) {
		if (Input::keyHold(Input::A_KEY)) {
			strafe(-200.f * dt);
		}
		if (Input::keyHold(Input::D_KEY)) {
			strafe(200.f * dt);
		}
	}

	// Toggle flying mode
	if (Input::keyPressed(Input::Q_KEY))
		mFlying = !mFlying;

	// Get all camera related vectors
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR up    = XMLoadFloat3(&mUp);
	XMVECTOR look  = XMLoadFloat3(&mLook);
	XMVECTOR pos   = XMLoadFloat3(&mPos);

	XMVECTOR walkDir = XMLoadFloat3(&mWalkDir);

	// We have to reorthonormalize the view matrix because numerical errors
	// may make the matrix non-orthonormal.
	look    = XMVector3Normalize(look);
	up      = XMVector3Normalize(XMVector3Cross(look, right));
	right   = XMVector3Cross(up, look);
	walkDir = XMVector3Normalize(walkDir);

	// Get position of the camera.
	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	// Transfer back to Float3 type vectors.
	XMStoreFloat3(&mRight, right); 
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mWalkDir, walkDir);

	// Fill Matrix.
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
	// Calculate an angle to pitch or turn based on mouse movement when the mouse is captured.
	if (button == MK_LBUTTON) {
		float dx = XMConvertToRadians(0.50f * (float)(x - lastX));
		float dy = XMConvertToRadians(0.50f * (float)(y - lastY));

		pitch(dy);
		turn(dx);
	}
}