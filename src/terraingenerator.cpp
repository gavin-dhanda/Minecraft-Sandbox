#include "terraingenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FastNoiseLite.h"  // Include FastNoiseLite
#include "src/cube.h"
#include <random>
#include <iostream>

TerrainGenerator::TerrainGenerator(){
}

// Add this method to TerrainGenerator class
float TerrainGenerator::getFractalNoise(FastNoiseLite noise, float x, float y, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;  // Used for normalizing result

    for(int i = 0; i < octaves; i++) {
        total += noise.GetNoise(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}


void generateTree(int baseX, int baseY, int baseZ, std::map<std::tuple<int,int,int>, Cube*> &matricesCubes,
                  int originalX, int originalY, int originalZ) {
    int treeHeight = rand() % 5 + 4; // Random tree height between 4 and 8
    int leafRadius = 2; // Radius of the leaves around the top

    // Make trunk
    for (int z = (baseZ); z < baseZ + treeHeight; ++z) {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(baseX, baseY, z));
        Cube* newCube = new Cube(translation);
        newCube->setID(2);
        matricesCubes[{originalX, originalY, z - baseZ + originalZ}] = newCube;
    }

    // Generate leaves
    for (int x = -leafRadius; x <= leafRadius; ++x) {
        for (int y = -leafRadius; y <= leafRadius; ++y) {
            for (int z = treeHeight - leafRadius; z <= treeHeight; ++z) {
                if (x * x + y * y + (z - treeHeight + leafRadius) * (z - treeHeight + leafRadius) <= leafRadius * leafRadius) {
                    glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(baseX + x, baseY + y, baseZ + z));
                    Cube* newCube = new Cube(translation);
                    newCube->setID(3);
                    matricesCubes[{originalX + x, originalY + y, baseZ + z}] = newCube;
                }
            }
        }
    }
}


// Function takes in the current chunk location (using ints) and using these as offsets.
std::map<std::tuple<int, int, int>, Cube*> TerrainGenerator::createTranslationMatricesForChunk(int chunkX, int chunkY) {
    std::map<std::tuple<int, int, int>, Cube*> matricesCubes;

    // Use FastNoiseLite library.
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    noise.SetFrequency(0.02f); // Can change this to get more mountainous terrain.

    // Want to center the chunk around the players current location.
    float centerXOffset = chunkSize / 2.0f;
    float centerYOffset = chunkSize / 2.0f;
    float centerZOffset = maxChunkHeight / 2.0f;

    // Based on what chunk we are in we need to offset from the origin using the values of chunkX and chunkY.
    float worldXOffset = chunkX * chunkSize;
    float worldYOffset = chunkY * chunkSize;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    int terrainOffset = getRandomInt();

    // Iterate through the block in the chunk.
    for (int x = 0; x < chunkSize; x++) {
        for (int y = 0; y < chunkSize; y++) {



            // Use noise to get the height of terrain.
            float heightValue = noise.GetNoise((float)x + chunkX * chunkSize, (float)y + chunkY * chunkSize);
            int terrainHeight = static_cast<int>((heightValue + 1) * 0.5 * maxChunkHeight)+previousHeightOFfset;

            // Iterate through the depth of the terrain and create blocks up until the terrain height.
            for (int z = 0; z < chunkDepth; z++) {

                // use the calculated offsets from earlier to form the translation matrix of the given cube.
                // z is offset by negative maxChunkHeight so that the blocks form beneath us.
                glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(
                                                                            x - centerXOffset + worldXOffset,
                                                                            y - centerYOffset + worldYOffset,
                                                                            -maxChunkHeight + z - centerZOffset));

                // Basic terrain.
                if (z < terrainHeight) {
                    matricesCubes[{x,y,z}] = new Cube(translation);
                }

                // Add a layer of water.
                if (z <= chunkDepth - 14) {
                    if (z > terrainHeight-1) {
                        Cube* cube = new Cube(translation);
                        cube->setID(5);
                        matricesCubes[{x,y,z}] = cube;
                    }
                }

                if (z == terrainHeight){
                    // Generate random probability of creating a tree and calculate the x,y, and z values for the tree.
                    float chance = distribution(generator);
                    float currHeight = -maxChunkHeight + z - centerZOffset-1;
                    float currX =  x - centerXOffset + worldXOffset;
                    float currY = y - centerYOffset + worldYOffset;

                    // If height and probability match characteristics generate tree.
                    if (chance < treeProbability && currHeight > treeHeight){
                        generateTree(currX, currY, currHeight, matricesCubes, x, y, z); // Generate the tree
                    }
                    matricesCubes[{x,y,z}] = new Cube(translation);
                }
            }
        }
    }
//    if(counter >= 7 || previousHeightOFfset >= 7){
//        previousHeightOFfset-=getRandomInt();
//    }
//    else{
//        previousHeightOFfset+=getRandomInt();
//    }
//    if(counter >=10){
//        counter = 0;
//    }
//    else{
//        counter+=1;
//    }

    return matricesCubes;
}

