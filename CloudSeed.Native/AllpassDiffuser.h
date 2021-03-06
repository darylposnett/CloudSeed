
#ifndef ALLPASSDIFFUSER
#define ALLPASSDIFFUSER

#include <vector>
#include "Default.h"
#include "ModulatedAllpass.h"

using namespace std;

namespace CloudSeed
{
	class AllpassDiffuser
	{
	public:
		static const int MaxStageCount = 8;

	private:
		int samplerate;

		vector<ModulatedAllpass*> filters;
		int delay;
		double modRate;
		vector<double> seedValues;
		int seed;
		double crossSeed;
		
	public:
		int Stages;

		AllpassDiffuser(int bufferSize, int samplerate);
		~AllpassDiffuser();
		int GetSamplerate();
		void SetSamplerate(int samplerate);		
		void SetSeed(int seed);
		void SetCrossSeed(double crossSeed);

		bool GetModulationEnabled();
		void SetModulationEnabled(bool value);
		void SetInterpolationEnabled(bool enabled);
		double* GetOutput();
		
		void SetDelay(int delaySamples);
		void SetFeedback(double feedback);
		void SetModAmount(double amount);
		void SetModRate(double rate);
		void Process(double* input, int sampleCount);
		void ClearBuffers();

	private:
		void Update();
		void UpdateSeeds();
	};
}
#endif
