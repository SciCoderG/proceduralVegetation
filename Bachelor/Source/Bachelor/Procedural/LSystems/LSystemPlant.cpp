// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "LSystemPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "Bachelor/Procedural/Data/MeshData.h"
#include "Bachelor/Procedural/Branch.h"
#include "Bachelor/Utility/MeshDataConstructor.h"
#include "Bachelor/Utility/MeshConstructor.h"
#include "Bachelor/Utility/BranchUtility.h"
#include "Bachelor/Utility/LSystemInterpreter.h"
#include "TurtleInterpreter.h"

#define PRODUCTION_WAS_UNSUCCESSFUL "ProductionUnsuccessfullError"



// Sets default values
ALSystemPlant::ALSystemPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* Root = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	Root->InitSphereRadius(50.0f);
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	Mesh->SetupAttachment(RootComponent);

	RootBranch = new FBranch();
	

	AllMeshData = new FMeshData();

	LSystemData.BendingByTropismParameter = 0.2f;

	InitUtilityValues();
}

ALSystemPlant::~ALSystemPlant() {
	delete AllMeshData; 
}

DECLARE_CYCLE_STAT(TEXT("LSystem ~ BeginPlay"), STAT_BeginPlay, STATGROUP_LSystem);
DECLARE_CYCLE_STAT(TEXT("LSystem ~ GenerateTreeMesh"), STAT_GenerateTreeMesh, STATGROUP_LSystem);
void ALSystemPlant::BeginPlay()
{
	SCOPE_CYCLE_COUNTER(STAT_BeginPlay);

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());

	ConstructDataMaps();

	CompleteDerivation();
	UE_LOG(LogTemp, Warning, TEXT("Resulting derivation has a length of: %d"), CurrentDerivation.Len());
	UClass* turtleInterpreterClass = ATurtleInterpreter::StaticClass();
	turtleInterpreter = GetWorld()->SpawnActor<ATurtleInterpreter>(turtleInterpreterClass, GetActorLocation(), GetActorRotation(), FActorSpawnParameters());
	turtleInterpreter->StartInterpretation(&RootBranch, LSystemData, &CurrentDerivation);

	InitUtilityValues();
	{
		SCOPE_CYCLE_COUNTER(STAT_GenerateTreeMesh);
		UMeshConstructor::GenerateTreeMesh(&TreeConstructionData);
	}
	
	turtleInterpreter->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("-----------------"));
	if (NULL != RootBranch) {
		UBranchUtility::RecursiveDeleteAllBranches(RootBranch);
	}
}

// Called every frame
void ALSystemPlant::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

#if WITH_EDITOR  
void ALSystemPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	/*if ((PropertyName == GET_MEMBER_NAME_CHECKED(ALSystemPlant, MaxNumGrowthIterations))) {
		// Potential place to regenerate tree
	}*/

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ALSystemPlant::InitUtilityValues() {
	TreeConstructionData.Mesh = Mesh;
	TreeConstructionData.AllMeshData = AllMeshData;
	TreeConstructionData.RootBranch = RootBranch;
}

void ALSystemPlant::ConstructDataMaps() {
	ReplaceConstantIDsByValues();
	ConstructProductionMap();
}

void ALSystemPlant::ReplaceConstantIDsByValues() {
	for (FLSConstantData constantData : LSystemData.Constants) {
		FString constantToString = FString::SanitizeFloat(constantData.ConstantValue);
		LSystemData.Axiom = LSystemData.Axiom.Replace(*constantData.ID, *constantToString, ESearchCase::CaseSensitive);
		for (int i = 0; i < LSystemData.Productions.Num(); ++i) {
			FProductionData* production = &LSystemData.Productions[i];
			production->ProductionResult = production->ProductionResult.Replace(*constantData.ID, *constantToString, ESearchCase::CaseSensitive);
//			production->Condition = production->Condition.Replace(*constantData.ID, *constantToString, ESearchCase::CaseSensitive);
		}
	}
}

void ALSystemPlant::ConstructProductionMap() {
	RemoveAnyWhiteSpaces();
	for (FProductionData productionData : LSystemData.Productions) {
		if (!productionData.ID.IsEmpty()) {
			ProductionMap.Add(productionData.ID[0], productionData);
		}
	}
}

void ALSystemPlant::RemoveAnyWhiteSpaces() {
	LSystemData.Axiom = LSystemData.Axiom.Replace(TEXT(" "), TEXT(""));

	for (int i = 0; i < LSystemData.Productions.Num(); ++i) {
		FProductionData* production = &LSystemData.Productions[i];
		production->ProductionResult = production->ProductionResult.Replace(TEXT(" "), TEXT(""));
	}
}

