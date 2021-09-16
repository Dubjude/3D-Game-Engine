#include"Terrain.h"

Terrain::Terrain()
{
	using namespace std::chrono;
	std::cout << "Starting ..." << std::endl;

	auto start0 = high_resolution_clock::now();
	generatePerlin<GLfloat>();
	auto stop0 = high_resolution_clock::now();
	auto duration0 = duration_cast<microseconds>(stop0 - start0);
	std::cout << "Perlin (done) - " << duration0.count() << "ms" << std::endl;

	auto start1 = high_resolution_clock::now();
	getMaxMin();
	auto stop1 = high_resolution_clock::now();
	auto duration1 = duration_cast<microseconds>(stop1 - start1);
	std::cout << "Max Min (done) - " << duration1.count() << "ms" << std::endl;

	auto start2 = high_resolution_clock::now();
	assignColors();
	auto stop2 = high_resolution_clock::now();
	auto duration2 = duration_cast<microseconds>(stop2 - start2);
	std::cout << "Color assign (done) - " << duration2.count() << "ms" << std::endl;

	auto start3 = high_resolution_clock::now();
	generateTriangles();
	auto stop3 = high_resolution_clock::now();
	auto duration3 = duration_cast<microseconds>(stop3 - start3);
	std::cout << "Triangle (done) - " << duration3.count() << "ms" << std::endl;


	std::cout << "Finnished!" << std::endl;
}

void Terrain::setupPositions()
{
	for (int i = 0; i < arrSize; i++) 
	{
		cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

		for (int n = 0; n < noiseArr.size(); n++)
		{
			noiseArr[n].positions.push_back(0.0f);
		}
	}
	for (int n = 0; n < noiseArr.size(); n++)
	{
		noiseArr[n].scale = 10.0f;
		noiseArr[n].octaves = n + 2;
		noiseArr[n].bias = (float)pow(2, -n);
	}
}

template <typename Type>
void Terrain::generatePerlin()
{
	for (int n = 0; n < noiseArr.size(); n++)
	{
		float* seed = new float[arrSize];
		float* output = new float[arrSize];
		setupPositions();

		for (int i = 0; i < arrSize; i++)
		{
			seed[i] = (float)rand() / (float)RAND_MAX;
		}

		noise.PerlinNoise2D(arrWidth, arrHeight, seed, noiseArr[n].octaves, noiseArr[n].bias, output);

		for (int x = 0; x < arrWidth; x++)
		{
			for (int y = 0; y < arrHeight; y++)
			{
				int index = x + (y * arrWidth);

				noiseArr[n].positions[index] = output[index] * noiseArr[n].scale;
			}
		}
		delete[] seed;
		delete[] output;
	}

	float biasSum = 0.0f;
	for (int n = 0; n < noiseArr.size(); n++)
	{
		biasSum += noiseArr[n].bias;
	}

	for (int x = 0; x < arrWidth; x++)
	{
		for (int y = 0; y < arrHeight; y++)
		{
			int index = x + (y * arrWidth);

			float sum = 0;
			for (int n = 0; n < noiseArr.size(); n++)
			{
				sum += noiseArr[n].positions[index];
			}

			float elevation = sum / biasSum;
			cubePositions[index][1] = (Type)(pow(elevation * fudge_factor, exponent));
			cubePositions[index][0] = x * trigStep;
			cubePositions[index][2] = y * trigStep;
		}
	}

}

void Terrain::getMaxMin()
{
	for (int i = 0; i < arrSize; i++) 
	{
		if (cubePositions[i][1] < minHeight) minHeight = cubePositions[i][1];
		if (cubePositions[i][1] > maxHeight) maxHeight = cubePositions[i][1];
	}
}

void Terrain::assignColors()
{
	float heightDiff = maxHeight - minHeight;
	waterLevel = waterPercent * heightDiff;
	float landDiff = (heightDiff - waterLevel) / land.size();

	for (int i = 0; i < land.size(); i++) 
	{
		landAssign[i + 1] = (landDiff * i * land[i][0]) + waterLevel;
	}
	landAssign[0] = waterLevel;
}

