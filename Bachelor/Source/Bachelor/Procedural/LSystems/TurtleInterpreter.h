// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TurtleInterpreter.generated.h"

struct FBranch;

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

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	void StartInterpretation(FBranch** RootBranch, FString LSystemResult);

private:

	void ConstructFunctionMap();

	void InterpreteLSystemResult();

#pragma region FunctionMap functions
	virtual void SetVertical();

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

#pragma endregion

	typedef void (ATurtleInterpreter::*ZeroArgFunctionPtrType)(void);
	typedef void (ATurtleInterpreter::*OneArgFunctionPtrType)(float value);
	typedef void (ATurtleInterpreter::*TwoArgFunctionPtrType)(float first, float second);

	bool CheckFunctions(int CurrentCharIndex, int& OutNumCharsToSkip);
	bool CheckZeroArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip);
	bool CheckOneArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip);
	bool CheckTwoArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip);

	// https://wiki.unrealengine.com/Function_Pointers
	TMap<TCHAR, ZeroArgFunctionPtrType> ZeroArgumentFunctionMap;
	TMap<TCHAR, OneArgFunctionPtrType> OneArgumentFunctionMap;
	TMap<TCHAR, TwoArgFunctionPtrType> TwoArgumentFunctionMap;

	FVector CurrentPosition;
	FQuat CurrentRotation;
	FBranch* CurrentBranch;

	// do stacking by stacking data structs with Position, Rotation and FBranch
	TArray<FVector> PositionStack;
	TArray<FRotator> RotationStack;
	TArray<FBranch*> BranchStack;


	FString StringToInterpret;
};
