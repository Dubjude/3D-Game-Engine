#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include<iostream>
#include<glad/glad.h>

#include<vector>
#include<map>
#include<array>
#include <chrono>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Noise.h"

// constants 
const int width = 500;
const int height = 500;
const float t_cubeSize = 1.0f;

// ignore
const int arr_width = 9;
const int arr_height = 36;

// for noise
const int noise_levels = 3;

// for color
const int land_levels = 14;

// noise struct
struct sNoise
{
	float scale = 15.0f;
	int octaves = 1;
	float bias = 0.5f;
	std::vector<float> positions;
};

class Terrain
{
public:
	Terrain();
	std::vector<GLfloat> vertices;
	float offset[3] = { -width / 2, -15.0f, -height / 2 };

private:
	int discretizationDeg = 1;   // number of triangles per unit
	float trigStep = 1.0f / discretizationDeg;

	int chunkWidth = width;
	int chunkHeight = height;
	int arrWidth = width * discretizationDeg;
	int arrHeight = height * discretizationDeg;
	const int arrSize = (arrWidth * arrHeight);
	std::vector<glm::vec3> cubePositions;

	// for coloring
	// - water
	float waterPercent = 0.3f;
	float waterLevel;
	glm::vec3 water = { 0.3f, 0.3f, 0.5f };

	// - land
	float landAssign[land_levels + 1];
	std::array<glm::vec4, land_levels> land =
	{ //  Multiplier   /       Color        //
		glm::vec4(1.0f,   0.8f, 0.7f, 0.5f),
		glm::vec4(1.0f,   0.2f, 0.5f, 0.3f),
		glm::vec4(1.0f,   0.3f, 0.6f, 0.3f),
		glm::vec4(1.0f,   0.5f, 0.7f, 0.3f),
		glm::vec4(1.0f,   0.4f, 0.6f, 0.3f),
		glm::vec4(1.0f,   0.3f, 0.5f, 0.3f),
		glm::vec4(1.0f,   0.3f, 0.45f, 0.3f),
		glm::vec4(1.0f,   0.6f, 0.7f, 0.5f),
		glm::vec4(1.0f,   0.8f, 0.8f, 0.6f),
		glm::vec4(1.0f,   0.7f, 0.7f, 0.7f),
		glm::vec4(1.0f,   0.9f, 0.9f, 0.9f),
		glm::vec4(1.0f,   0.7f, 0.7f, 0.67f),
		glm::vec4(1.0f,   0.53f, 0.53f, 0.53f),
		glm::vec4(10.0f,   0.33f, 0.33f, 0.33f)
	}; // better keep first value for last vec4 big to avoid coloring errors
	float maxHeight, minHeight;


	// noise settings
	Noise noise;
	float fudge_factor = 1.2f;
	int exponent = 2;
	std::array<sNoise, noise_levels> noiseArr;

	// functions
	void setupPositions();
	template <typename Type>
	void generatePerlin();

	void getMaxMin();
	void assignColors();
	glm::vec3 getColor(float yPos);

	void writeRow(int row_n, 
		glm::vec3 point,
		glm::vec3 color,
		float t0, float t1, float t_ID);

	void generateTriangles();
	void writeTriangle(int trig_n, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
		glm::vec3 color, float t_ID);

	// old - might not work 
	void generateCubes();
	void writeCube(int cube_n, glm::vec3 mid,
		glm::vec3 col, float t_ID);
};

#endif
