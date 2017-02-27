// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "TurtleInterpreter.h"
#include "Bachelor/Utility/LSystemInterpreter.h"
#include "Data/TurtleState.h"
#include "Data/LSystemData.h"
#include "Procedural/Branch.h"


// Sets default values
ATurtleInterpreter::ATurtleInterpreter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ConstructFunctionMap();

}

ATurtleInterpreter::~ATurtleInterpreter() {

}

// Called when the game starts or when spawned
void ATurtleInterpreter::BeginPlay()
{
	
}

// Called every frame
void ATurtleInterpreter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ATurtleInterpreter::StartInterpretation(FBranch** RootBranch, FLSystemData LSystemData, FString* LSystemResult) {

	this->CurrentBranch = *RootBranch;
	this->Tropism = LSystemData.Tropism;
	this->BendingByTropismParameter = LSystemData.BendingByTropismParameter;
	
	CurrentBranch->End = CurrentBranch->Start + FVector::UpVector * 10;

	CurrentPosition = GetActorLocation();
	CurrentRotation = GetActorQuat();

	Interprete(LSystemResult);
}

DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ Interpretation"), STAT_Interpretation, STATGROUP_TurtleInterpreter);
void ATurtleInterpreter::Interprete(FString* ToInterprete){
	{
		SCOPE_CYCLE_COUNTER(STAT_Interpretation);
		for (int i = 0; i < (*ToInterprete).Len(); ++i) {
			int NumCharsToSkip = 0;
			bool wasProcessed = CheckFunctions(ToInterprete, i, NumCharsToSkip);
			if (wasProcessed) {
				i += NumCharsToSkip;
			}
		}
	}
}

void ATurtleInterpreter::ConstructFunctionMap() {
	// functions with no arguments
	ZeroArgumentFunctionMap.Add('$', &ATurtleInterpreter::SetVertical);
	ZeroArgumentFunctionMap.Add('[', &ATurtleInterpreter::PushTurtleState);
	ZeroArgumentFunctionMap.Add(']', &ATurtleInterpreter::PopTurtleState);

	// functions with one argument
	OneArgumentFunctionMap.Add('F', &ATurtleInterpreter::ConstructBranch);

	OneArgumentFunctionMap.Add('+', &ATurtleInterpreter::TurnLeft);
	OneArgumentFunctionMap.Add('-', &ATurtleInterpreter::TurnRight);

	OneArgumentFunctionMap.Add('&', &ATurtleInterpreter::PitchDown);
	OneArgumentFunctionMap.Add('^', &ATurtleInterpreter::PitchUp);

	OneArgumentFunctionMap.Add('/', &ATurtleInterpreter::RollLeft);
	OneArgumentFunctionMap.Add('\\', &ATurtleInterpreter::RollRight);

	// functions with two arguments
	TwoArgumentOperatorMap.Add('*', &ATurtleInterpreter::Multiplicate);
	TwoArgumentOperatorMap.Add('/', &ATurtleInterpreter::Divide);
	TwoArgumentOperatorMap.Add('^', &ATurtleInterpreter::Exponent);
	TwoArgumentOperatorMap.Add('+', &ATurtleInterpreter::Add);
	TwoArgumentOperatorMap.Add('-', &ATurtleInterpreter::Substract);

}

#pragma region FunctionMap functions

void ATurtleInterpreter::PushTurtleState() {
	FTurtleState* newState = new FTurtleState();
	newState->StatePosition = CurrentPosition;
	newState->StateRotation = CurrentRotation;
	newState->StateBranch = CurrentBranch;

	TurtleStateStack.Push(newState);
}

