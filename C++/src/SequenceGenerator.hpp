#pragma once

#include<vector>

#include "cdma.hpp"

namespace cdma
{

	using MotherSequenceIndices = pair<vector<uint8_t>, vector<uint8_t>>;

	class SequenceGenerator
	{
	public:
		
		SequenceGenerator(IndexPair indices, uint32_t length) :
			registerSumIndices(indices),
			sequenceLength(static_cast<size_t>(length))
		{}

		std::vector<bool> generate();

	protected:

		void shiftMotherSequence(deque<bool>& motherSequence, vector<uint8_t>& xorIndices);

	private:

		const IndexPair registerSumIndices;

		const size_t sequenceLength;

		static const size_t REGISTER_LENGTH;

		static MotherSequenceIndices SHIFT_INDICES;
	};

}
