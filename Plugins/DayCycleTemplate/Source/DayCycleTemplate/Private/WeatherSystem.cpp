// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherSystem.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"

// Sets default values
AWeatherSystem::AWeatherSystem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ticksToUpdate = 1.0f;
	Speed = 1.0f;
	sunAngle = 270.0f;
	maxAngle = 345.0f;

	morningStart = 260.0f; // 260.0f
	noonStart = 300.0f; // 300.0f
	nightStart = 330.0f; // 330.0f

	morningColor = FLinearColor(1.0f, 1.0f, 1.0f);
	noonColor = FLinearColor(1.0f, 0.7f, 0.45f);
	nightColor = FLinearColor(0.0f, 0.0f, 0.0f);
	morningIntensity = 10.0f;
	noonIntensity = 7.0f;
	nightIntensity = 0.0f;

	fogChance = 0.5f;
	HeavyFogChance = 0.33f;

	dayPhase = 0;
	nightMul = 0.5f;
	alpha = 1.0f;
	tickCount = 0.0f;

	DirectionalLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));
	DirectionalLight->SetupAttachment(Root);
	DirectionalLight->SetRelativeRotation(FRotator(sunAngle, 0.0f, 0.0f));
	DirectionalLight->SetIntensity(7.0f);
	DirectionalLight->SetLightColor(FLinearColor(1.0f, 1.0f, 1.0f));

	ExponentialHeightFog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	ExponentialHeightFog->SetupAttachment(Root);
	ExponentialHeightFog->SetVolumetricFog(true);
	//ExponentialHeightFog->SetFogDensity(0.02f);
	//ExponentialHeightFog->SetFogHeightFalloff(0.2f);
	//ExponentialHeightFog->SetVolumetricFogScatteringDistribution(0.3f);
	//ExponentialHeightFog->SetVolumetricFogExtinctionScale(2.5f);
	// Roll for fog initial state
	rollFog();
}

// Called when the game starts or when spawned
void AWeatherSystem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeatherSystem::OnMorningStartEvent()
{
	OnMorningStart.Broadcast();
}

void AWeatherSystem::OnNoonStartEvent()
{
	OnNoonStart.Broadcast();
}

void AWeatherSystem::OnNightStartEvent()
{
	OnNightStart.Broadcast();
}

void AWeatherSystem::lerpColorIntensity(FLinearColor color1, FLinearColor color2, float intensity1, float intensity2, float t)
{
	FLinearColor newcolor = FMath::Lerp(color1, color2, t);
	DirectionalLight->SetLightColor(newcolor);
	float newIntensity = FMath::Lerp(intensity1, intensity2, t);
	DirectionalLight->SetIntensity(newIntensity);
}

void AWeatherSystem::updateSun(float DeltaTime)
{
	sunAngle += Speed * DeltaTime * alpha * ticksToUpdate;
	if (sunAngle >= maxAngle)
	{
		sunAngle = 195.0f;
		alpha = 1.0f;
		// Set the foggy day based on the fog chance
		rollFog();
	}
	if (sunAngle <= morningStart)
	{
		// Lerp the light color
		float t = (sunAngle - 195.0f) / (morningStart - 195.0f);
		lerpColorIntensity(nightColor, morningColor, nightIntensity, morningIntensity, t);
		if (dayPhase != 0)
		{
			dayPhase = 0;
			OnMorningStartEvent();
		}
	}
	if (sunAngle >= morningStart && sunAngle <= noonStart)
	{
		// Lerp the light color
		float t = (sunAngle - morningStart) / (noonStart - morningStart);
		lerpColorIntensity(morningColor, noonColor, morningIntensity, noonIntensity, t);
		if (dayPhase != 1)
		{
			dayPhase = 1;
			OnNoonStartEvent();
		}
	}
	if (sunAngle >= noonStart && sunAngle <= nightStart)
	{
		// Lerp the light color
		float t = (sunAngle - noonStart) / (nightStart - noonStart);
		lerpColorIntensity(noonColor, nightColor, noonIntensity, nightIntensity, t);
		if (dayPhase != 2)
		{
			dayPhase = 2;
			OnNightStartEvent();
		}
	}
	if (sunAngle >= nightStart && dayPhase != 3)
	{
		dayPhase = 3;
		alpha = nightMul;
	}
	DirectionalLight->SetWorldRotation(FRotator(sunAngle, 0.0f, 0.0f));
}

void AWeatherSystem::rollFog()
{
	// Roll for fog initial state
	float fogRoll = FMath::FRandRange(0.0f, 1.0f);
	if (fogRoll <= fogChance)
	{
		// Reroll for heavy fog
		float heavyFogRoll = FMath::FRandRange(0.0f, 1.0f);
		if (heavyFogRoll <= HeavyFogChance)
		{
			// Set the heavy fog
			setHeavyFog();
		}
		else
		{
			// Set the normal fog
			setPrettyFog();
		}
	}
	else
	{
		// Set the clear sky
		setMildFog();
	}
}

// FogPretty: FogDensity 0.04, FogHeightFalloff 1.6, ScatteringDistribution 0.3, ExtinctionScale 2.5, VolmetricFog True
void AWeatherSystem::setPrettyFog()
{
	ExponentialHeightFog->SetFogDensity(0.04f);
	ExponentialHeightFog->SetFogHeightFalloff(1.6f);
	ExponentialHeightFog->SetVolumetricFogExtinctionScale(2.5f);
	ExponentialHeightFog->SetStartDistance(0.0f);
}

// FogMild: FogDensity 0.02, FogHeightFalloff 0.2, ScatteringDistribution 0.3, ExtinctionScale 2.5, VolmetricFog True
void AWeatherSystem::setMildFog()
{
	ExponentialHeightFog->SetFogDensity(0.02f);
	ExponentialHeightFog->SetFogHeightFalloff(0.2f);
	ExponentialHeightFog->SetVolumetricFogExtinctionScale(2.5f);
	ExponentialHeightFog->SetStartDistance(0.0f);
}

// FogHeavy: FogDensity 0.2, FogHeightFalloff 0.2, ScatteringDistribution 0.3, ExtinctionScale 10.0, StartDistance 800 VolmetricFog True
void AWeatherSystem::setHeavyFog()
{
	ExponentialHeightFog->SetFogDensity(0.2f);
	ExponentialHeightFog->SetFogHeightFalloff(0.2f);
	ExponentialHeightFog->SetVolumetricFogExtinctionScale(10.0f);
	ExponentialHeightFog->SetStartDistance(1000.0f);
}

// Called every frame
void AWeatherSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tickCount++;
	if (tickCount >= ticksToUpdate)
	{
		tickCount = 0;
		updateSun(DeltaTime);
	}
}

