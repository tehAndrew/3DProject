/* Code by Andreas Palmqvist
   Based on code by Frank D Luna*/

#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include <windows.h>
#include <DirectXMath.h>

#include "Input.h"

using namespace DirectX;

class FirstPersonCamera {
	private:
		// Camera 
		XMFLOAT3 mPos;
		XMFLOAT3 mRight;
		XMFLOAT3 mUp;
		XMFLOAT3 mLook;

		// Walking/Flying
		XMFLOAT3 mWalkDir; // Defines a plane the camera is restricted to
		bool     mFlying;

		// Projection (Lens)
		float mNearPlane;
		float mFarPlane;
		float mAspectRatio;
		float mFovY;

		// Matrices
		XMFLOAT4X4 mViewMatrix;
		XMFLOAT4X4 mProjMatrix;

	public:
		FirstPersonCamera();
		virtual ~FirstPersonCamera() {};

		// Set matrices.
		void setView(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up);
		void setProj(float fovY, float aspectRatio, float nearPlane, float farPlane);

		XMVECTOR getPos() const;

		// Get matrices.
		XMMATRIX getViewMatrix() const;
		XMMATRIX getProjMatrix() const;

		// Move camera.
		void walk(float dist);   // Walk forward or backward.
		void strafe(float dist); // Walk left or right.

		// Rotate camera.
		void pitch(float angle); // Look up or down.
		void turn(float angle);  // Look left or right.

		void update(float dt);

		// Event.
		void onMouseMove(WPARAM button, int x, int y, int lastX, int lastY);
};

#endif