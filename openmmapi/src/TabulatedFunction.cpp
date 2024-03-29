/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2014-2022 Stanford University and the Authors.      *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "openmm/TabulatedFunction.h"
#include "openmm/OpenMMException.h"

using namespace OpenMM;
using namespace std;

bool TabulatedFunction::getPeriodic() const {
    return periodic;
}

int TabulatedFunction::getUpdateCount() const {
    return updateCount;
}

Continuous1DFunction::Continuous1DFunction(const vector<double>& values, double min, double max, bool periodic) {
    this->periodic = periodic;
    setFunctionParameters(values, min, max);
}

void Continuous1DFunction::getFunctionParameters(vector<double>& values, double& min, double& max) const {
    values = this->values;
    min = this->min;
    max = this->max;
}

void Continuous1DFunction::setFunctionParameters(const vector<double>& values, double min, double max) {
    if (max <= min)
        throw OpenMMException("Continuous1DFunction: max <= min for a tabulated function.");
    int n = values.size();
    if (periodic) {
        if (n < 3)
            throw OpenMMException("Continuous1DFunction: a periodic tabulated function must have at least three points");
       // Note: value-matching at boundary is eventually checked at spline creation.
       // if (values[0] != values[n-1])
       //     throw OpenMMException("Continuous1DFunction: with periodic=true, the first and last points must have the same value");
    }
    else if (n < 2)
        throw OpenMMException("Continuous1DFunction: a non-periodic tabulated function must have at least two points");
    this->values = values;
    this->min = min;
    this->max = max;
    updateCount++;
}

Continuous1DFunction* Continuous1DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Continuous1DFunction(new_vec, min, max);
}

bool Continuous1DFunction::operator==(const TabulatedFunction& other) const {
    const Continuous1DFunction* fn = dynamic_cast<const Continuous1DFunction*>(&other);
    if (fn == NULL)
        return false;
    if (fn->min != min || fn->max != max)
        return false;
    return (fn->values == values);
}

Continuous2DFunction::Continuous2DFunction(int xsize, int ysize, const vector<double>& values, double xmin, double xmax, double ymin, double ymax, bool periodic) {
    this->periodic = periodic;
    setFunctionParameters(xsize, ysize, values, xmin, xmax, ymin, ymax);
}

void Continuous2DFunction::getFunctionParameters(int& xsize, int& ysize, vector<double>& values, double& xmin, double& xmax, double& ymin, double& ymax) const {
    values = this->values;
    xsize = this->xsize;
    ysize = this->ysize;
    xmin = this->xmin;
    xmax = this->xmax;
    ymin = this->ymin;
    ymax = this->ymax;
}

void Continuous2DFunction::setFunctionParameters(int xsize, int ysize, const vector<double>& values, double xmin, double xmax, double ymin, double ymax) {
    if (periodic) {
        if (xsize < 3 || ysize < 3)
           throw OpenMMException("Continuous2DFunction: must have at least three points along each axis if periodic");
        // Note: value-matching at boundary is eventually checked at 2D-spline creation.
    }
    else if (xsize < 2 || ysize < 2)
        throw OpenMMException("Continuous2DFunction: must have at least two points along each axis");
    if (values.size() != xsize*ysize)
        throw OpenMMException("Continuous2DFunction: incorrect number of values");
    if (xmax <= xmin)
        throw OpenMMException("Continuous2DFunction: xmax <= xmin for a tabulated function.");
    if (ymax <= ymin)
        throw OpenMMException("Continuous2DFunction: ymax <= ymin for a tabulated function.");
    this->values = values;
    this->xsize = xsize;
    this->ysize = ysize;
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    updateCount++;
}

Continuous2DFunction* Continuous2DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Continuous2DFunction(xsize, ysize, new_vec, xmin, xmax, ymin, ymax);
}

bool Continuous2DFunction::operator==(const TabulatedFunction& other) const {
    const Continuous2DFunction* fn = dynamic_cast<const Continuous2DFunction*>(&other);
    if (fn == NULL)
        return false;
    if (fn->xsize != xsize || fn->ysize != ysize)
        return false;
    if (fn->xmin != xmin || fn->xmax != xmax)
        return false;
    if (fn->ymin != ymin || fn->ymax != ymax)
        return false;
    return (fn->values == values);
}

Continuous3DFunction::Continuous3DFunction(int xsize, int ysize, int zsize, const vector<double>& values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, bool periodic) {
    this->periodic = periodic;
    setFunctionParameters(xsize, ysize, zsize, values, xmin, xmax, ymin, ymax, zmin, zmax);
}

void Continuous3DFunction::getFunctionParameters(int& xsize, int& ysize, int& zsize, vector<double>& values, double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) const {
    values = this->values;
    xsize = this->xsize;
    ysize = this->ysize;
    zsize = this->zsize;
    xmin = this->xmin;
    xmax = this->xmax;
    ymin = this->ymin;
    ymax = this->ymax;
    zmin = this->zmin;
    zmax = this->zmax;
}

