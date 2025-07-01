// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace ValhallaGameplayTags
{
	// Pawn
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Pawn_Player);
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Pawn_Minion);

	// Construction
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Construction_Gateway);
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Construction_Wall);
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Construction_Nexus);
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Construction_Turret);

	// Siege Weapons
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_SiegeWeapons_BatteringRam);
	VALHALLAPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_SiegeWeapons_SiegeGun);
}
