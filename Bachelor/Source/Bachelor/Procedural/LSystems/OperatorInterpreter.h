// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OperatorInterpreter.generated.h"


UCLASS()
class BACHELOR_API AOperatorInterpreter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOperatorInterpreter();

	virtual ~AOperatorInterpreter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	bool CheckFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip);

private:

	void ConstructFunctionMap();


#pragma region FunctionMap functions

	virtual float Multiplicate(float first, float second);

	virtual bool Lower(float first, float second);
	virtual bool LowerEqual(float first, float second);

	virtual bool Greater(float first, float second);
	virtual bool GreaterEqual(float first, float second);

	virtual bool Equal(float first, float second);

#pragma endregion

	typedef float (AOperatorInterpreter::*TwoArgMathOperatorPtrType)(float first, float second);

	typedef bool (AOperatorInterpreter::*TwoArgLogicOperatorPtrType)(float first, float second);


	void CheckAllAttributesForOperators(TArray<FString>& OutAttributes);
	FString CheckForMathOperators(FString* Attribute);
	FString CheckForLogicOperators(FString* Attribute);

	int FindIndexOfNextMathOperator(FString* Attribute, int LastOperatorIndex, TwoArgMathOperatorPtrType** OutOperatorPtrType);
	int FindIndexOfNextLogicOperator(FString* Attribute, int LastOperatorIndex, TwoArgLogicOperatorPtrType** OutOperatorPtrType);

	// https://wiki.unrealengine.com/Function_Pointers
	TMap<TCHAR, TwoArgMathOperatorPtrType> TwoArgumentMathOperatorMap;
	TMap<TCHAR, TwoArgLogicOperatorPtrType> TwoArgumentLogicOperatorMap;

};
