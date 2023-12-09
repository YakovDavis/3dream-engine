#pragma once

namespace D3E
{
	struct UserScript
	{
		virtual void Init(){};
		virtual void Start(){};
		virtual void Update(float dT){};
		virtual void OnDestroy(){};
	};
} // namespace D3E