#pragma once

UENUM(BlueprintType)
enum class EPlayerTeamType : uint8
{
	None = 0,
	Red,
	Blue,
	Neutral
};

UENUM(BlueprintType)
enum class EPlayerWeaponType : uint8
{
	Axe = 0,
	Bow
};

UENUM(BlueprintType)
enum class EPlayerCurrentState : uint8
{
	Alive = 0,
	Dead,
	OnBoard
};

UENUM(BlueprintType, Blueprintable)
enum class EActorType : uint8
{
	Player = 0,
	Minion,
	Construction,
	Turret,
	Vehicle,
	InteractableItem,
	Sanctum,
	GatewayTurret
};

UENUM(BlueprintType)
enum class ESeatType : uint8
{
	None = 0,
	Primary,
	Secondary,
	NoSeat
};

UENUM(BlueprintType)
enum class EArtefactType : uint8
{
	None = 0,
	HoldArtefact,
	UseArtefact
};

UENUM(BlueprintType)
enum class EArtefactHowToUse : uint8
{
	None = 0,
	Active,
	Trigger,
	Passive
};

UENUM(BlueprintType)
enum class EArtefactTargetType : uint8
{
	None = 0,
	Range,
	Myself
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Arti_Horn,
	Arti_Root,
	Arti_Feather,
	Arti_Beer,
	Arti_Grace,
	Arti_Jar
};

UENUM(BlueprintType)
enum class EItemEffectType : uint8
{
	First = 0,
	Second
};

UENUM(BlueprintType)
enum class EArtefactEffectType : uint8
{
	None = 0,
	phDmg,
	hpRegen,
	atkSpeed,
	Hp,
	phDef
};

UENUM(BlueprintType)
enum class EMinionType : uint8
{
	Melee = 0,
	Range
};

UENUM(BlueprintType)
enum class ECharacterSkillType : uint8
{
	None = 0,
	LeftClick,
	RightClick,
	QKey,
	EKey,
	RKey
};