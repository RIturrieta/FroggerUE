// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeatherSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMorningStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoonStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightStartDelegate);

UCLASS()
class DAYCYCLETEMPLATE_API AWeatherSystem : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = DLight)
	class UDirectionalLightComponent* DirectionalLight;

	UPROPERTY(BlueprintReadOnly, Category = EHFog)
	class UExponentialHeightFogComponent* ExponentialHeightFog;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float ticksToUpdate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float sunAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float morningStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float noonStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float nightStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float fogChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float HeavyFogChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeatherSystem)
	float nightMul;

private:
	class USceneComponent* Root;
	float time;
	float maxAngle;
	struct FLinearColor morningColor;
	struct FLinearColor noonColor;
	struct FLinearColor nightColor;
	float morningIntensity;
	float noonIntensity;
	float nightIntensity;
	int dayPhase;
	float alpha;
	float tickCount;
	
public:	
	// Sets default values for this actor's properties
	AWeatherSystem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnMorningStartEvent();
	UFUNCTION()
	void OnNoonStartEvent();
	UFUNCTION()
	void OnNightStartEvent();
	void lerpColorIntensity(FLinearColor color1, FLinearColor color2, float intensity1, float intensity2, float t);
	void updateSun(float DeltaTime);
	void rollFog();
	void setPrettyFog();
	void setMildFog();
	void setHeavyFog();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintAssignable, Category = "Day Phase")
	FOnMorningStartDelegate OnMorningStart;
	UPROPERTY(BlueprintAssignable, Category = "Day Phase")
	FOnNoonStartDelegate OnNoonStart;
	UPROPERTY(BlueprintAssignable, Category = "Day Phase")
	FOnNightStartDelegate OnNightStart;

};
