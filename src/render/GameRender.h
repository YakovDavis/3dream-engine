#pragma once

#include "nvrhi/nvrhi.h"

namespace D3E
{
	// Internal class for managing render devices, swap chains etc.
	class GameRender
	{
	public:
		void Init();

		void DestroyResources();

		GameRender() = default;
		virtual ~GameRender() = default;

	protected:
		bool InitD3D12();

		nvrhi::DeviceHandle device_;

		friend class Game;
	};
}

