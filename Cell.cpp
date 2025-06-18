#include "Cell.h"
#include "Model.h"
#include "Agent.h"
#include <random>
#include <algorithm>

Cell::Cell() 
   : water(0), 
     nutrients(10), 
     x(-1), 
     y(-1), 
     model(nullptr), 
     maxSoilSaturation(100), // Initialize maxSoilSaturation with a default value
     soilSaturation(10),     // Initialize soilSaturation with a default value
     weather(weatherState::Sunny) // Initialize weather with a default state
{}
void Cell::initialize(Model* m, int row, int col) {
    model = m;
    x = row;
    y = col;
}

std::vector<Cell*> Cell::getOrthogonalNeighbors() {
    return getNeighborsWithinDistance(1);
}

std::vector<Cell*> Cell::getNeighborsWithinDistance(int distance) {
    std::vector<Cell*> neighbors;
    neighbors.reserve(2 * distance * distance + 2 * distance);

    for (int dx = -distance; dx <= distance; ++dx) {
        int maxDy = distance - std::abs(dx);
        for (int dy = -maxDy; dy <= maxDy; ++dy) {
            if (dx == 0 && dy == 0) continue; // Skip the center cell (self)
            int nx = x + dx;
            int ny = y + dy;

            if (model->isTorus()) {
                nx = (nx + model->getHeight()) % model->getHeight();
                ny = (ny + model->getWidth()) % model->getWidth();
            }
            else {
                if (nx < 0 || nx >= model->getHeight() || ny < 0 || ny >= model->getWidth()) {
                    continue;
                }
            }
            Cell* neighbor = model->getCell(nx, ny);
            if (neighbor) {
                neighbors.push_back(neighbor);
            }
        }
    }
    return neighbors;
}

Cell* Cell::getRandomNeighbor() {
    std::vector<Cell*> neighbors = getOrthogonalNeighbors();
    if (!neighbors.empty()) {
        std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
        return neighbors[dist(model->getRNG())];
    }
    return nullptr;
}

void Cell::setWeather(weatherState w)
{
    weather = w;
}

weatherState Cell::getWeather() const
{
    return weather;
}

void Cell::updateEnvironment() {
    // Get the current climate from the model (assuming it's stored there)
    const Climate& climate = model->getClimate();
    
    // Get current weather effects
    const WeatherEffects& effects = climate.effects.at(weather);
    
    // Update water level based on weather effects
    water = std::max(0, water + effects.waterChange);
    // Simulate evaporation
    int evaporation = static_cast<int>(water * effects.evaporationRate);
    water = std::max(0, water - evaporation);
    // Update soil saturation based on water
    if (water > 0) {
        if (soilSaturation < maxSoilSaturation) {
            soilSaturation = std::min(maxSoilSaturation, soilSaturation + 1);
            water--;
        }
    }
    
    // Determine next weather state based on transition probabilities
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double random = dist(model->getRNG());
    double cumulative = 0.0;
    
    const auto& transitions = climate.transitionMatrix.at(weather);
    for (const auto& [nextState, probability] : transitions) {
        cumulative += probability;
        if (random <= cumulative) {
            weather = nextState;
            break;
        }
    }
}

int Cell::getWater() const {
    return water; 
}
void Cell::modifyWater(int w) {
    water += w;
    if (water < 0) {
        water = 0;
    }
}
int Cell::getNutrients() const { 
    return nutrients; 
}

void Cell::modifyNutrients(int n) {
    nutrients += n;
    if (nutrients < 0) {
        nutrients = 0;
    }
}

void Cell::addAgent(long long int agentId) {
    agentIds.push_back(agentId);
}

void Cell::removeAgent(long long int agentId) {
    agentIds.erase(std::remove(agentIds.begin(), agentIds.end(), agentId), agentIds.end());
}

bool Cell::hasType(std::string type) const {
    for (auto agentId : agentIds) {
        Agent* agent = model->getAgent(agentId);
        if (agent && agent->getType() == type) {
            return true;
        }
    }
    return false;
}

void Cell::modifySoilSaturation(int s){
    soilSaturation += s;
    if (soilSaturation < 0) {
        soilSaturation = 0;
    }
    else if (soilSaturation > maxSoilSaturation) {
        soilSaturation = maxSoilSaturation;
    }
}
