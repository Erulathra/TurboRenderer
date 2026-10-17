#pragma once

namespace Turbo
{
	template <typename Function, typename... TArgs>
	constexpr auto Invoke(Function&& function, TArgs&&... args) -> decltype(((Function&&)function)((TArgs&&)args...))
	{
		return ((Function&&)function)((TArgs&&)args...);
	}
}
