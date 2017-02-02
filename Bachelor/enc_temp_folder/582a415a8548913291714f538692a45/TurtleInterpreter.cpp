// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "TurtleInterpreter.h"
#include "Bachelor/Utility/LSystemInterpreter.h"
#include "Data/TurtleState.h"
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
	CurrentPosition = GetActorLocation();
	CurrentRotation = GetActorRotation().Quaternion();
}

// Called every frame
void ATurtleInterpreter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ATurtleInterpreter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	/*if ((PropertyName == GET_MEMBER_NAME_CHECKED(ALSystemPlant, MaxNumGrowthIterations))) {
		// Potential place to regenerate tree
	}*/

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ATurtleInterpreter::StartInterpretation(FBranch** RootBranch, FString LSystemResult) {
	this->CurrentBranch = *RootBranch;
	this->StringToInterpret = LSystemResult;

	SetVertical();
	Interprete(LSystemResult);
}

void ATurtleInterpreter::Interprete(FString ToInterprete){
	for (int i = 0; i < ToInterprete.Len(); ++i) {
		TCHAR currentChar = ToInterprete[i];
		int NumCharsToSkip = 0;
		bool wasProcessed = CheckFunctions(ToInterprete, i, NumCharsToSkip);
		if (wasProcessed) {
			i += NumCharsToSkip;
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

}

#pragma region FunctionMap functions

void ATurtleInterpreter::PushTurtleState() {
	FTurtleState* newState = new FTurtleState();
	newState->StatePosition = CurrentPosition;
	newState->StateRotation = CurrentRotation;
	newState->StateBranch = &CurrentBranch;

	TurtleStateStack.Push(newState);
}

void ATurtleInterpreter::PopTurtleState() {

	if (TurtleStateStack.Num() > 0) {
		FTurtleState* toReturnTo = TurtleStateStack.Pop();

		CurrentPosition = toReturnTo->StatePosition;
		CurrentRotation = toReturnTo->StateRotation;

		if (NULL != (*toReturnTo->StateBranch)) {
			CurrentBranch = (*toReturnTo->StateBranch)->ParentBranch;
			if (NULL == CurrentBranch) {
				CurrentBranch = (*toReturnTo->StateBranch);
			}
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
	CurrentRotation = FQuat::MakeFromEuler(FVector::RightVector * 90);
}


void ATurtleInterpreter::ConstructBranch(float length) {
	FBranch* newBranch = new FBranch();
	
	CurrentBranch->ChildBranches.Add(newBranch);
	newBranch->ParentBranch = CurrentBranch;
	newBranch->Start = CurrentBranch->End;
	newBranch->End = newBranch->Start + length * CurrentRotation.RotateVector(GetActorForwardVector());
	newBranch->BranchDepth = TurtleStateStack.Num();

	CurrentPosition = newBranch->End;
	CurrentBranch = newBranch;
}


void ATurtleInterpreter::Turn(float angle) {
	CurrentRotation *= FQuat(FVector::UpVector, FMath::DegreesToRadians(angle));
}
void ATurtleInterpreter::TurnLeft(float angle) {
	Turn(angle);
}
void ATurtleInterpreter::TurnRight(float angle) {
	Turn(-angle);
}

void ATurtleInterpreter::Pitch(float angle) {
	CurrentRotation *= FQuat(FVector::RightVector, FMath::DegreesToRadians(angle));
}
void ATurtleInterpreter::PitchDown(float angle) {
	Pitch(angle);
}
void ATurtleInterpreter::PitchUp(float angle) {
	Pitch(-angle);
}

void ATurtleInterpreter::Roll(float angle) {
	CurrentRotation *= FQuat(FVector::ForwardVector, FMath::DegreesToRadians(angle));
}
void ATurtleInterpreter::RollLeft(float angle) {
	Roll(angle);
}
void ATurtleInterpreter::RollRight(float angle) {
	Roll(-angle);
}

float ATurtleInterpreter::Multiplicate(float first, float second) {
	return first * second;
}

#pragma endregion

bool ATurtleInterpreter::CheckFunctions(FString ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	// this looks so freaking stupid.
	bool wasProcessed = CheckZeroArgFunctions(ToInterprete, CurrentCharIndex, OutNumCharsToSkip);
	if (wasProcessed) {
		return wasProcessed;
	}
	wasProcessed = CheckOneArgFunctions(ToInterprete, CurrentCharIndex, OutNumCharsToSkip);
	return wasProcessed;
}

bool ATurtleInterpreter::CheckZeroArgFunctions(FString ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	bool wasProcessed = false;
	TCHAR currentChar = ToInterprete[CurrentCharIndex];
	ZeroArgFunctionPtrType* functionPtr = ZeroArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		(this->*(*functionPtr))();
		wasProcessed = true;
		OutNumCharsToSkip = 1;
	}
	return wasProcessed;
}

bool ATurtleInterpreter::CheckOneArgFunctions(FString ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	bool wasProcessed = false;
	TCHAR currentChar = ToInterprete[CurrentCharIndex];
	OneArgFunctionPtrType* functionPtr = OneArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		int bracketPositionIndex = CurrentCharIndex + 1;
		TArray<FString> attributes = ULSystemInterpreter::GetAttributesBetweenBrackets(ToInterprete, bracketPositionIndex);
		if (1 != attributes.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("Number of Parameters for Function \"%s\" is %d, was given %d. Index: %d"), &currentChar, 1, attributes.Num(), CurrentCharIndex);
		}
		else {
			CheckAllAttributesForOperators(attributes);
			wasProcessed = TryCallOneArgFunction(&currentChar, attributes, functionPtr, OutNumCharsToSkip);
		}
	}
	return wasProcessed;
}

void ATurtleInterpreter::CheckAllAttributesForOperators(TArray<FString>& OutAttributes) {
	for (int i = 0; i < OutAttributes.Num(); ++i) {
		FString attribute = OutAttributes[i];
		OutAttributes[i] = CheckForMathOperators(attribute);
	}
}

FString ATurtleInterpreter::CheckForMathOperators(FString Attribute) {
	TwoArgOperatorPtrType* CurrentOperatorPtr = NULL;
	TwoArgOperatorPtrType* LastOperatorPtr = NULL;

	int LastOperatorIndex = FindIndexOfNextOperator(Attribute, 0, &LastOperatorPtr);
	FString resultString = Attribute.Left(LastOperatorIndex);
	int CurrentOperatorIndex = FindIndexOfNextOperator(Attribute, LastOperatorIndex, &CurrentOperatorPtr);
	while (CurrentOperatorIndex != LastOperatorIndex) {
		if (resultString.IsNumeric()) {
			float firstParameter = FCString::Atof(*resultString);

			int secondParameterStart = LastOperatorIndex + 1;
			int secondParameterCount = CurrentOperatorIndex - secondParameterStart;
			FString secondParameterString = Attribute.Mid(secondParameterStart, secondParameterCount);
			
			if (secondParameterString.IsNumeric()) {
				float secondParameter = FCString::Atof(*secondParameterString);

				float result = (this->*(*LastOperatorPtr))(firstParameter, secondParameter);
				resultString = FString::SanitizeFloat(result);
			}
		}
		LastOperatorIndex = CurrentOperatorIndex;
		LastOperatorPtr = CurrentOperatorPtr;
		CurrentOperatorIndex = FindIndexOfNextOperator(Attribute, LastOperatorIndex, &CurrentOperatorPtr);
	}
	return resultString;
}

int ATurtleInterpreter::FindIndexOfNextOperator(FString Attribute, int LastOperatorIndex, TwoArgOperatorPtrType** OutOperatorPtrType) {
	int nextOperatorIndex = LastOperatorIndex;
	for (int i = LastOperatorIndex + 1; i < Attribute.Len(); ++i) {
		TCHAR currentChar = Attribute[i];
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

bool ATurtleInterpreter::TryCallOneArgFunction(TCHAR* CurrentChar, TArray<FString> Attributes, OneArgFunctionPtrType* FunctionPtr, int& OutNumCharsToSkip) {
	bool wasProcessed = false;

	if (Attributes[0].IsNumeric()) {
		float firstAttribute = FCString::Atof(*Attributes[0]);
		(this->*(*FunctionPtr))(firstAttribute);
		int numOfBrackets = 2;
		OutNumCharsToSkip += Attributes[0].Len() + numOfBrackets;
		wasProcessed = true;
	}
	else {
		// Using TCHAR* looks silly, but otherwise it won't be copied correctly.
		UE_LOG(LogTemp, Warning, TEXT("Expected Numerical Input for Function \"%s\", was given \"%s\""), CurrentChar, *Attributes[0]);
	}
	return wasProcessed;
}