DECLARE_CYCLE_STAT(TEXT("LSystem ~ CompleteDerivation"), STAT_CompleteDerivation, STATGROUP_LSystem);
void ALSystemPlant::CompleteDerivation() {
	SCOPE_CYCLE_COUNTER(STAT_CompleteDerivation);
	CurrentDerivation = LSystemData.Axiom;
	for (int i = 0; i < LSystemData.NumberOfDerivations; ++i) {
		CurrentDerivation = Derivate(CurrentDerivation);
		if (PRODUCTION_WAS_UNSUCCESSFUL == CurrentDerivation) {
			UE_LOG(LogTemp, Warning, TEXT("Critical Error during Derivation - interrupting."));
			break;
		}
	}
}

DECLARE_CYCLE_STAT(TEXT("LSystem ~ Derivate"), STAT_Derivate, STATGROUP_LSystem);
FString ALSystemPlant::Derivate(FString ToDerivate) {
	SCOPE_CYCLE_COUNTER(STAT_Derivate);
	FString derivationResult;
	for (int i = 0; i < ToDerivate.Len(); ++i) {
		TCHAR currentChar = ToDerivate[i];
		FProductionData* potentialProduction = ProductionMap.Find(currentChar);
		if (NULL == potentialProduction) {
			derivationResult.AppendChar(currentChar);
		}
		else {
			int numberOfCharsToIgnore = 0;
			FString productionResult = CheckProduction(potentialProduction, i, numberOfCharsToIgnore);
			if (PRODUCTION_WAS_UNSUCCESSFUL == productionResult) {
				UE_LOG(LogTemp, Warning, TEXT("Error during Derivation - Error during application of Production."));
				return PRODUCTION_WAS_UNSUCCESSFUL;
			}
			i += numberOfCharsToIgnore;
			derivationResult += productionResult;
		}
	}
	return derivationResult;
}

DECLARE_CYCLE_STAT(TEXT("LSystem ~ CheckProduction"), STAT_CheckProduction, STATGROUP_LSystem);
FString ALSystemPlant::CheckProduction(FProductionData* Production, int KeyIndex, int& OutNumberOfCharsToIgnore){
	SCOPE_CYCLE_COUNTER(STAT_CheckProduction);
	int numParameters = Production->ParameterList.Num();

	FString productionResult = Production->ProductionResult;
	if (0 == numParameters) {
		return productionResult;
	}
	int BracketPositionIndex = KeyIndex + 1;
	FString contentBetweenBrackets = ULSystemInterpreter::GetContentBetweenBrackets(&CurrentDerivation, BracketPositionIndex);

	if (INTERPRETER_ERROR_NUMBER_OF_BRACKETS == contentBetweenBrackets) {
		return PRODUCTION_WAS_UNSUCCESSFUL;
	}
	if (INTERPRETER_ERROR_FIRST_CHAR_IS_NOT_A_BRACKET == contentBetweenBrackets) {
		UE_LOG(LogTemp, Warning, TEXT("Expected Bracket after production %s - this production expects parameters."), *Production->ID);
		return PRODUCTION_WAS_UNSUCCESSFUL;
	}

	OutNumberOfCharsToIgnore += contentBetweenBrackets.Len() + 2; // content of parameterlist + brackets

	TMap<FString, FString> parameterValues;
	bool fillingWasSuccessful = FillParameterValues(parameterValues, Production, contentBetweenBrackets);
	if (!fillingWasSuccessful) {
		return PRODUCTION_WAS_UNSUCCESSFUL;
	}

	for (int i = 0; i < numParameters; ++i) {
		FString parameter = Production->ParameterList[i];

		FString parameterValueString = *parameterValues.Find(parameter);
		parameterValueString = Derivate(parameterValueString); // Derivate parameter inputs!

		const TCHAR* parameterValue = *parameterValueString;
		const TCHAR* constParameter = *parameter;

		productionResult = productionResult.Replace(constParameter, parameterValue, ESearchCase::CaseSensitive);
	}

	return productionResult;
}

DECLARE_CYCLE_STAT(TEXT("LSystem ~ FillParameterValues"), STAT_FillParameterValues, STATGROUP_LSystem);
bool ALSystemPlant::FillParameterValues(TMap<FString, FString> &OutParameterValues, FProductionData* Production, FString ContentBetweenBrackets) {
	SCOPE_CYCLE_COUNTER(STAT_FillParameterValues);

	int numParameters = Production->ParameterList.Num();

	if (numParameters > 1) {
		TArray<FString> commaDividedContent;
		int numElementsInCommaDividedContent = ContentBetweenBrackets.ParseIntoArray(commaDividedContent, TEXT(","), false);

		if (numParameters != numElementsInCommaDividedContent) {
			UE_LOG(LogTemp, Warning, TEXT("Number of Parameters for Production %s is %d, was given %d"), *Production->ID, numParameters, numElementsInCommaDividedContent);
			return false;
		}

		for (int i = 0; i < numParameters; ++i) {
			FString parameter = Production->ParameterList[i];
			FString parameterValue = commaDividedContent[i];
			OutParameterValues.Add(parameter, parameterValue);
		}
	}
	else {
		FString parameter = Production->ParameterList[0];
		FString parameterValue = ContentBetweenBrackets;
		OutParameterValues.Add(parameter, parameterValue);
	}
	return true;
}