glm::vec3 Terrain::getColor(float yPos)
{
	if (yPos <= waterLevel) return water;

	for (int i = 1; i < land.size() + 1; i++) 
	{
		if (yPos > landAssign[i - 1] and yPos < landAssign[i]) {
			int index = i - 1;
			return glm::vec3(land[index][1], land[index][2], land[index][3]);
		}
	}
	return glm::vec3(0.0f, 0.0f, 0.0f); // exception case
}

void Terrain::writeRow(int row_n, glm::vec3 point, glm::vec3 color,
	float t0, float t1, float t_ID)
{
	vertices.push_back(point[0]);
	vertices.push_back(point[1]);
	vertices.push_back(point[2]);
	vertices.push_back(color[0]);
	vertices.push_back(color[1]);
	vertices.push_back(color[2]);
	vertices.push_back(t0);
	vertices.push_back(t1);
	vertices.push_back(t_ID);
}

void Terrain::generateTriangles()
{
	for (int x = 0; x < (arrWidth - 1); x++) 
	{
		for (int y = 0; y < (arrHeight - 1); y++) 
		{
			int trig_n = x + (y * arrWidth);

			int i1 = x + (y * arrWidth);
			glm::vec3 p1(cubePositions[i1][0], cubePositions[i1][1], cubePositions[i1][2]);
			int i2 = (x + 1) + (y * arrWidth);
			glm::vec3 p2(cubePositions[i2][0], cubePositions[i2][1], cubePositions[i2][2]);
			int i3 = x + ((y + 1) * arrWidth);
			glm::vec3 p3(cubePositions[i3][0], cubePositions[i3][1], cubePositions[i3][2]);
			writeTriangle(trig_n, p1, p2, p3, getColor(p1[1]), 0.0f);

			int i4 = (x + 1) + (y * arrWidth);
			glm::vec3 p4(cubePositions[i4][0], cubePositions[i4][1], cubePositions[i4][2]);
			int i5 = (x + 1) + ((y + 1) * arrWidth);
			glm::vec3 p5(cubePositions[i5][0], cubePositions[i5][1], cubePositions[i5][2]);
			int i6 = x + ((y + 1) * arrWidth);
			glm::vec3 p6(cubePositions[i6][0], cubePositions[i6][1], cubePositions[i6][2]);
			writeTriangle(trig_n, p4, p5, p6, getColor(p1[1]), 0.0f);
		}
	}
}

void Terrain::writeTriangle(int trig_n, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
	glm::vec3 color, float t_ID)
{
	int loc = trig_n * arr_height;
	writeRow(loc, p1, color, 0.0f, 0.0f, 0.0f);
	writeRow(loc + 1, p2, color, 1.0f, 0.0f, 0.0f);
	writeRow(loc + 2, p3, color, 1.0f, 1.0f, 0.0f);
}

void Terrain::generateCubes()
{
	for (int i = 0; i < (width * height); i++)
	{
		glm::vec3 cubePosition(cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);
		writeCube(i, cubePosition, getColor(cubePositions[i][1]), 0.0f);
	}
}

void Terrain::writeCube(int cube_n, glm::vec3 mid,
	glm::vec3 col, float t_ID)
{
	float hs = t_cubeSize / 2;
	int loc = cube_n * arr_height;
	writeRow(loc, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 1, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 2, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 3, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 4, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 5, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);

	writeRow(loc + 6, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 7, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 8, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 9, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 10, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 11, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);

	writeRow(loc + 12, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 13, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 14, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 15, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 16, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 17, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);

	writeRow(loc + 18, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 19, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 20, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 21, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 22, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 23, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);

	writeRow(loc + 24, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 25, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 26, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 27, glm::vec3(mid[0] + hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 28, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 29, glm::vec3(mid[0] - hs, mid[1] - hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);

	writeRow(loc + 30, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
	writeRow(loc + 31, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 1.0f, t_ID);
	writeRow(loc + 32, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 33, glm::vec3(mid[0] + hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 1.0f, 0.0f, t_ID);
	writeRow(loc + 34, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] + hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 0.0f, t_ID);
	writeRow(loc + 35, glm::vec3(mid[0] - hs, mid[1] + hs, mid[2] - hs), glm::vec3(col[0], col[1], col[2]), 0.0f, 1.0f, t_ID);
}