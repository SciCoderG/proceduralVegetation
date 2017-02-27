// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "OperatorInterpreter.h"
#include "Bachelor/Utility/LSystemInterpreter.h"

// Sets default values
AOperatorInterpreter::AOperatorInterpreter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ConstructFunctionMap();

}

AOperatorInterpreter::~AOperatorInterpreter() {

}

// Called when the game starts or when spawned
void AOperatorInterpreter::BeginPlay()
{

}

// Called every frame
void AOperatorInterpreter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AOperatorInterpreter::ConstructFunctionMap() {

	// functions with two arguments
	TwoArgumentMathOperatorMap.Add('*', &AOperatorInterpreter::Multiplicate);

}

#pragma region FunctionMap functions

float AOperatorInterpreter::Multiplicate(float first, float second) {
	return first * second;
}
bool AOperatorInterpreter::Lower(float first, float second) {
	return first < second;
}
bool AOperatorInterpreter::LowerEqual(float first, float second) {
	return first <= second;
}
bool AOperatorInterpreter::Greater(float first, float second) {
	return first > second;
}
bool AOperatorInterpreter::GreaterEqual(float first, float second) {
	return first >= second;
}
bool AOperatorInterpreter::Equal(float first, float second) {
	return first == second;
}

#pragma endregion

bool AOperatorInterpreter::CheckFunctions(FString* ToInterprete, int CurrentCharIndex, int& OutNumCharsToSkip) {
	return false;
}

void AOperatorInterpreter::CheckAllAttributesForOperators(TArray<FString>& OutAttributes) {
	for (int i = 0; i < OutAttributes.Num(); ++i) {
		FString attribute = OutAttributes[i];
		OutAttributes[i] = CheckForMathOperators(&attribute);
	}
}

// the next two functions are actually really dumb.

FString AOperatorInterpreter::CheckForMathOperators(FString* Attribute) {
	{
		TwoArgMathOperatorPtrType* currentOperatorPtr = NULL;
		TwoArgMathOperatorPtrType* lastOperatorPtr = NULL;

		int lastOperatorIndex = FindIndexOfNextMathOperator(Attribute, 0, &lastOperatorPtr);
		FString resultString = (*Attribute).Left(lastOperatorIndex);
		int currentOperatorIndex = FindIndexOfNextMathOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
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
			currentOperatorIndex = FindIndexOfNextMathOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
		}
		return resultString;
	}
}

FString AOperatorInterpreter::CheckForLogicOperators(FString* Attribute) {
	TwoArgLogicOperatorPtrType* currentOperatorPtr = NULL;
	TwoArgLogicOperatorPtrType* lastOperatorPtr = NULL;

	int lastOperatorIndex = FindIndexOfNextLogicOperator(Attribute, 0, &lastOperatorPtr);
	FString resultString = (*Attribute).Left(lastOperatorIndex);
	int currentOperatorIndex = FindIndexOfNextLogicOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
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
		currentOperatorIndex = FindIndexOfNextLogicOperator(Attribute, lastOperatorIndex, &currentOperatorPtr);
	}
	return resultString;
}


int AOperatorInterpreter::FindIndexOfNextMathOperator(FString* Attribute, int LastOperatorIndex, TwoArgMathOperatorPtrType** OutOperatorPtrType) {
	int nextOperatorIndex = LastOperatorIndex;
	for (int i = LastOperatorIndex + 1; i < (*Attribute).Len(); ++i) {
		TCHAR currentChar = (*Attribute)[i];
		nextOperatorIndex = i;
		(*OutOperatorPtrType) = TwoArgumentMathOperatorMap.Find(currentChar);
		if (NULL != (*OutOperatorPtrType)) {
			return nextOperatorIndex;
		}
	}
	if (nextOperatorIndex != LastOperatorIndex) {
		nextOperatorIndex++;
	}
	return nextOperatorIndex;
}

int AOperatorInterpreter::FindIndexOfNextLogicOperator(FString* Attribute, int LastOperatorIndex, TwoArgLogicOperatorPtrType** OutOperatorPtrType) {
	int nextOperatorIndex = LastOperatorIndex;
	for (int i = LastOperatorIndex + 1; i < (*Attribute).Len(); ++i) {
		TCHAR currentChar = (*Attribute)[i];
		nextOperatorIndex = i;
		(*OutOperatorPtrType) = TwoArgumentLogicOperatorMap.Find(currentChar);
		if (NULL != (*OutOperatorPtrType)) {
			return nextOperatorIndex;
		}
	}
	if (nextOperatorIndex != LastOperatorIndex) {
		nextOperatorIndex++;
	}
	return nextOperatorIndex;
}

