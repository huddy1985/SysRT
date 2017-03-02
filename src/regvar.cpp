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
#include <cstdlib>
#include <regvar.hpp>
#include <factory.hpp>
#include <strtoken.hpp>

using namespace std;
using namespace parse_util;

/**
       This namespace should not be visible, and in any case, users
       should never access objects of this namefile. This is used just
       for initialization of the objects needed for the abstract
       factory that creates RandomVars.
 */
namespace __var_stub
{
static registerInFactory<RandomVar,
DeltaVar,
RandomVar::BASE_KEY_TYPE>
registerDelta(DeltaName);

static registerInFactory<RandomVar,
UniformVar,
RandomVar::BASE_KEY_TYPE>
registerUnif1(UnifName1);

static registerInFactory<RandomVar,
UniformVar,
RandomVar::BASE_KEY_TYPE>
registerUnif2(UnifName2);

static registerInFactory<RandomVar,
NormalVar,
RandomVar::BASE_KEY_TYPE>
registerNormal1(NormalName1);

static registerInFactory<RandomVar,
NormalVar,
RandomVar::BASE_KEY_TYPE>
registerNormal2(NormalName2);

static registerInFactory<RandomVar,
ExponentialVar,
RandomVar::BASE_KEY_TYPE>
registerExp1(ExponentialName1);

static registerInFactory<RandomVar,
ExponentialVar,
RandomVar::BASE_KEY_TYPE>
registerExp2(ExponentialName2);

static registerInFactory<RandomVar,
ParetoVar,
RandomVar::BASE_KEY_TYPE>
registerPareto(ParetoName);

static registerInFactory<RandomVar,
PoissonVar,
RandomVar::BASE_KEY_TYPE>
registerPoisson(PoissonName);

static registerInFactory<RandomVar,
DetVar,
RandomVar::BASE_KEY_TYPE>
registerDet(DetName);


}

RandomVar *parsevar(const std::string &str)
{
	RandomVar *temp;

	string token = get_token(str);

	string p = get_param(str);

	vector<string> parms = split_param(p);


	auto_ptr<RandomVar>
	var(genericFactory<RandomVar>::instance().create(token,parms));

	if (var.get() == 0) throw ParseExc("parsevar", str);

	temp = var.release();

	return temp;
}
