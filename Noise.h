#ifndef NOISE_CLASS_H
#define NOISE_CLASS_H

#include <iostream>
#include <string>
#include <algorithm>

class Noise 
{
public:
	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput);
	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);

private:
	double pi = 3.1415927;

	// linear interpolation
	float lerp(float blend, float sample1, float sample2);
	// cosine interpolation
	float coserp(float blend, float sample1, float sample2);
};

#endif
