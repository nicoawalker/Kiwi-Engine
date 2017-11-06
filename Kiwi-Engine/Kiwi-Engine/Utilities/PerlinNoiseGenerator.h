#ifndef _KIWI_PERLINNOISEGENERATOR_H_
#define _KIWI_PERLINNOISEGENERATOR_H_

#include <vector>

namespace Kiwi
{

	class Vector3d;

	class PerlinNoiseGenerator
	{
	protected:

		int m_octaveCount;

		double m_persistence;
		double m_frequency;

		std::vector<unsigned int> m_pTable;

	public:

		PerlinNoiseGenerator();
		PerlinNoiseGenerator( long seed );

		~PerlinNoiseGenerator();

		double Perlin( const Kiwi::Vector3d& coordinate );
		double Perlin( const Kiwi::Vector3d& coordinate, int octaveCount, double persistence, double frequency );

		void SetPersistence( double persistence );
		void SetOctaveCount( int numOctaves );
		void SetFrequency( double frequency );

	protected:

		double _Perlin( double x, double y, double z );

		double _Fade( double t );

		double _Gradient( int hash, double x, double y, double z );

		int _FastFloor( double value );

		double _Noise(int x, int y, int z);
		//double _SmoothNoise(double x, double y, double z);

	};

}

#endif