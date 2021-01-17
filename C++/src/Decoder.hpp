#pragma once

#include<cstdint>
#include<vector>

#include"SequenceGenerator.hpp"

namespace cdma
{

	struct Correlation
	{
		uint16_t satelliteId;
		uint16_t offset;
		bool message;
	};

	class Decoder
	{
	public:

		Decoder(std::vector<int16_t>& sequence);

		std::vector<Correlation> decode(const std::vector<SequenceGenerator>& generators) const;

	protected:

	private:

		void correlate(
			std::vector<bool>& sequence,
			const uint16_t peak,
			size_t satelliteId,
			std::vector<Correlation>* outResult) const;

	private:
		
		std::vector<int16_t> chipSequence;

		const uint16_t satelliteCount;

		static const uint16_t MAX_DEVIATION;
	};
}