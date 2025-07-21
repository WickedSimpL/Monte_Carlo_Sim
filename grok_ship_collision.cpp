#include <iostream>
#include <array>
#include <random>
#include <cmath>
#include <chrono>    // Add this include
#include <iomanip>

std::array<float, 2> randomUniformPosition(float areaSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-areaSize, areaSize);

    std::array<float, 2> result;
    for (float& x : result) {
        x = dist(gen);
    }
    return result;
}

float randomUniform(float first, float second) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(first, second);

    return dist(gen);
}

float cpaDistance(std::array<float, 2> pos1, std::array<float, 2> vel1, std::array<float, 2> pos2, std::array<float, 2> vel2) {
    std::array<float, 2> relPos;
    relPos[0] = pos2[0] - pos1[0];
    relPos[1] = pos2[1] - pos1[1];

    std::array<float, 2> relVel;
    relVel[0] = vel2[0] - vel1[0];
    relVel[1] = vel2[1] - vel1[1];

    float relVelNorm2 = relVel[0] * relVel[0] + relVel[1] * relVel[1];

    if (relVelNorm2 == 0) {
        return std::sqrt(relPos[0] * relPos[0] + relPos[1] * relPos[1]);
    }

    float dot = relPos[0] * relVel[0] + relPos[1] * relVel[1];
    float t = -dot / relVelNorm2;

    if (t < 0) {
        return std::sqrt(relPos[0] * relPos[0] + relPos[1] * relPos[1]);
    }

    std::array<float, 2> closestRel;
    closestRel[0] = relPos[0] + t * relVel[0];
    closestRel[1] = relPos[1] + t * relVel[1];

    return std::sqrt(closestRel[0] * closestRel[0] + closestRel[1] * closestRel[1]); 
}

float estimateCrossingRisk(float numTrials, int numOtherShips, float area, float maxSpeed, float thresholdDist) {
    int countCollisions = 0;
    std::array<float, 2> mainPos = {0.0f, 0.0f};
    std::array<float, 2> mainVel = {1.0f, 0.0f};

    for (int i = 0; i < numTrials; ++i) {
        bool isCollision = false;

        for (int j = 0; j < numOtherShips; ++j) {
            auto otherPos = randomUniformPosition(area);
            float angle = randomUniform(0.0f, 2.0f * std::acos(-1.0f));
            float speed = randomUniform(0.0f, maxSpeed);
            
            std::array<float, 2> otherVel = {speed * std::cos(angle), speed * std::sin(angle)};
            float dist = cpaDistance(mainPos, mainVel, otherPos, otherVel);
            // Removed print for clean output
            if (dist < thresholdDist) {
                isCollision = true;
                break;
            }
        }
        if (isCollision) {
            countCollisions++;
        }
    }
    return static_cast<float>(countCollisions) / numTrials;
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    int num_trials = 50000;
    int num_other_ships = 15;
    float area_size = 25.0f;
    float max_speed = 2.0f;
    float threshold_dist = 0.2f;  // Aligned to match Python
    float rate = estimateCrossingRisk(num_trials, num_other_ships, area_size, max_speed, threshold_dist);
    float prob = rate * 100.0f;
    std::cout << "\nEstimated probability of collision: " << std::fixed << std::setprecision(2) << prob << "%" << std::endl;
    

    auto end = std::chrono::high_resolution_clock::now();  // End timer
    std::chrono::duration<double> dur = end - start;
    std::cout << "Function 'Main' executed in: " << std::fixed << std::setprecision(4) << dur.count() << " seconds\n";
    return 0;
}