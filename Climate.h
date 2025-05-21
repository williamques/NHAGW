#pragma once
#include <unordered_map>
#include <string>

enum weatherState {
    Drought,
    Sunny,
    Cloudy,
    Rainy,
    HeavyRain,
    Stormy
};

struct WeatherEffects {
    int waterChange;      // Water level change per step
    double evaporationRate; // Rate of water evaporation
};

struct Climate {
    std::string type = "Temperate";
    std::unordered_map<weatherState, WeatherEffects> effects = {
        {weatherState::Drought, {-2, 0.8}},
        {weatherState::Sunny, {-1, 0.5}},
        {weatherState::Cloudy, {0, 0.2}},
        {weatherState::Rainy, {2, 0.1}},
        {weatherState::HeavyRain, {4, 0.05}},
        {weatherState::Stormy, {3, 0.3}}
    };
    
    std::unordered_map<weatherState, std::unordered_map<weatherState, double>> transitionMatrix = {
        {weatherState::Drought, {
            {weatherState::Drought, 0.6}, {weatherState::Sunny, 0.4}
        }},
        {weatherState::Sunny, {
            {weatherState::Sunny, 0.5}, {weatherState::Cloudy, 0.3}, {weatherState::Rainy, 0.2}
        }},
        {weatherState::Cloudy, {
            {weatherState::Sunny, 0.3}, {weatherState::Cloudy, 0.4}, {weatherState::Rainy, 0.3}
        }},
        {weatherState::Rainy, {
            {weatherState::Cloudy, 0.3}, {weatherState::Rainy, 0.4}, {weatherState::HeavyRain, 0.2}, {weatherState::Stormy, 0.1}
        }},
        {weatherState::HeavyRain, {
            {weatherState::Rainy, 0.4}, {weatherState::HeavyRain, 0.3}, {weatherState::Stormy, 0.3}
        }},
        {weatherState::Stormy, {
            {weatherState::Rainy, 0.5}, {weatherState::Stormy, 0.3}, {weatherState::Cloudy, 0.2}
        }}
    };
};