// based on the current camera position and render distance loads and unloads chunks
bool TerrainGenerator::checkAndLoadChunks() {
    // Adjust position for the fact that the original chunk is centered on the player.
    float adjustedPositionX = playerPosition.x + (playerPosition.x / abs(playerPosition.x)) * (float) chunkSize / 2.f;
    float adjustedPositionY = playerPosition.y + (playerPosition.y / abs(playerPosition.y)) * (float) chunkSize / 2.f;

    // Index of the current chunk that the player is in.
    int currentChunkX = static_cast<int>(adjustedPositionX / (chunkSize * offset));
    int currentChunkY = static_cast<int>(adjustedPositionY / (chunkSize * offset));

    // Create a list to keep track of chunks to unload
    std::vector<std::pair<int, int>> chunksToUnload;

    // Iterate through all loaded chunks
    for (auto& chunk : chunkMatrices1) {
        int x = chunk.first.first;
        int y = chunk.first.second;

        // Check if the chunk is outside the render distance
        if (x < currentChunkX - renderDistance || x > currentChunkX + renderDistance ||
            y < currentChunkY - renderDistance || y > currentChunkY + renderDistance) {
            // Mark this chunk for removal
            chunksToUnload.push_back(chunk.first);
        }
    }

    for (auto& chunk : chunksToUnload) {
        cachedChunkMatrices2[chunk] = chunkMatrices1[chunk];
        chunkMatrices1.erase(chunk);
    }

    if(counter % 20 == 0){
    int randomInt = getRandomInt();
    if(randomInt != 0 && randomInt != 1 && randomInt != -1){
        previousHeightOFfset++;
    }
    else{
        if(previousHeightOFfset <= 1){
            previousHeightOFfset = randomInt;
        }
        else{
            previousHeightOFfset--;
        }

    }

    }
    counter++;
    for (int x = currentChunkX - renderDistance; x <= currentChunkX + renderDistance; ++x) {
        for (int y = currentChunkY - renderDistance; y <= currentChunkY + renderDistance; ++y) {
            std::pair<int, int> chunkKey = {x, y};
            if (chunkMatrices1.find(chunkKey) == chunkMatrices1.end()) {
                // Check cache first
                if (cachedChunkMatrices2.find(chunkKey) != cachedChunkMatrices2.end()) {
                    // Load from cache
                    chunkMatrices1[chunkKey] = cachedChunkMatrices2[chunkKey];
                    cachedChunkMatrices2.erase(chunkKey);
                } else {
                    // Generate new chunk
                    chunkMatrices1[chunkKey] = createTranslationMatricesForChunk(x, y);
                }
            }
        }
    }
    return !chunksToUnload.empty();
}

// takes in player position (cameraPosition instance variable) and updates chunks based on that
std::vector<Cube*> TerrainGenerator::updatePlayerPosition(const glm::vec3& newPosition, std::vector<Cube*> oldCubes, bool isFirst) {
    playerPosition = newPosition;

    std::vector<Cube*> cubesVector = oldCubes;
    // should return true or false
    if(checkAndLoadChunks() || isFirst){
        cubesVector.clear();
        for (const auto& chunkEntry : chunkMatrices1) {
            const auto& chunkMatrix = chunkEntry.second;

            for (const auto& cubeIndex : chunkMatrix) {
                const auto& cube = cubeIndex.second;
                cubesVector.push_back(cube);
            }
        }
    }
    return cubesVector;
}

// finds the z value of the block underneath the camera position passed in.
int TerrainGenerator::getGroundHeight(glm::vec3 position) {
    // Adjust position for the fact that the original chunk is centered on the player.
    float adjustedPositionX = position.x + (position.x / abs(position.x)) * (float) chunkSize / 2.f;
    float adjustedPositionY = position.y + (position.y / abs(position.y)) * (float) chunkSize / 2.f;

    // Index of the current chunk that the player is in.
    int currentChunkX = static_cast<int>(adjustedPositionX / (chunkSize * offset));
    int currentChunkY = static_cast<int>(adjustedPositionY / (chunkSize * offset));

    // Value of X and Y, anywhere from 0 -> chunkSize, within the chunk.
    int chunkX = (int) position.x - chunkSize * currentChunkX + chunkSize / 2;
    int chunkY = (int) position.y - chunkSize * currentChunkY + chunkSize / 2;

    // Offset by -1 if it's in a negative chunk so value starts at 0.
    chunkX = (currentChunkX < 0) ? chunkX - 1 : chunkX;
    chunkY = (currentChunkY < 0) ? chunkY - 1 : chunkY;

    // Check the block above and below the Z position.
    bool inWater = false;
    for (int i = -1; i <= 1; i++) {
        int chunkZ = floor(position.z - 0.5) + i + chunkDepth + chunkDepth / 2;
        // If any of the blocks are filled, return false.
        if (chunkMatrices1[{currentChunkX, currentChunkY}].find({chunkX, chunkY, chunkZ}) !=
            chunkMatrices1[{currentChunkX, currentChunkY}].end()) {
            int id = chunkMatrices1[{currentChunkX, currentChunkY}][{chunkX, chunkY, chunkZ}]->getID();
            if (id != 5) {
                return 0;
            } else inWater = true;
        }
    }
    return (inWater) ? 2 : 1;
}

// getter method for chunk data.
const std::map<std::pair<int, int>, std::map<std::tuple<int, int, int>, Cube*>>& TerrainGenerator::getChunkMatrices() const {
    return chunkMatrices1;
}

int TerrainGenerator::getRandomInt() {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Set up a discrete distribution where 0 has a 90% probability,
    // and -1, 1, 2, 3 each have a 2.5% probability
    std::discrete_distribution<> dis({50, 20, 20, 5, 5});

    // Generate a number based on the distribution
    int number = dis(gen);

    // Map the generated number to the desired range and return it
    if (number == 0) {
        return 0;  // 50% chance
    } else if (number == 1) {
        return -1; // 20% chance
    } else {
        return number - 1; // 20% chance for 1, 5% each for 2 and 3
    }


}

