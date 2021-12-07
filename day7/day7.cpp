#include <algorithm>
#include <numeric>
#include <vector>

#include <fmt/core.h>


using i32 = std::int32_t;

i32 ComputeMedianPosition(const std::vector<i32>& crabPositions)
{
	std::vector<i32> crabPositionsCopy = crabPositions;
	size_t n = crabPositionsCopy.size() / 2;
	std::nth_element(crabPositionsCopy.begin(), crabPositionsCopy.begin() + n, crabPositionsCopy.end());
	return crabPositionsCopy[n];
}

i32 ComputeMeanPosition(const std::vector<i32>& crabPositions)
{
	return std::accumulate(crabPositions.begin(), crabPositions.end(), 0) / crabPositions.size();
}

i32 ComputeTotalNeededFuelLinear(const std::vector<i32>& crabPositions, i32 destinationPosition)
{
	auto addDistance = [destinationPosition](i32 subTotal, i32 position) { return subTotal + std::abs(destinationPosition - position); };
	return std::accumulate(crabPositions.begin(), crabPositions.end(), 0U, addDistance);
}

i32 ComputeTotalNeededFuelExponential(const std::vector<i32>& crabPositions, i32 destinationPosition)
{
	auto addDistance = [destinationPosition](i32 subTotal, i32 position)
	{
		i32 distanceTraveled{ std::abs(destinationPosition - position) };
		i32 fuelNeeded{ distanceTraveled % 2 == 0 ? (distanceTraveled / 2) * (distanceTraveled + 1) : (distanceTraveled / 2 + 1) * distanceTraveled };
		return subTotal + fuelNeeded;
	};
	return std::accumulate(crabPositions.begin(), crabPositions.end(), 0U, addDistance);
}

int main()
{
	// Saw this trick in someone's solution for another day of AoC.
	// It's ugly, but I found it amusing.
	std::vector<i32> crabPositions
	{
#include "input.txt"
	};

	i32 destinationMedianPosition{ ComputeMedianPosition(crabPositions) };
	i32 neededFuelMedian{ ComputeTotalNeededFuelLinear(crabPositions, destinationMedianPosition) };

	fmt::print("Destination Position (using median): {}.\n", destinationMedianPosition);
	fmt::print("Needed Fuel: {}.\n", neededFuelMedian);

	i32 destinationMeanPosition{ ComputeMeanPosition(crabPositions) };
	i32 neededFuelMean{ ComputeTotalNeededFuelExponential(crabPositions, destinationMeanPosition) };

	fmt::print("Destination Position (using mean): {}.\n", destinationMeanPosition);
	fmt::print("Needed Fuel: {}.\n", neededFuelMean);

	return 0;
}