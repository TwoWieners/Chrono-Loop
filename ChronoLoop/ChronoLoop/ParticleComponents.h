#pragma once
#include <list>
#include <vector>
#include <string>
#include "Rendering\TextureManager.h"
#include "Rendering\Structures.h"
#include "Common\FileIO.h"
#include <wrl/client.h>


struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct ID3D11InputLayout;

namespace Epoch
{
	struct GSParticle
	{
		vec4f pos;
		float size;
		float pad[3];
	};
	struct GSMatrix
	{
		matrix4 model;
	};

	struct Particle
	{
		int mLife;
		float mSize;
		vec4f mPos, mPrevPos, mVelocity;
		vec4f mSColor, mEColor, mCurColor;
		bool mActive;

		//Constructors
		Particle();
		Particle(int _life, float _size, vec4f _pos, vec4f _color);
		Particle(int _life, float _size, vec4f _pos, vec4f _scolor, vec4f _ecolor);

		Particle& operator=(const Particle& _other);
	};

	class ParticleEmitter
	{
		long mID;
		char* mName;

	private:
		const char* mTName;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tv;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> text;
		ID3D11Buffer* mVBuffer;
		ID3D11InputLayout* mILayout;
		//BlendMode
		//BoundingBox
		vec4f mPos;

		Particle* mBase;
		std::list<Particle*> mParticles;
		std::vector<GSParticle> mGParticles;
		int mTotalParticles, mMaxParticles, mPerSec;
		vec4f mStartColor, mEndColor;

		virtual void UpdateParticle(Particle* _p);
		void CreateBuffers();
		void CreateTextureResource();
		void UpdateBuffers();
		void CleanUpParticles();
		void EmitParticles();

	public:
		ParticleEmitter(int _numParticles, int _totalp, int _maxp, int _persec, vec4f _pos);
		virtual ~ParticleEmitter();

		ID3D11Buffer* GetVertexBuffer();
		ID3D11ShaderResourceView* GetTexture();
		int GetVertexCount();

		void SetTexture(const char* _tex);
		virtual void SetParticle(Particle* _p);

		virtual void Update(float _delta);

	};
}