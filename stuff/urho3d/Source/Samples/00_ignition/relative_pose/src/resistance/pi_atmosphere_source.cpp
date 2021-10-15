
#include "pi_atmosphere_source.h"
#include "pi_consts.h"

namespace Ign
{

PiAtmosphereSource::PiAtmosphereSource()
    : AtmosphereSource()
{
}

PiAtmosphereSource::PiAtmosphereSource( const PiSourceDesc & desc )
{
    *this = desc;
}

PiAtmosphereSource::~PiAtmosphereSource()
{
}

void PiAtmosphereSource::operator=( const PiSourceDesc & desc )
{
    desc_ = desc;

    Float specificHeatCp;
    Float gasMolarMass;
    if ( desc.super_type_ == Ign::SUPERTYPE_GAS_GIANT )
    {
        specificHeatCp = 12950.0; // constant pressure specific heat, for a combination of hydrogen and helium
        gasMolarMass = 0.0023139903;
    }
    else
    {
        specificHeatCp = 1000.5; // constant pressure specific heat, for the combination of gasses that make up air
        // XXX using earth's molar mass of air...
        gasMolarMass = 0.02897;
    }
    const Float GAS_CONSTANT = 8.3144621;
    const Float PA_2_ATMOS = 1.0 / 101325.0;

    // surface gravity = -G*M/planet radius^2
    const Float M = desc_.mass();
    const Float R = desc_.radius();
    surface_gravity_ = -(G * M) / (R * R);
    const Float lapseRate_L = -surface_gravity_ / specificHeatCp; // negative deg/m
    const Float surfaceTemperature_T0 = static_cast<Float>( desc_.average_temp_ ); //K

    //double surfaceDensity, h;
    //Color c;
    //sbody->GetAtmosphereFlavor(&c, &surfaceDensity); // kg / m^3
    Float surfaceDensity = desc_.atmos_density_.ToDouble();
    surfaceDensity /= gasMolarMass; // convert to moles/m^3

    //P = density*R*T=(n/V)*R*T
    const Float surfaceP_p0 = PA_2_ATMOS * ((surfaceDensity)*GAS_CONSTANT * surfaceTemperature_T0); // in atmospheres
    Float h;
    if ( surfaceP_p0 < 0.002 )
    {
        h = 0.0;
    }
    else
    {
        //*outPressure = p0*(1-l*h/T0)^(g*M/(R*L);
        // want height for pressure 0.001 atm:
        // h = (1 - exp(RL/gM * log(P/p0))) * T0 / l
        const Float RLdivgM = (GAS_CONSTANT * lapseRate_L) / (-surface_gravity_ * gasMolarMass);
        h = (1.0 - std::exp( RLdivgM * std::log( 0.001 / surfaceP_p0 ) ) ) * surfaceTemperature_T0 / lapseRate_L;
        //		double h2 = (1.0 - pow(0.001/surfaceP_p0, RLdivgM)) * surfaceTemperature_T0 / lapseRate_L;
        //		double P = surfaceP_p0*pow((1.0-lapseRate_L*h/surfaceTemperature_T0),1/RLdivgM);
    }
    atmosphere_radius_ = h + R;
}

bool PiAtmosphereSource::params( Float distFromCenter, Float & pressure, Float & density ) const
{
    // This model has no atmosphere beyond the adiabetic limit
    // Note: some code duplicated in InitParams(). Check if changing.
    if ( distFromCenter >= atmosphere_radius_ )
    {
        density  = 0.0;
        pressure = 0.0;
        return false;
    }

    Float surfaceDensity;
    Float specificHeatCp;
    Float gasMolarMass;
    if ( desc_.super_type_ == Ign::SUPERTYPE_GAS_GIANT )
    {
        specificHeatCp = 12950.0; // constant pressure specific heat, for a combination of hydrogen and helium
        gasMolarMass = 0.0023139903;
    }
    else
    {
        specificHeatCp = 1000.5; // constant pressure specific heat, for the combination of gasses that make up air
        // XXX using earth's molar mass of air...
        gasMolarMass = 0.02897;
    }
    const Float GAS_CONSTANT = 8.3144621;
    const Float PA_2_ATMOS   = 1.0 / 101325.0;

    // lapse rate http://en.wikipedia.org/wiki/Adiabatic_lapse_rate#Dry_adiabatic_lapse_rate
    // the wet adiabatic rate can be used when cloud layers are incorporated
    // fairly accurate in the troposphere
    const Float lapseRate_L = -surface_gravity_ / specificHeatCp; // negative deg/m

    const Float height_h = ( distFromCenter - desc_.radius() ); // height in m
    const Float surfaceTemperature_T0 = static_cast<Float>( desc_.average_temp_ ); //K

    //Color c;
    //sbody->GetAtmosphereFlavor( &c, &surfaceDensity ); // kg / m^3
    surfaceDensity = desc_.atmos_density_.ToDouble();
    // convert to moles/m^3
    surfaceDensity /= gasMolarMass;

    //P = density*R*T=(n/V)*R*T
    const Float surfaceP_p0 = PA_2_ATMOS * ( (surfaceDensity)*GAS_CONSTANT * surfaceTemperature_T0 ); // in atmospheres

    // height below zero should not occur
    if (height_h < 0.0)
    {
        pressure = surfaceP_p0;
        density = surfaceDensity * gasMolarMass;
        return true;
    }

    //*outPressure = p0*(1-l*h/T0)^(g*M/(R*L);
    pressure = surfaceP_p0 * std::pow( ( 1.0 - (lapseRate_L * height_h) / surfaceTemperature_T0 ),
                                       ( -surface_gravity_ * gasMolarMass / (GAS_CONSTANT * lapseRate_L) ) ); // in ATM since p0 was in ATM
    //                                                                               ^^g used is abs(g)
    // temperature at height
    const Float temp = surfaceTemperature_T0 + lapseRate_L * height_h;

    density = (pressure / (PA_2_ATMOS * GAS_CONSTANT * temp)) * gasMolarMass;

    return true;
}


}




