// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LSystemInterpreter.generated.h"

#define INTERPRETER_ERROR_NUMBER_OF_BRACKETS "ERROR_NUMBER_OF_BRACKETS"
#define INTERPRETER_ERROR_FIRST_CHAR_IS_NOT_A_BRACKET "INTERPRETER_ERROR_FIRST_CHAR_IS_NOT_A_BRACKET"


UCLASS()
class BACHELOR_API ULSystemInterpreter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:


	static FString GetContentBetweenBrackets(FString* Current, int BracketPositionIndex);

	static TArray<FString> GetAttributesBetweenBrackets(FString* Current, int BracketPositionIndex);




};
