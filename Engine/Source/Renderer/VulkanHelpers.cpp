#include "VulkanHelpers.h"

namespace Turbo
{
	LiteralString EnumToString(VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			return "General";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			return "Validation";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			return "Performance";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
			return "Device Address Binding";
			invalidDefaultCase
		}

		return "Invalid";
	}

	StringView FlagsToString(Arena& arena, VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		u32 flags = messageType;
		StringView result = String::BeginString(arena);

		if (messageType != 0)
		{
			while (flags != 0)
			{
				const u32 trailingZeros = CountTrailingZeros(flags);
				const u32 flagValue = 1u << trailingZeros;

				flags &= ~flagValue;

				LiteralString FlagName = EnumToString((VkDebugUtilsMessageTypeFlagsEXT)flagValue);
				String::AppendString(arena, result, String::MakeStringView(FlagName));
				String::AppendString(arena, result, LiteralStringView("|"));
			}

			String::ChopRight(result, 1);
		}

		return result;
	}

	VkImageLayout FindLayout(ETexState state, EPixFormat format)
	{
		VkImageLayout outLayout = VK_IMAGE_LAYOUT_GENERAL;
		const bool bDepthStencil = HasDepthOrStencil(format);

		if (state == ETexState::None)
		{
			outLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		else if (bDepthStencil)
		{
			if ((state & (ETexState::AllSRV | ETexState::RTVDSVRead)) != 0)
			{
				outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}
			else
			{
				outLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
		}
		else if ((state & ETexState::AllRTVDSV) != 0)
		{
			outLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		else if ((state & ETexState::AllUAV) != 0)
		{
			outLayout = VK_IMAGE_LAYOUT_GENERAL;
		}
		else if ((state & ETexState::AllSRV) != 0)
		{
			outLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		else if (state == ETexState::TransferSrc)
		{
			outLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		}
		else if (state == ETexState::TransferDst)
		{
			outLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		else if (state == ETexState::Present)
		{
			outLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}

		return outLayout;
	}

	void FindStageAccess(ETexState state, EPixFormat format, VkPipelineStageFlagBits2& outStage, VkAccessFlagBits2& outAccess)
	{
		outStage = 0;
		outAccess = 0;

		if (state == ETexState::None)
		{
			outAccess = VK_ACCESS_2_NONE;
			outStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			return;
		}

		const bool bDepthStencil = HasDepthOrStencil(format);

		if ((state & ETexState::SRVGraphics) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			outAccess |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if ((state & ETexState::UAVGraphics) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			outAccess |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if ((state & ETexState::SRVCompute) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			outAccess |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if ((state & ETexState::UAVCompute) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
			outAccess |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if ((state & ETexState::SRVTraceRays) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
			outAccess |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if ((state & ETexState::UAVTraceRays) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
			outAccess |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if ((state & ETexState::RTVDSVRead) != 0)
		{
			if (bDepthStencil)
			{
				outStage |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
				outAccess |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			}
			else
			{
				outStage |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				outAccess |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			}
		}

		if ((state & ETexState::RTVDSVWrite) != 0)
		{
			if (bDepthStencil)
			{
				outStage |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
				outAccess |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}
			else
			{
				outStage |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				outAccess |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
			}
		}

		if ((state & ETexState::TransferSrc) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			outAccess |= VK_ACCESS_2_TRANSFER_READ_BIT;
		}

		if ((state & ETexState::TransferDst) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
			outAccess |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}

		if ((state & ETexState::Present) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
			outAccess |= VK_ACCESS_2_MEMORY_READ_BIT;
		}

		check(outStage != 0 && outAccess != 0);
	}

	void FindStageAccess(EBufState state, VkPipelineStageFlagBits2& outStage, VkAccessFlagBits2& outAccess)
	{
		outStage = 0;
		outAccess = 0;

		if (state == EBufState::None)
		{
			outAccess = VK_ACCESS_2_NONE;
			outStage = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
			return;
		}

		if ((state & EBufState::AllUniform) != 0)
		{
			outAccess |= VK_ACCESS_2_UNIFORM_READ_BIT;
		}

		if ((state & EBufState::ShaderRead) != 0)
		{
			outAccess |= VK_ACCESS_2_SHADER_READ_BIT;
		}

		if ((state & EBufState::ShaderWrite) != 0)
		{
			outAccess |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}

		if ((state & EBufState::AllGraphics) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT
				| VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT
				| VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT | VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT
				| VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		}

		if ((state & EBufState::VertexOrIndex) != 0)
		{
			outAccess |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT;
			outStage |= VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
		}

		if ((state & EBufState::AllIndirect) != 0)
		{
			outAccess |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
			outStage |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
		}

		if ((state & EBufState::TransferSrc) != 0)
		{
			outAccess |= VK_ACCESS_2_TRANSFER_READ_BIT;
			outStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		}

		if ((state & EBufState::TransferDst) != 0)
		{
			outAccess |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
			outStage |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
		}

		if ((state & EBufState::AccelerationStructureBuild) != 0)
		{
			outAccess |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
			outStage |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		}

		if ((state & EBufState::ShaderBindingTable) != 0)
		{
			outAccess |= VK_ACCESS_2_SHADER_BINDING_TABLE_READ_BIT_KHR;
			outStage |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
		}

		if ((state & EBufState::AccelerationStructureBuildScratch) != 0)
		{
			outAccess |= VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
			outStage |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		}

		if ((state & (EBufState::AllCompute & ~EBufState::IndirectCompute)) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		}

		if ((state & (EBufState::AllTraceRays & ~EBufState::IndirectTraceRays)) != 0)
		{
			outStage |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
		}

		check(outStage != 0 && outAccess != 0);
	}
} // namespace Turbo
