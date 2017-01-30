// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "LSystemInterpreter.h"



FString ULSystemInterpreter::GetContentBetweenBrackets(FString Current) {
	int numberOfOpenBrackets = 0;
	int startIndex = 0;
	int endIndex = -1;
	for (int i = 0; i < Current.Len(); ++i) {
		TCHAR currentChar = Current[i];
		if ('(' == currentChar) {
			if (0 == numberOfOpenBrackets) {
				startIndex = i+1;
			}
			numberOfOpenBrackets++;
		}
		else if (')' == currentChar) {
			numberOfOpenBrackets--;
			if (0 == numberOfOpenBrackets) {
				endIndex = i;
				break;
			}
		}
	}

	if (-1 == endIndex) {
		UE_LOG(LogTemp, Warning, TEXT("Error with bracket enclosing of string: %s"), *Current);
		return INTERPRETER_ERROR_NUMBER_OF_BRACKETS;
	}

	if (1 != startIndex) {
		UE_LOG(LogTemp, Warning, TEXT("First Char is not a bracket: %s"), *Current);
		return INTERPRETER_ERROR_FIRST_CHAR_IS_NOT_A_BRACKET;
	}

	int numOfChars = endIndex - startIndex;
	return Current.Mid(startIndex, numOfChars);
}