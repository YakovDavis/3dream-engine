#pragma once

#include "nvrhi/nvrhi.h"
#include "Debug.h"

namespace D3E
{
	class NvrhiMessageCallback final : public nvrhi::IMessageCallback
	{
	public:
		void message(nvrhi::MessageSeverity severity, const char* messageText)
		{
			{
				using nvrhi::MessageSeverity;
				switch (severity)
				{
					case MessageSeverity::Info:
						Debug::LogMessage(eastl::string("[NVRHI]") +
						                       messageText);
						break;
					case MessageSeverity::Warning:
						Debug::LogWarning(eastl::string("[NVRHI]") +
						                       messageText);
						break;
					case MessageSeverity::Error:
						Debug::LogError(eastl::string("[NVRHI]") +
						                     messageText);
						break;
					case MessageSeverity::Fatal:
						Debug::LogError(eastl::string("[NVRHI]") +
						                     messageText);
						break;
					default:;
				}
			}

			if (severity == nvrhi::MessageSeverity::Fatal)
			{
				Debug::Assert(true, messageText);
			}
		}
	};
}
