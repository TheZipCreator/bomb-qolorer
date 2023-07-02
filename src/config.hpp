#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/GameObject.hpp"

DECLARE_CONFIG(ModConfig,
	CONFIG_VALUE(enabled, bool, "Enabled", true);
	CONFIG_VALUE(bombColor, UnityEngine::Color, "Bomb Color", UnityEngine::Color(0.25, 0.25, 0.25, 1));
)

#endif
