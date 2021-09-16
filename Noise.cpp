#include"Noise.h"

void Noise::PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput)
{
	for (int x = 0; x < nCount; x++)
	{
		float fNoise = 0.0f;
		float fScaleAcc = 0.0f;
		float fScale = 1.0f;

		for (int o = 0; o < nOctaves; o++)
		{
			int nPitch = nCount >> o;
			int nSample1 = (x / nPitch) * nPitch;
			int nSample2 = (nSample1 + nPitch) % nCount;

			float fBlend = (float)(x - nSample1) / (float)nPitch;

			float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

			fScaleAcc += fScale;
			fNoise += fSample * fScale;
			fScale = fScale / fBias;
		}

		fOutput[x] = fNoise / fScaleAcc;
	}
}

void Noise::PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
{
	for (int x = 0; x < nWidth; x++)
	{
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;

				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = coserp(fBlendX, fSeed[nSampleY1 * nWidth + nSampleX1], fSeed[nSampleY1 * nWidth + nSampleX2]);
				float fSampleB = coserp(fBlendX, fSeed[nSampleY2 * nWidth + nSampleX1], fSeed[nSampleY2 * nWidth + nSampleX2]);

				fScaleAcc += fScale;
				float blend2 = (1 - cos(fBlendY * pi)) / 2; // only for coserp
				fNoise += (blend2 * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			fOutput[y * nWidth + x] = fNoise / fScaleAcc;
		}
	}

}

float Noise::lerp(float blend, float sample1, float sample2)
{
	return (1.0f - blend) * sample1 + blend * sample2;
}


float Noise::coserp(float blend, float sample1, float sample2)
{
	float blend2 = (1.0f - cos(blend * pi)) / 2;
	return lerp(blend2, sample1, sample2); //(sample1 * (1.0f - blend2) + sample2 * blend2);
}