void Continuous3DFunction::setFunctionParameters(int xsize, int ysize, int zsize, const vector<double>& values, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) {
    if (periodic) {
       if (xsize < 3 || ysize < 3 || zsize < 3)
           throw OpenMMException("Continuous3DFunction: must have at least three points along each axis if periodic");
       // Note: value-matching at boundary is eventually checked at 3D-spline creation.
    }
    else if (xsize < 2 || ysize < 2 || zsize < 2)
        throw OpenMMException("Continuous3DFunction: must have at least two points along each axis");
    if (values.size() != xsize*ysize*zsize)
        throw OpenMMException("Continuous3DFunction: incorrect number of values");
    if (xmax <= xmin)
        throw OpenMMException("Continuous3DFunction: xmax <= xmin for a tabulated function.");
    if (ymax <= ymin)
        throw OpenMMException("Continuous3DFunction: ymax <= ymin for a tabulated function.");
    if (zmax <= zmin)
        throw OpenMMException("Continuous3DFunction: zmax <= zmin for a tabulated function.");
    this->values = values;
    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
    this->zmin = zmin;
    this->zmax = zmax;
    updateCount++;
}

Continuous3DFunction* Continuous3DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Continuous3DFunction(xsize, ysize, zsize, new_vec, xmin, xmax, ymin, ymax, zmin, zmax);
}

bool Continuous3DFunction::operator==(const TabulatedFunction& other) const {
    const Continuous3DFunction* fn = dynamic_cast<const Continuous3DFunction*>(&other);
    if (fn == NULL)
        return false;
    if (fn->xsize != xsize || fn->ysize != ysize || fn->zsize != zsize)
        return false;
    if (fn->xmin != xmin || fn->xmax != xmax)
        return false;
    if (fn->ymin != ymin || fn->ymax != ymax)
        return false;
    if (fn->zmin != zmin || fn->zmax != zmax)
        return false;
    return (fn->values == values);
}

Discrete1DFunction::Discrete1DFunction(const vector<double>& values) {
    this->values = values;
}

void Discrete1DFunction::getFunctionParameters(vector<double>& values) const {
    values = this->values;
}

void Discrete1DFunction::setFunctionParameters(const vector<double>& values) {
    this->values = values;
    updateCount++;
}

Discrete1DFunction* Discrete1DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Discrete1DFunction(new_vec);
}

bool Discrete1DFunction::operator==(const TabulatedFunction& other) const {
    const Discrete1DFunction* fn = dynamic_cast<const Discrete1DFunction*>(&other);
    if (fn == NULL)
        return false;
    return (fn->values == values);
}

Discrete2DFunction::Discrete2DFunction(int xsize, int ysize, const vector<double>& values) {
    if (values.size() != xsize*ysize)
        throw OpenMMException("Discrete2DFunction: incorrect number of values");
    this->xsize = xsize;
    this->ysize = ysize;
    this->values = values;
}

void Discrete2DFunction::getFunctionParameters(int& xsize, int& ysize, vector<double>& values) const {
    xsize = this->xsize;
    ysize = this->ysize;
    values = this->values;
}

void Discrete2DFunction::setFunctionParameters(int xsize, int ysize, const vector<double>& values) {
    if (values.size() != xsize*ysize)
        throw OpenMMException("Discrete2DFunction: incorrect number of values");
    this->xsize = xsize;
    this->ysize = ysize;
    this->values = values;
    updateCount++;
}

Discrete2DFunction* Discrete2DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Discrete2DFunction(xsize, ysize, new_vec);
}

bool Discrete2DFunction::operator==(const TabulatedFunction& other) const {
    const Discrete2DFunction* fn = dynamic_cast<const Discrete2DFunction*>(&other);
    if (fn == NULL)
        return false;
    if (fn->xsize != xsize || fn->ysize != ysize)
        return false;
    return (fn->values == values);
}

Discrete3DFunction::Discrete3DFunction(int xsize, int ysize, int zsize, const vector<double>& values) {
    if (values.size() != xsize*ysize*zsize)
        throw OpenMMException("Discrete3DFunction: incorrect number of values");
    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;
    this->values = values;
}

void Discrete3DFunction::getFunctionParameters(int& xsize, int& ysize, int& zsize, vector<double>& values) const {
    xsize = this->xsize;
    ysize = this->ysize;
    zsize = this->zsize;
    values = this->values;
}

void Discrete3DFunction::setFunctionParameters(int xsize, int ysize, int zsize, const vector<double>& values) {
    if (values.size() != xsize*ysize*zsize)
        throw OpenMMException("Discrete3DFunction: incorrect number of values");
    this->xsize = xsize;
    this->ysize = ysize;
    this->zsize = zsize;
    this->values = values;
    updateCount++;
}

Discrete3DFunction* Discrete3DFunction::Copy() const {
    vector<double> new_vec(values.size());
    for (size_t i = 0; i < values.size(); i++)
        new_vec[i] = values[i];
    return new Discrete3DFunction(xsize, ysize, zsize, new_vec);
}

bool Discrete3DFunction::operator==(const TabulatedFunction& other) const {
    const Discrete3DFunction* fn = dynamic_cast<const Discrete3DFunction*>(&other);
    if (fn == NULL)
        return false;
    if (fn->xsize != xsize || fn->ysize != ysize || fn->zsize != zsize)
        return false;
    return (fn->values == values);
}
