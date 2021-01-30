#pragma once

#include <cstdint>
#include <vector>
#include <deque>

namespace cdma
{

	using MotherSequenceIndices = std::pair<std::vector<uint8_t>, std::vector<uint8_t>>;
	using MotherSequences = std::pair<std::deque<bool>, std::deque<bool>>;
	using IndexPair = std::pair<uint8_t, uint8_t>;

	class SequenceGenerator
	{
	public:
		
		SequenceGenerator(IndexPair indices, uint32_t length) :
			registerSumIndices(indices),
			sequenceLength(static_cast<size_t>(length))
		{}

		std::vector<bool> generate() const;

	protected:

		void shiftMotherSequence(std::deque<bool>& motherSequence, std::vector<uint8_t>& xorIndices) const;

	private:

		const IndexPair registerSumIndices;

		const size_t sequenceLength;

		static const size_t REGISTER_LENGTH;

		static MotherSequenceIndices SHIFT_INDICES;
	};

}
