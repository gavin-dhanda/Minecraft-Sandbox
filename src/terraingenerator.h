#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H
#include "src/cube.h"
#include <vector>
#include <glm/glm.hpp>
#include <map>
#include "cube.h"
#include "FastNoiseLite.h"

class TerrainGenerator
{
public:
    TerrainGenerator();
    std::map<std::tuple<int, int, int>, Cube*> createTranslationMatricesForChunk(int chunkX, int chunkY);
    static float getOffset();
    static const int chunkSize = 8;
    static const int maxChunkHeight = 25;
    static const int chunkDepth  = 25;
    static const int offset = 1;
    float treeProbability = 0.15; // probability of generating a tree above grass (1% -> happens once in every 100 blocks generated)
    float treeHeight = -24; // generates trees above z_values > -15

    int previousHeightOFfset = 0;

    float getFractalNoise(FastNoiseLite noise, float x, float y, int octaves, float persistence);

    glm::vec3 playerPosition;
    int renderDistance = 2; // Number of chunks to render in each direction from the player
    std::vector<Cube*> updatePlayerPosition(const glm::vec3& newPosition, std::vector<Cube*> oldCubes, bool isFirst);
    bool checkAndLoadChunks();
    const std::map<std::pair<int, int>, std::map<std::tuple<int, int, int>, Cube*>>& getChunkMatrices() const;

    std::map<std::pair<int, int>, std::map<std::tuple<int, int, int>, Cube*>> chunkMatrices1;
    std::map<std::pair<int, int>, std::map<std::tuple<int, int, int>, Cube*>> cachedChunkMatrices2;

    std::vector<Cube*> cubesVector;

    int getRandomInt();
    int counter;


    // Takes in the camera position and gets the height of the terrain at that point.
    int getGroundHeight(glm::vec3 position);
};

#endif // TERRAINGENERATOR_H
