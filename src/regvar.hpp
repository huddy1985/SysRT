/***************************************************************************
Author: Giuseppe Lipari
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __REGVAR_HPP__
#define __REGVAR_HPP__

#include <randomvar.hpp>

    const RandomVar::BASE_KEY_TYPE DeltaName("delta");
    const RandomVar::BASE_KEY_TYPE UnifName1 ("unif");
    const RandomVar::BASE_KEY_TYPE UnifName2 ("uniform");
    const RandomVar::BASE_KEY_TYPE NormalName1 ("normal");
    const RandomVar::BASE_KEY_TYPE NormalName2 ("gauss");
    const RandomVar::BASE_KEY_TYPE ExponentialName1 ("exp");
    const RandomVar::BASE_KEY_TYPE ExponentialName2 ("exponential");
    const RandomVar::BASE_KEY_TYPE ParetoName ("pareto");
    const RandomVar::BASE_KEY_TYPE PoissonName ("poisson");
    const RandomVar::BASE_KEY_TYPE DetName ("trace");
    const RandomVar::BASE_KEY_TYPE GenericName ("PDF");
  
    RandomVar* parsevar(const std::string &str);

#endif
