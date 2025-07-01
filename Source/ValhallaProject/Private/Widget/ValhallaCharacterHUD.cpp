// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ValhallaCharacterHUD.h"
#include "Components/Image.h"

void UValhallaCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

    if (Image_SkillCoolTime2)
    {
        Image_SkillCoolTime2->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Image_SkillCoolTime3)
    {
        Image_SkillCoolTime3->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Image_SkillCoolTime4)
    {
        Image_SkillCoolTime4->SetVisibility(ESlateVisibility::Hidden);
    }

    if (Image_SkillCoolTime5)
    {
        Image_SkillCoolTime5->SetVisibility(ESlateVisibility::Hidden);
    }
}
