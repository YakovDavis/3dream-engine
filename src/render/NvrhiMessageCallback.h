#pragma once

#include "nvrhi/nvrhi.h"
#include "D3E/Debug.h"

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
						Debug::LogMessage(String("[NVRHI] ") +
						                       messageText);
						break;
					case MessageSeverity::Warning:
						Debug::LogWarning(String("[NVRHI] ") +
						                       messageText);
						break;
					case MessageSeverity::Error:
						Debug::LogError(String("[NVRHI] ") +
						                     messageText);
						break;
					case MessageSeverity::Fatal:
						Debug::LogError(String("[NVRHI] ") +
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
