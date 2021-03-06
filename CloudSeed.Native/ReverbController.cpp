#include "ReverbController.h"
#include "AudioLib/ValueTables.h"
#include "AllpassDiffuser.h"
#include "MultitapDiffuser.h"
#include "Utils.h"

using namespace AudioLib;

namespace CloudSeed
{
	ReverbController::ReverbController(int samplerate)
		: channelL(bufferSize, samplerate),	channelR(bufferSize, samplerate)
	{
		this->samplerate = samplerate;
	}

	int ReverbController::GetSamplerate()
	{
		return samplerate;
	}

	void ReverbController::SetSamplerate(int samplerate)
	{
		this->samplerate = samplerate;

		channelL.SetSamplerate(samplerate);
		channelR.SetSamplerate(samplerate);
	}

	int ReverbController::GetParameterCount()
	{
		return (int)Parameter::Count;
	}

	double* ReverbController::GetAllParameters()
	{
		return parameters;
	}

	double ReverbController::GetScaledParameter(Parameter param)
	{
		switch (param)
		{
			// Input
			case Parameter::InputMix:                  return P(Parameter::InputMix);
			case Parameter::PreDelay:                  return (int)(P(Parameter::PreDelay) * 500);

			case Parameter::HighPass:                  return 20 + ValueTables::Get(P(Parameter::HighPass), ValueTables::Response4Oct) * 980;
			case Parameter::LowPass:                   return 400 + ValueTables::Get(P(Parameter::LowPass), ValueTables::Response4Oct) * 19600;

			// Early
			case Parameter::TapCount:                  return 1 + (int)(P(Parameter::TapCount) * (MultitapDiffuser::MaxTaps - 1));
			case Parameter::TapLength:                 return (int)(P(Parameter::TapLength) * 500);
			case Parameter::TapGain:                   return ValueTables::Get(P(Parameter::TapGain), ValueTables::Response2Dec);
			case Parameter::TapDecay:                  return P(Parameter::TapDecay);

			case Parameter::DiffusionEnabled:          return P(Parameter::DiffusionEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::DiffusionStages:           return 1 + (int)(P(Parameter::DiffusionStages) * (AllpassDiffuser::MaxStageCount - 0.001));
			case Parameter::DiffusionDelay:            return (int)(P(Parameter::DiffusionDelay) * 50);
			case Parameter::DiffusionFeedback:         return P(Parameter::DiffusionFeedback);

			// Late
			case Parameter::LineCount:                 return 1 + (int)(P(Parameter::LineCount) * 11.999);
			case Parameter::LineDelay:                 return (int)(P(Parameter::LineDelay) * 500);
			case Parameter::LineDecay:                 return 0.05 + ValueTables::Get(P(Parameter::LineDecay), ValueTables::Response3Dec) * 59.95;

			case Parameter::LateDiffusionEnabled:      return P(Parameter::LateDiffusionEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::LateDiffusionStages:       return 1 + (int)(P(Parameter::LateDiffusionStages) * (AllpassDiffuser::MaxStageCount - 0.001));
			case Parameter::LateDiffusionDelay:        return (int)(P(Parameter::LateDiffusionDelay) * 50);
			case Parameter::LateDiffusionFeedback:     return P(Parameter::LateDiffusionFeedback);

			// Frequency Response
			case Parameter::PostLowShelfGain:          return ValueTables::Get(P(Parameter::PostLowShelfGain), ValueTables::Response2Dec);
			case Parameter::PostLowShelfFrequency:     return 20 + ValueTables::Get(P(Parameter::PostLowShelfFrequency), ValueTables::Response4Oct) * 980;
			case Parameter::PostHighShelfGain:         return ValueTables::Get(P(Parameter::PostHighShelfGain), ValueTables::Response2Dec);
			case Parameter::PostHighShelfFrequency:    return 400 + ValueTables::Get(P(Parameter::PostHighShelfFrequency), ValueTables::Response4Oct) * 19600;
			case Parameter::PostCutoffFrequency:       return 400 + ValueTables::Get(P(Parameter::PostCutoffFrequency), ValueTables::Response4Oct) * 19600;

			// Modulation
			case Parameter::EarlyDiffusionModAmount:   return P(Parameter::EarlyDiffusionModAmount) * 2.5;
			case Parameter::EarlyDiffusionModRate:     return ValueTables::Get(P(Parameter::EarlyDiffusionModRate), ValueTables::Response2Dec) * 5;
			case Parameter::LineModAmount:             return P(Parameter::LineModAmount) * 2.5;
			case Parameter::LineModRate:               return ValueTables::Get(P(Parameter::LineModRate), ValueTables::Response2Dec) * 5;
			case Parameter::LateDiffusionModAmount:    return P(Parameter::LateDiffusionModAmount) * 2.5;
			case Parameter::LateDiffusionModRate:      return ValueTables::Get(P(Parameter::LateDiffusionModRate), ValueTables::Response2Dec) * 5;

			// Seeds
			case Parameter::TapSeed:                   return (int)std::floor(P(Parameter::TapSeed) * 1000000 + 0.001);
			case Parameter::DiffusionSeed:             return (int)std::floor(P(Parameter::DiffusionSeed) * 1000000 + 0.001);
			case Parameter::DelaySeed:                 return (int)std::floor(P(Parameter::DelaySeed) * 1000000 + 0.001);
			case Parameter::PostDiffusionSeed:         return (int)std::floor(P(Parameter::PostDiffusionSeed) * 1000000 + 0.001);

			// Output
			case Parameter::CrossSeed:                 return P(Parameter::CrossSeed);

			case Parameter::DryOut:                    return ValueTables::Get(P(Parameter::DryOut), ValueTables::Response2Dec);
			case Parameter::PredelayOut:               return ValueTables::Get(P(Parameter::PredelayOut), ValueTables::Response2Dec);
			case Parameter::EarlyOut:                  return ValueTables::Get(P(Parameter::EarlyOut), ValueTables::Response2Dec);
			case Parameter::MainOut:                   return ValueTables::Get(P(Parameter::MainOut), ValueTables::Response2Dec);

			// Switches
			case Parameter::HiPassEnabled:             return P(Parameter::HiPassEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::LowPassEnabled:            return P(Parameter::LowPassEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::LowShelfEnabled:           return P(Parameter::LowShelfEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::HighShelfEnabled:          return P(Parameter::HighShelfEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::CutoffEnabled:             return P(Parameter::CutoffEnabled) < 0.5 ? 0.0 : 1.0;
			case Parameter::LateStageTap:			   return P(Parameter::LateStageTap) < 0.5 ? 0.0 : 1.0;

			// Effects
			case Parameter::Interpolation:			   return P(Parameter::Interpolation) < 0.5 ? 0.0 : 1.0;

			default: return 0.0;
		}

		return 0.0;
	}

	void ReverbController::SetParameter(Parameter param, double value)
	{
		parameters[(int)param] = value;
		auto scaled = GetScaledParameter(param);

		if (param == Parameter::CrossSeed) // this is the only parameter that varies between the two channels
		{
			channelL.SetParameter(param, 0);
			channelR.SetParameter(param, scaled);
		}
		else
		{
			channelL.SetParameter(param, scaled);
			channelR.SetParameter(param, scaled);
		}
	}

	void ReverbController::ClearBuffers()
	{
		channelL.ClearBuffers();
		channelR.ClearBuffers();
	}

	void ReverbController::Process(double** input, double** output, int bufferSize)
	{
		auto len = bufferSize;
		auto cm = GetScaledParameter(Parameter::InputMix) * 0.5;
		auto cmi = (1 - cm);

		for (uint i = 0; i < len; i++)
		{
			leftChannelIn[i] = input[0][i] * cmi + input[1][i] * cm;
			rightChannelIn[i] = input[1][i] * cmi + input[0][i] * cm;
		}

		channelL.Process(leftChannelIn, len);
		channelR.Process(rightChannelIn, len);
		auto leftOut = channelL.GetOutput();
		auto rightOut = channelR.GetOutput();

		for (uint i = 0; i < len; i++)
		{
			output[0][i] = leftOut[i];
			output[1][i] = rightOut[i];
		}
	}

	double ReverbController::P(Parameter para)
	{
		auto idx = (int)para;
		return idx >= 0 && idx < (int)Parameter::Count ? parameters[idx] : 0.0;
	}
}
