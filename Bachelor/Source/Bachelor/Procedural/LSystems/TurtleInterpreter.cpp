// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "TurtleInterpreter.h"
#include "Bachelor/Utility/LSystemInterpreter.h"
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
	InterpreteLSystemResult();
}

void ATurtleInterpreter::InterpreteLSystemResult() {
	for (int i = 0; i < StringToInterpret.Len(); ++i) {
		TCHAR currentChar = StringToInterpret[i];
		int NumCharsToSkip = 0;
		bool wasProcessed = CheckFunctions(i, NumCharsToSkip);
		if (wasProcessed) {
			i += NumCharsToSkip;
		}
	}
}

void ATurtleInterpreter::ConstructFunctionMap() {
	// functions with no arguments
	ZeroArgumentFunctionMap.Add('$', &ATurtleInterpreter::SetVertical);

	// functions with one argument
	OneArgumentFunctionMap.Add('F', &ATurtleInterpreter::ConstructBranch);

	OneArgumentFunctionMap.Add('+', &ATurtleInterpreter::TurnLeft);
	OneArgumentFunctionMap.Add('-', &ATurtleInterpreter::TurnRight);

	OneArgumentFunctionMap.Add('&', &ATurtleInterpreter::PitchDown);
	OneArgumentFunctionMap.Add('^', &ATurtleInterpreter::PitchUp);

	OneArgumentFunctionMap.Add('/', &ATurtleInterpreter::RollLeft);
	OneArgumentFunctionMap.Add('\\', &ATurtleInterpreter::RollRight);

	// functions with two arguments

}

#pragma region FunctionMap functions

void ATurtleInterpreter::SetVertical() {
	CurrentRotation = FQuat::MakeFromEuler(FVector::RightVector * 90);
}


void ATurtleInterpreter::ConstructBranch(float length) {
	FBranch* newBranch = new FBranch();

	CurrentBranch->ChildBranches.Add(newBranch);
	newBranch->ParentBranch = CurrentBranch;
	newBranch->Start = CurrentBranch->End;
	newBranch->End = newBranch->Start + length * CurrentRotation.RotateVector(GetActorForwardVector());

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

#pragma endregion


bool ATurtleInterpreter::CheckFunctions(int CurrentCharIndex, int& OutNumCharsToSkip) {
	// this looks so freaking stupid.
	bool wasProcessed = CheckZeroArgFunctions(CurrentCharIndex, OutNumCharsToSkip);
	if (wasProcessed) {
		return wasProcessed;
	}
	wasProcessed = CheckOneArgFunctions(CurrentCharIndex, OutNumCharsToSkip);
	if (wasProcessed) {
		return wasProcessed;
	}
	wasProcessed = CheckTwoArgFunctions(CurrentCharIndex, OutNumCharsToSkip);
	return wasProcessed;
}

bool ATurtleInterpreter::CheckZeroArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip) {
	bool wasProcessed = false;
	TCHAR currentChar = StringToInterpret[CurrentCharIndex];
	ZeroArgFunctionPtrType* functionPtr = ZeroArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		(this->*(*functionPtr))();
		wasProcessed = true;
		OutNumCharsToSkip = 0;
	}
	return wasProcessed;
}

bool ATurtleInterpreter::CheckOneArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip) {
	bool wasProcessed = false;
	TCHAR currentChar = StringToInterpret[CurrentCharIndex];
	OneArgFunctionPtrType* functionPtr = OneArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		int BracketPositionIndex = CurrentCharIndex + 1;
		TArray<FString> Attribute = ULSystemInterpreter::GetAttributesBetweenBrackets(StringToInterpret, BracketPositionIndex);
		if (1 != Attribute.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("Number of Parameters for Function %s is %d, was given %d"), currentChar, 1, Attribute.Num());
		}
		else {
			float firstAttribute = FCString::Atof(*Attribute[0]);
			(this->*(*functionPtr))(firstAttribute);

			int numOfBrackets = 2;
			OutNumCharsToSkip += Attribute[0].Len() + numOfBrackets;
			wasProcessed = true;
		}
	}
	return wasProcessed;
}

bool ATurtleInterpreter::CheckTwoArgFunctions(int CurrentCharIndex, int& OutNumCharsToSkip) {
	bool wasProcessed = false;
	TCHAR currentChar = StringToInterpret[CurrentCharIndex];
	TwoArgFunctionPtrType* functionPtr = TwoArgumentFunctionMap.Find(currentChar);
	if (NULL != functionPtr) {
		int BracketPositionIndex = CurrentCharIndex + 1;
		TArray<FString> Attribute = ULSystemInterpreter::GetAttributesBetweenBrackets(StringToInterpret, BracketPositionIndex);
		if (2 != Attribute.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("Number of Parameters for Function %s is %d, was given %d"), currentChar, 2, Attribute.Num());
		}
		else {
			float firstAttribute = FCString::Atof(*Attribute[0]);
			float secondAttribute = FCString::Atof(*Attribute[1]);
			(this->*(*functionPtr))(firstAttribute, secondAttribute);

			int numOfBrackets = 2;
			int numOfCommas = 1;
			OutNumCharsToSkip += Attribute[0].Len() + Attribute[1].Len() + numOfBrackets + numOfCommas;
			wasProcessed = true;
		}
	}
	return wasProcessed;
}