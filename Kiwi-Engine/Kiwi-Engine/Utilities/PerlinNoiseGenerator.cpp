#include "PerlinNoiseGenerator.h"

#include "../Core/Vector3d.h"
#include "../Core/Utilities.h"

#include <random> //for std::default_random_engine
#include <numeric> //for std::iota
#include <algorithm> //for std::shuffle

namespace Kiwi
{

	PerlinNoiseGenerator::PerlinNoiseGenerator() :
		m_octaveCount( 1 ), m_persistence( 0.5 ), m_frequency( 1.0 )
	{

		// Hash lookup table as defined by Ken Perlin.  This is a randomly
		// arranged array of all numbers from 0-255 inclusive.
		//the table is repeated once to prevent overflow
		m_pTable = { 151,160,137,91,90,15,	
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,	
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
			151,160,137,91,90,15,
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};

	}

	PerlinNoiseGenerator::PerlinNoiseGenerator(long seed) :
		m_octaveCount( 1 ), m_persistence( 0.5 ), m_frequency( 1.0 )
	{

		//create a random pTable based on the seed value
		m_pTable.resize( 256 );

		// Fill p with values from 0 to 255
		std::iota( m_pTable.begin(), m_pTable.end(), 0 );

		// Initialize a random engine with seed
		std::default_random_engine engine( seed );

		// Suffle  using the above random engine
		std::shuffle( m_pTable.begin(), m_pTable.end(), engine );

		// Duplicate the permutation vector
		m_pTable.insert( m_pTable.end(), m_pTable.begin(), m_pTable.end() );

	}

	PerlinNoiseGenerator::~PerlinNoiseGenerator()
	{
	}

	double PerlinNoiseGenerator::Perlin( const Kiwi::Vector3d& coordinate )
	{
		return this->Perlin( coordinate, m_octaveCount, m_persistence, m_frequency );
	}

	double PerlinNoiseGenerator::Perlin( const Kiwi::Vector3d& coordinate, int octaveCount, double persistence, double frequency )
	{

		//double x = coordinate.x;
		//double y = coordinate.y;
		//double z = coordinate.z;

		//double total = 0.0;
		//double maxValue = 0.0;

		//for( unsigned int i = 0; i < octaveCount; i++ )
		//{

		//	total += this->_Perlin( x * frequency, y * frequency, z * frequency ) * amplitude;

		//	maxValue += amplitude;
		//	amplitude *= persistence;
		//	frequency *= 2.0;

		//}

		////return value clamped between [0,1]
		//return (maxValue != 0.0) ? total / maxValue : 0.0;

		double x = coordinate.x;
		double y = coordinate.y;
		double z = coordinate.z;

		double total = 0.0;
		double _amp = 1.0;

		for( unsigned int i = 0; i < (unsigned int)octaveCount; i++ )
		{
			total += this->_Perlin( x * frequency, y * frequency, z * frequency ) * _amp;

			_amp *= persistence;
			frequency *= 2.0;
		}

		//return value between [-1,1]
		return total;

	}

	void PerlinNoiseGenerator::SetPersistence( double persistence )
	{
		m_persistence = persistence;
	}

	void PerlinNoiseGenerator::SetOctaveCount( int numOctaves )
	{
		m_octaveCount = numOctaves;
	}

	void PerlinNoiseGenerator::SetFrequency( double frequency )
	{
		m_frequency = frequency;
	}

	double PerlinNoiseGenerator::_Perlin( double x, double y, double z )
	{

		//calculate the unit cube that encapsulates this point
		//the left bound is cubeX,Y,Z and the right bound is that + 1
		int cubeX = _FastFloor(x) & 255;
		int cubeY = _FastFloor(y) & 255;
		int cubeZ = _FastFloor(z) & 255;

		//calculate the position of this point within the unit cube
		double unitPosX = x - _FastFloor( x );
		double unitPosY = y - _FastFloor( y );
		double unitPosZ = z - _FastFloor( z );

		//now 'fade' the location to create a smoother effect
		double u = _Fade( unitPosX );
		double v = _Fade( unitPosY );
		double w = _Fade( unitPosZ );

		//hash the 8 unit cube coordinates (the edges) surrounding the input coordinate
		//this gets a unique value for every coordinate
		int A, AA, AB, B, BA, BB;
		A = m_pTable[cubeX] + cubeY;
		AA = m_pTable[A] + cubeZ;
		AB = m_pTable[A + 1] + cubeZ;
		B = m_pTable[cubeX + 1] + cubeY;
		BA = m_pTable[B] + cubeZ;
		BB = m_pTable[B + 1] + cubeZ;

		//https://gist.github.com/Flafla2/f0260a861be0ebdeef76
		// The gradient function calculates the dot product between a pseudorandom
		// gradient vector and the vector from the input coordinate to the 8
		// surrounding edges in its unit cube.
		// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
		// values we made earlier.

		return Kiwi::Lerp( Kiwi::Lerp( Kiwi::Lerp( _Gradient( m_pTable[AA], unitPosX, unitPosY, unitPosZ ),
												   _Gradient( m_pTable[BA], unitPosX - 1.0, unitPosY, unitPosZ ),
												   u ),
									   Kiwi::Lerp( _Gradient( m_pTable[AB], unitPosX, unitPosY - 1.0, unitPosZ ),
												   _Gradient( m_pTable[BB], unitPosX - 1.0, unitPosY - 1.0, unitPosZ ),
												   u ),
									   v ),
						   Kiwi::Lerp( Kiwi::Lerp( _Gradient( m_pTable[AA + 1], unitPosX, unitPosY, unitPosZ - 1.0 ),
												   _Gradient( m_pTable[BA + 1], unitPosX - 1.0, unitPosY, unitPosZ - 1.0 ),
												   u ),
									   Kiwi::Lerp( _Gradient( m_pTable[AB + 1], unitPosX, unitPosY - 1.0, unitPosZ - 1.0 ),
												   _Gradient( m_pTable[BB + 1], unitPosX - 1.0, unitPosY - 1.0, unitPosZ - 1.0 ),
												   u ),
									   v ),
						   w );

	}

	double PerlinNoiseGenerator::_Fade( double t )
	{

		return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3

	}

	double PerlinNoiseGenerator::_Gradient( int hash, double x, double y, double z )
	{

		/*given the hash, picks a random vector from amongst the cube edges:
		(1,1,0),(-1,1,0),(1,-1,0),(-1,-1,0),
		(1,0,1),(-1,0,1),(1,0,-1),(-1,0,-1),
		(0,1,1),(0,-1,1),(0,1,-1),(0,-1,-1)*/
		int h = hash & 15;
		// Convert lower 4 bits of hash inot 12 gradient directions
		double u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);

	}

	int PerlinNoiseGenerator::_FastFloor( double value )
	{
		return (value > 0) ? (int)value : (int)(value - 1);
	}

	double PerlinNoiseGenerator::_Noise( int x, int y, int z )
	{

		int n = (x + y * 57) + (z * 29);
		n = (n << 13) ^ n;
		return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);

	}

}