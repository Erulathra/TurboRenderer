#pragma once

#include "DataStructures/Arena.h"

namespace Turbo
{
	struct GPUDevice;
	struct Graphics;

	enum struct EventType : u32
	{
		Sigint,
	};

	struct EventData
	{
	};

	struct Engine
	{
		/* Data */
		Arena persistentMemory = {};
		Arena transientMemory = {};

		void* NameDB = nullptr;
		PlatformContext* platformContext = nullptr;
		GPUDevice* gpu = nullptr;

		Graphics* graphics = nullptr;

		bool shouldTick = false;

		/* API */
		void Init(void* permanent, u64 permanentSize, void* transient, u64 transientSize);
		void Shutdown();

		bool Tick();

		void PushEvent(EventType type, const EventData& eventData);
		void OnEvent(EventType type, const EventData& eventData);
	};
} // namespace Turbo
