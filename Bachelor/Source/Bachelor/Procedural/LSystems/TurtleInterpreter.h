// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TurtleInterpreter.generated.h"

struct FBranch;
struct FTurtleState;
struct FLSystemData;

DECLARE_STATS_GROUP(TEXT("TurtleInterpreter"), STATGROUP_TurtleInterpreter, STATCAT_Advanced);

UCLASS()
class BACHELOR_API ATurtleInterpreter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurtleInterpreter();

	virtual ~ATurtleInterpreter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void StartInterpretation(FBranch** RootBranch, FLSystemData LSystemData, FString* LSystemResult);

private:

	void ConstructFunctionMap();

	void Interprete(FString* ToInterprete);

#pragma region FunctionMap functions
	virtual void SetVertical();

	virtual void PushTurtleState();
	virtual void PopTurtleState();

	virtual void ConstructBranch(float length);

	virtual void Turn(float angle);
	virtual void TurnLeft(float angle);
	virtual void TurnRight(float angle);

	virtual void Pitch(float angle);
	virtual void PitchDown(float angle);
	virtual void PitchUp(float angle);

	virtual void Roll(float angle);
	virtual void RollLeft(float angle);
	virtual void RollRight(float angle);

	virtual float Multiplicate(float first, float second);
	virtual float Divide(float first, float second);
	virtual float Exponent(float first, float second);
	virtual float Add(float first, float second);
	virtual float Substract(float first, float second);
	
#pragma endregion

	typedef void (ATurtleInterpreter::*ZeroArgFunctionPtrType)(void);
	typedef void (ATurtleInterpreter::*OneArgFunctionPtrType)(float value);
	typedef float (ATurtleInterpreter::*TwoArgOperatorPtrType)(float first, float second);

	bool CheckFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip);

	bool CheckZeroArgFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip);

	bool CheckOneArgFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip);

	void CheckAllAttributesForOperators(TArray<FString>& OutAttributes);
	FString CheckForMathOperators(FString* Attribute);
	int FindIndexOfNextOperator(FString* Attribute, int LastOperatorIndex, TwoArgOperatorPtrType** OutOperatorPtrType);

	bool TryCallOneArgFunction(TCHAR* CurrentChar, TArray<FString> Attributes, OneArgFunctionPtrType* FunctionPtr, int& OutNumCharsToSkip);


	// https://wiki.unrealengine.com/Function_Pointers
	TMap<TCHAR, ZeroArgFunctionPtrType> ZeroArgumentFunctionMap;
	TMap<TCHAR, OneArgFunctionPtrType> OneArgumentFunctionMap;
	TMap<TCHAR, TwoArgOperatorPtrType> TwoArgumentOperatorMap;

	FVector Tropism;
	float BendingByTropismParameter;

	FVector CurrentPosition;
	FQuat CurrentRotation;
	FBranch* CurrentBranch;

	TArray<FTurtleState*> TurtleStateStack;

	FString CompleteStringToInterprete;
};
