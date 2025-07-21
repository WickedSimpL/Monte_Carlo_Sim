#include <iostream>
#include <array>
#include <random>
#include <cmath>

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

    float relVelNorm2 = relVel[0] * relVel[0] + relVel[1] * relVel[1];  // Dot Product
    
    if (relVelNorm2 == 0) {
        return std::sqrt(relPos[0] * relPos[0] + relPos[1] * relPos[1]);
    }

    float t = -(relPos[0] * relVel[0] + relPos[1] * relVel[1]) / relVelNorm2;

    if (t < 0) {
        return std::sqrt(relPos[0] * relPos[0] + relPos[1] * relPos[1]);
    }

    std::array<float, 2> closestRel;
    closestRel[0] = relPos[0] + t * relPos[0];
    closestRel[1] = relPos[1] + t * relPos[1];

    return std::sqrt(closestRel[0] * closestRel[0] + closestRel[1] * closestRel[1]); 
}


float estimateCrossingRisk(float numTrials, int numOtherShips, int area, int maxSpeed, float thresholdDist) {
    int countCollisions = 0;
    std::array<float, 2> mainPos = {0.0, 0.0};
    std::array<float, 2> mainVel = {1.0, 0.0};

    for (int i = 0; i < numTrials; i++) {
        bool isCollision = false;

        for (int j = 0; j < numOtherShips; j++) {
            std::array<float, 2> otherPos = randomUniformPosition(area);
            float angle = randomUniform(0, 2 * M_PI);
            float speed = randomUniform(0, maxSpeed);
            
            std::array<float, 2> otherVel = {speed * std::cos(angle), speed * std::sin(angle)};
            // Function call to CPA here TODO
            float dist = cpaDistance(mainPos, mainVel, otherPos, otherVel);
            std::cout << "CPA = " << dist << '\n';
            if (dist < thresholdDist) {
                isCollision = true;
                break;
            }
        }
        if (isCollision) {
            countCollisions++;
        }
    }
    float rate = countCollisions / numTrials;
    return rate;
}


int main() {
    int num_trials = 50000;
    int num_other_ships = 15;
    int area_size = 25;
    int max_speed = 2;
    float threshold_dist = 0.5;
    float prob = (estimateCrossingRisk(num_trials, num_other_ships, area_size, max_speed, threshold_dist) * 100);
    std::cout << "\n Estimated probability of collision: " << prob << "%" << std::endl;
    
    return 0;
}