void ATurtleInterpreter::PopTurtleState() {
	if (TurtleStateStack.Num() > 0) {
		FTurtleState* toReturnTo = TurtleStateStack.Pop();

		CurrentPosition = toReturnTo->StatePosition;
		CurrentRotation = toReturnTo->StateRotation;

		if (NULL != toReturnTo->StateBranch) {
			CurrentBranch = toReturnTo->StateBranch;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Null Pointer was saved in StateBranch, can't recover correct branch during PopTurtleState()."));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Tried to pop Turtle State without any states left - there is a Branch-Bracket error in the L-System!"));
	}
}

void ATurtleInterpreter::SetVertical() {
	FVector turtleWorldLeft = CurrentRotation.RotateVector(-FVector::RightVector).GetSafeNormal();
	FVector turtleWorldProjX = turtleWorldLeft.ProjectOnToNormal(FVector::UpVector);
	FVector turtleWorldProjY = turtleWorldLeft.ProjectOnToNormal(-FVector::RightVector);

	FVector turtleWorldProjHorizontal = turtleWorldProjX + turtleWorldProjY;
	turtleWorldProjHorizontal = turtleWorldProjHorizontal.GetSafeNormal();

	float angleToRoll = FMath::Acos(FVector::DotProduct(turtleWorldLeft, turtleWorldProjHorizontal));
	RollLeft(angleToRoll);
}

void ATurtleInterpreter::ConstructBranch(float length) {
	FBranch* newBranch = new FBranch();
	
	CurrentBranch->ChildBranches.Add(newBranch);
	newBranch->ParentBranch = CurrentBranch;
	newBranch->Start = CurrentBranch->End;

	FVector turtleForwardVec = CurrentRotation.RotateVector(FVector::UpVector);

	newBranch->End = newBranch->Start + length * (turtleForwardVec + BendingByTropismParameter * Tropism).GetSafeNormal();
	newBranch->BranchDepth = TurtleStateStack.Num();

	CurrentPosition = newBranch->End;
	CurrentBranch = newBranch;

	
	
}


void ATurtleInterpreter::Turn(float angle) {
	CurrentRotation *= FQuat(-FVector::ForwardVector, FMath::DegreesToRadians(angle));

	CurrentRotation.Normalize();
}
void ATurtleInterpreter::TurnLeft(float angle) {
	Turn(-angle);
}
void ATurtleInterpreter::TurnRight(float angle) {
	Turn(angle);
}

void ATurtleInterpreter::Pitch(float angle) {
	CurrentRotation *= FQuat(-FVector::RightVector, FMath::DegreesToRadians(angle));
	CurrentRotation.Normalize();
}
void ATurtleInterpreter::PitchDown(float angle) {
	Pitch(-angle);
}
void ATurtleInterpreter::PitchUp(float angle) {
	Pitch(angle);
}

void ATurtleInterpreter::Roll(float angle) {
	
	CurrentRotation *= FQuat(FVector::UpVector, FMath::DegreesToRadians(angle));

	CurrentRotation.Normalize();
}
void ATurtleInterpreter::RollLeft(float angle) {
	Roll(-angle);
}
void ATurtleInterpreter::RollRight(float angle) {
	Roll(angle);
}

float ATurtleInterpreter::Multiplicate(float first, float second) {
	return first * second;
}

float ATurtleInterpreter::Divide(float first, float second) {
	return first / second;
}

float ATurtleInterpreter::Exponent(float first, float second) {
	return FMath::Pow(first, second);
}

float ATurtleInterpreter::Add(float first, float second) {
	return first + second;
}

float ATurtleInterpreter::Substract(float first, float second) {
	return first - second;
}

#pragma endregion
DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ CheckFunctions"), STAT_CheckFunctions, STATGROUP_TurtleInterpreter);
bool ATurtleInterpreter::CheckFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	{
		SCOPE_CYCLE_COUNTER(STAT_CheckFunctions);

		// this looks so freaking stupid.
		bool wasProcessed = CheckZeroArgFunctions(ToInterprete, CurrentCharIndex, OutNumCharsToSkip);
		if (wasProcessed) {
			return wasProcessed;
		}
		wasProcessed = CheckOneArgFunctions(ToInterprete, CurrentCharIndex, OutNumCharsToSkip);
		return wasProcessed;
	}
}

DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ ZeroArgFunctions"), STAT_ZeroArgFunctions, STATGROUP_TurtleInterpreter);
bool ATurtleInterpreter::CheckZeroArgFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	SCOPE_CYCLE_COUNTER(STAT_ZeroArgFunctions);

	bool wasProcessed = false;
	TCHAR currentChar = (*ToInterprete)[CurrentCharIndex];
	ZeroArgFunctionPtrType* functionPtr = ZeroArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		(this->*(*functionPtr))();
		wasProcessed = true;
		OutNumCharsToSkip = 0;
	}
	return wasProcessed;
}
DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ OneArgFunctions"), STAT_OneArgFunctions, STATGROUP_TurtleInterpreter);
bool ATurtleInterpreter::CheckOneArgFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	SCOPE_CYCLE_COUNTER(STAT_OneArgFunctions);
	bool wasProcessed = false;
	TCHAR currentChar = (*ToInterprete)[CurrentCharIndex];
	OneArgFunctionPtrType* functionPtr = OneArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		int bracketPositionIndex = CurrentCharIndex + 1;
		TArray<FString> attributes = ULSystemInterpreter::GetAttributesBetweenBrackets(ToInterprete, bracketPositionIndex);
		if (1 != attributes.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("Number of Parameters for Function \"%s\" is %d, was given %d. Index: %d"), &currentChar, 1, attributes.Num(), CurrentCharIndex);
		}
		else {
			wasProcessed = TryCallOneArgFunction(&currentChar, attributes, functionPtr, OutNumCharsToSkip);
		}
	}
	return wasProcessed;
}

