// Fill out your copyright notice in the Description page of Project Settings.


#include "Others/ValhallaGameplayTags.h"

namespace ValhallaGameplayTags
{
	// Pawn
	UE_DEFINE_GAMEPLAY_TAG(Actor_Pawn_Player, "Actor.Pawn.Player");
	UE_DEFINE_GAMEPLAY_TAG(Actor_Pawn_Minion, "Actor.Pawn.Minion");

	// Construction
	UE_DEFINE_GAMEPLAY_TAG(Actor_Construction_Gateway, "Actor.Construction.Gateway");
	UE_DEFINE_GAMEPLAY_TAG(Actor_Construction_Wall, "Actor.Construction.Wall");
	UE_DEFINE_GAMEPLAY_TAG(Actor_Construction_Nexus, "Actor.Construction.Nexus");
	UE_DEFINE_GAMEPLAY_TAG(Actor_Construction_Turret, "Actor.Construction.Turret");


	UE_DEFINE_GAMEPLAY_TAG(Actor_SiegeWeapons_BatteringRam, "Actor.SiegeWeapons.BatteringRam");
	UE_DEFINE_GAMEPLAY_TAG(Actor_SiegeWeapons_SiegeGun, "Actor.SiegeWeapons.SiegeGun");
}
