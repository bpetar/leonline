/** 
 * \file Particles.h
 * \brief CParticles class is containing Particle effects.
 * 
 * \author Petar Bajic, MPE (C) All Rights Reserved, Homepage: www.mystic-peanut.com
 * \date October, 21 2011.
 */

#if !defined(AFX_ParticlesMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_)
#define AFX_ParticlesMANAGER_H__AB04DC05_BB05_11D4_87CB_00C04F73BBBB__INCLUDED_

#include <irrlicht.h>
using namespace irr;
using namespace core;

//! Particle effect kind.
/** Particle effect type can be whirl, fire, explosion, etc... */
enum PARTICLES_EFFECT_TYPE
{
	//!Particles circling in cylinder like a whirlwind
	PARTICLES_EFFECT_WHIRL = 0,

	//!Particles blasting from one point in all directions
	PARTICLES_EFFECT_BLAST,

	//!Particles slowly rising in magic field area
	PARTICLES_EFFECT_FIELD,

	//!Fire looking effect
	PARTICLES_EFFECT_FIRE,

	//!Smoke looking effect
	PARTICLES_EFFECT_SMOKE
};

//! Particle effect color.
/** Particle effect color can be red, green, blue, etc... */
enum PARTICLES_EFFECT_COLOR
{
	//!Particles color red
	PARTICLES_EFFECT_RED = 0,

	//!Particles color green
	PARTICLES_EFFECT_GREEN
};

class CGameManager;

class CParticles
{
public:

	CParticles();
	~CParticles();
	
	CGameManager* m_GameManager;

};

#endif;