void ATurtleInterpreter::CheckAllAttributesForOperators(TArray<FString>& OutAttributes) {
	for (int i = 0; i < OutAttributes.Num(); ++i) {
		FString attribute = OutAttributes[i];
		OutAttributes[i] = CheckForMathOperators(&attribute);
	}
}

DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ MathOperators"), STAT_MathOperators, STATGROUP_TurtleInterpreter);
FString ATurtleInterpreter::CheckForMathOperators(FString* Attribute) {
	{
		SCOPE_CYCLE_COUNTER(STAT_MathOperators);

		TwoArgOperatorPtrType* currentOperatorPtr = NULL;
		TwoArgOperatorPtrType* lastOperatorPtr = NULL;

		int lastOperatorIndex = FindIndexOfNextOperator(Attribute, 0, &lastOperatorPtr);
		FString resultString = (*Attribute).Left(lastOperatorIndex);
		int currentOperatorIndex = FindIndexOfNextOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
		while (currentOperatorIndex != lastOperatorIndex) {
			if (resultString.IsNumeric()) {
				float firstParameter = FCString::Atof(*resultString);

				int secondParameterStart = lastOperatorIndex + 1;
				int secondParameterCount = currentOperatorIndex - secondParameterStart;
				FString secondParameterString = (*Attribute).Mid(secondParameterStart, secondParameterCount);

				if (secondParameterString.IsNumeric()) {
					float secondParameter = FCString::Atof(*secondParameterString);

					float result = (this->*(*lastOperatorPtr))(firstParameter, secondParameter);
					resultString = FString::SanitizeFloat(result);
				}
			}
			lastOperatorIndex = currentOperatorIndex;
			lastOperatorPtr = currentOperatorPtr;
			currentOperatorIndex = FindIndexOfNextOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
		}
		return resultString;
	}
}

int ATurtleInterpreter::FindIndexOfNextOperator(FString* Attribute, int LastOperatorIndex, TwoArgOperatorPtrType** OutOperatorPtrType) {
	int nextOperatorIndex = LastOperatorIndex;
	for (int i = LastOperatorIndex + 1; i < (*Attribute).Len(); ++i) {
		TCHAR currentChar = (*Attribute)[i];
		nextOperatorIndex = i;
		(*OutOperatorPtrType) = TwoArgumentOperatorMap.Find(currentChar);
		if (NULL != (*OutOperatorPtrType)) {
			return nextOperatorIndex;
		}
	}
	if (nextOperatorIndex != LastOperatorIndex) {
		nextOperatorIndex++;
	}
	return nextOperatorIndex;
}

DECLARE_CYCLE_STAT(TEXT("TurtleInterpreter ~ TryCallOneArgFunction"), STAT_TryCallOneArgFunction, STATGROUP_TurtleInterpreter);
bool ATurtleInterpreter::TryCallOneArgFunction(TCHAR* CurrentChar, TArray<FString> Attributes, OneArgFunctionPtrType* FunctionPtr, int& OutNumCharsToSkip) {
	SCOPE_CYCLE_COUNTER(STAT_TryCallOneArgFunction);

	bool wasProcessed = false;

	int numOfBrackets = 2;
	OutNumCharsToSkip += Attributes[0].Len() + numOfBrackets;

	CheckAllAttributesForOperators(Attributes);
	if (Attributes[0].IsNumeric()) {
		float firstAttribute = FCString::Atof(*Attributes[0]);
		(this->*(*FunctionPtr))(firstAttribute);
		wasProcessed = true;
	}
	else {
		OutNumCharsToSkip = 0;
		// Using TCHAR* looks silly, but otherwise it won't be copied correctly.
		UE_LOG(LogTemp, Warning, TEXT("Expected Numerical Input for Function \"%s\", was given \"%s\""), CurrentChar, *Attributes[0]);
	}
	return wasProcessed;
}
