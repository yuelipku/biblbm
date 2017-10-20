/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2013 FlowKit Sarl
 * Route d'Oron 2
 * 1010 Lausanne, Switzerland
 * E-mail contact: contact@flowkit.com
 *
 * The most recent release of Palabos can be downloaded at 
 * <http://www.palabos.org/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
 * Helper functions for domain initialization -- generic implementation.
 */
#ifndef DATA_INITIALIZER_WRAPPER_2D_HH
#define DATA_INITIALIZER_WRAPPER_2D_HH

#include "dataProcessors/dataInitializerWrapper2D.h"
#include "core/cell.h"
#include "atomicBlock/dataProcessorWrapper2D.h"
#include "atomicBlock/blockLattice2D.h"
#include "atomicBlock/dataField2D.h"
#include "multiBlock/multiBlockLattice2D.h"
#include "multiBlock/multiDataField2D.h"
#include "latticeBoltzmann/geometricOperationTemplates.h"

namespace plb {

/* *************** PART I ******************************************** */
/* *************** Initialization of the block-lattice: atomic-block * */
/* ******************************************************************* */

template<typename T, template<class U> class Descriptor>
void apply(BlockLattice2D<T,Descriptor>& lattice, Box2D domain,
           OneCellFunctional2D<T,Descriptor>* f)
{
    applyProcessingFunctional (
            new GenericLatticeFunctional2D<T,Descriptor>(f), domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void applyIndexed(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, OneCellIndexedFunctional2D<T,Descriptor>* f) {
    applyProcessingFunctional(new GenericIndexedLatticeFunctional2D<T,Descriptor>(f), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, Dynamics<T,Descriptor>* dynamics) {
    applyProcessingFunctional (
        new InstantiateDynamicsFunctional2D<T,Descriptor>(dynamics), domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(BlockLattice2D<T,Descriptor>& lattice, Box2D boundingBox,
                    DomainFunctional2D* domain, Dynamics<T,Descriptor>* dynamics) {
    applyProcessingFunctional (
        new InstantiateComplexDomainDynamicsFunctional2D<T,Descriptor>(dynamics, domain),
        boundingBox, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(BlockLattice2D<T,Descriptor>& lattice,
                    DotList2D const& dotList, Dynamics<T,Descriptor>* dynamics)
{
    applyProcessingFunctional (
        new InstantiateDotDynamicsFunctional2D<T,Descriptor>(dynamics), dotList, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(BlockLattice2D<T,Descriptor>& lattice,
                    plint iX, plint iY, Dynamics<T,Descriptor>* dynamics)
{
    DotList2D pos; pos.addDot(Dot2D(iX,iY));
    defineDynamics(lattice, pos, dynamics);
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( BlockLattice2D<T,Descriptor>& lattice, ScalarField2D<bool>& boolMask,
                     Box2D domain, Dynamics<T,Descriptor>* dynamics, bool whichFlag )
{
    applyProcessingFunctional (
            new DynamicsFromMaskFunctional2D<T,Descriptor>(dynamics, whichFlag),
            domain, lattice, boolMask );
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( BlockLattice2D<T,Descriptor>& lattice, ScalarField2D<bool>& boolMask,
                     Dynamics<T,Descriptor>* dynamics, bool whichFlag )
{
    defineDynamics(lattice, boolMask, lattice.getBoundingBox(), dynamics, whichFlag);
}


template<typename T, template<typename U> class Descriptor>
void defineDynamics( BlockLattice2D<T,Descriptor>& lattice, ScalarField2D<int>& intMask,
                     Box2D domain, Dynamics<T,Descriptor>* dynamics, int whichFlag )
{
    applyProcessingFunctional (
            new DynamicsFromIntMaskFunctional2D<T,Descriptor>(dynamics, whichFlag),
            domain, lattice, intMask );
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( BlockLattice2D<T,Descriptor>& lattice, ScalarField2D<int>& intMask,
                     Dynamics<T,Descriptor>* dynamics, int whichFlag )
{
    defineDynamics(lattice, intMask, lattice.getBoundingBox(), dynamics, whichFlag);
}

template<typename T, template<typename U> class Descriptor>
void recomposeFromFlowVariables ( BlockLattice2D<T,Descriptor>& lattice,
                                  ScalarField2D<T>& density, TensorField2D<T,2>& velocity,
                                  TensorField2D<T,3>& strainRate, Box2D domain )
{
    std::vector<AtomicBlock2D*> atomicBlocks(4);
    atomicBlocks[0] = &lattice;
    atomicBlocks[1] = &density;
    atomicBlocks[2] = &velocity;
    atomicBlocks[3] = &strainRate;
    applyProcessingFunctional( new RecomposeFromFlowVariablesFunctional2D<T,Descriptor>, domain,
                               atomicBlocks );
}

template<typename T, template<typename U> class Descriptor>
void recomposeFromFlowVariables ( BlockLattice2D<T,Descriptor>& lattice,
                                  ScalarField2D<T>& density, TensorField2D<T,2>& velocity,
                                  TensorField2D<T,3>& strainRate )
{
    recomposeFromFlowVariables( lattice, density, velocity, strainRate, lattice.getBoundingBox() );
}

template<typename T, template<class U> class Descriptor>
void setOmega(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, T omega) {
    applyProcessingFunctional(new AssignOmegaFunctional2D<T,Descriptor>(omega), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryVelocity(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, Array<T,2> velocity) {
    applyProcessingFunctional(new SetConstBoundaryVelocityFunctional2D<T,Descriptor>(velocity), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryDensity(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, T rho) {
    applyProcessingFunctional(new SetConstBoundaryDensityFunctional2D<T,Descriptor>(rho), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryTemperature(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, T temperature) {
    applyProcessingFunctional(new SetConstBoundaryTemperatureFunctional2D<T,Descriptor>(temperature), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void initializeAtEquilibrium(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, T rho, Array<T,2> velocity, T temperature) {
    applyProcessingFunctional(new IniConstEquilibriumFunctional2D<T,Descriptor>(rho, velocity, temperature), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void stripeOffDensityOffset(BlockLattice2D<T,Descriptor>& lattice, Box2D domain, T deltaRho) {
    applyProcessingFunctional(new StripeOffDensityOffsetFunctional2D<T,Descriptor>(deltaRho), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setCompositeDynamics( BlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                           CompositeDynamics<T,Descriptor>* compositeDynamics )
{
    applyProcessingFunctional (
            new InstantiateCompositeDynamicsFunctional2D<T,Descriptor>(compositeDynamics),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void setExternalScalar( BlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int whichScalar, T externalScalar )
{
    applyProcessingFunctional (
            new SetExternalScalarFunctional2D<T,Descriptor>(whichScalar, externalScalar),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void setExternalVector( BlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int vectorStartsAt, Array<T,Descriptor<T>::d> externalVector )
{
    applyProcessingFunctional (
            new SetExternalVectorFunctional2D<T,Descriptor>(vectorStartsAt, externalVector),
            domain, lattice );
}


/* *************** PART II ******************************************* */
/* *************** Initialization of the block-lattice: multi-block * */
/* ******************************************************************* */

template<typename T, template<class U> class Descriptor>
void apply(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, OneCellFunctional2D<T,Descriptor>* f) {
    applyProcessingFunctional(new GenericLatticeFunctional2D<T,Descriptor>(f), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void applyIndexed(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, OneCellIndexedFunctional2D<T,Descriptor>* f) {
    applyProcessingFunctional(new GenericIndexedLatticeFunctional2D<T,Descriptor>(f), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, Dynamics<T,Descriptor>* dynamics) {
    applyProcessingFunctional (
        new InstantiateDynamicsFunctional2D<T,Descriptor>(dynamics), domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D boundingBox,
                    DomainFunctional2D* domain, Dynamics<T,Descriptor>* dynamics) {
    applyProcessingFunctional (
        new InstantiateComplexDomainDynamicsFunctional2D<T,Descriptor>(dynamics, domain),
        boundingBox, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(MultiBlockLattice2D<T,Descriptor>& lattice,
                    DotList2D const& dotList, Dynamics<T,Descriptor>* dynamics)
{
    applyProcessingFunctional (
        new InstantiateDotDynamicsFunctional2D<T,Descriptor>(dynamics), dotList, lattice );
}

template<typename T, template<class U> class Descriptor>
void defineDynamics(MultiBlockLattice2D<T,Descriptor>& lattice,
                    plint iX, plint iY, Dynamics<T,Descriptor>* dynamics)
{
    DotList2D pos; pos.addDot(Dot2D(iX,iY));
    defineDynamics(lattice, pos, dynamics);
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( MultiBlockLattice2D<T,Descriptor>& lattice, MultiScalarField2D<bool>& boolMask,
                     Box2D domain, Dynamics<T,Descriptor>* dynamics, bool whichFlag )
{
    applyProcessingFunctional (
            new DynamicsFromMaskFunctional2D<T,Descriptor>(dynamics, whichFlag),
            domain, lattice, boolMask );
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( MultiBlockLattice2D<T,Descriptor>& lattice, MultiScalarField2D<bool>& boolMask,
                     Dynamics<T,Descriptor>* dynamics, bool whichFlag )
{
    defineDynamics(lattice, boolMask, lattice.getBoundingBox(), dynamics, whichFlag);
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( MultiBlockLattice2D<T,Descriptor>& lattice, MultiScalarField2D<int>& intMask,
                     Box2D domain, Dynamics<T,Descriptor>* dynamics, int whichFlag )
{
    applyProcessingFunctional (
            new DynamicsFromIntMaskFunctional2D<T,Descriptor>(dynamics, whichFlag),
            domain, lattice, intMask );
}

template<typename T, template<typename U> class Descriptor>
void defineDynamics( MultiBlockLattice2D<T,Descriptor>& lattice, MultiScalarField2D<int>& intMask,
                     Dynamics<T,Descriptor>* dynamics, int whichFlag )
{
    defineDynamics(lattice, intMask, lattice.getBoundingBox(), dynamics, whichFlag);
}

template<typename T, template<typename U> class Descriptor>
void recomposeFromFlowVariables ( MultiBlockLattice2D<T,Descriptor>& lattice,
                                  MultiScalarField2D<T>& density, MultiTensorField2D<T,2>& velocity,
                                  MultiTensorField2D<T,3>& strainRate, Box2D domain )
{
    std::vector<MultiBlock2D*> multiBlocks(4);
    multiBlocks[0] = &lattice;
    multiBlocks[1] = &density;
    multiBlocks[2] = &velocity;
    multiBlocks[3] = &strainRate;
    applyProcessingFunctional( new RecomposeFromFlowVariablesFunctional2D<T,Descriptor>, domain,
                               multiBlocks );
}

template<typename T, template<typename U> class Descriptor>
void recomposeFromFlowVariables ( MultiBlockLattice2D<T,Descriptor>& lattice,
                                  MultiScalarField2D<T>& density, MultiTensorField2D<T,2>& velocity,
                                  MultiTensorField2D<T,3>& strainRate )
{
    recomposeFromFlowVariables( lattice, density, velocity, strainRate, lattice.getBoundingBox() );
}

template<typename T, template<class U> class Descriptor>
void setOmega(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, T omega) {
    applyProcessingFunctional(new AssignOmegaFunctional2D<T,Descriptor>(omega), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryVelocity(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, Array<T,2> velocity) {
    applyProcessingFunctional(new SetConstBoundaryVelocityFunctional2D<T,Descriptor>(velocity), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryDensity(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, T rho) {
    applyProcessingFunctional(new SetConstBoundaryDensityFunctional2D<T,Descriptor>(rho), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setBoundaryTemperature(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, T temperature) {
    applyProcessingFunctional(new SetConstBoundaryTemperatureFunctional2D<T,Descriptor>(temperature), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void initializeAtEquilibrium(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, T rho, Array<T,2> velocity, T temperature) {
    applyProcessingFunctional(new IniConstEquilibriumFunctional2D<T,Descriptor>(rho, velocity, temperature), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void stripeOffDensityOffset(MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain, T deltaRho) {
    applyProcessingFunctional(new StripeOffDensityOffsetFunctional2D<T,Descriptor>(deltaRho), domain, lattice);
}

template<typename T, template<class U> class Descriptor>
void setCompositeDynamics( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                           CompositeDynamics<T,Descriptor>* compositeDynamics )
{
    applyProcessingFunctional (
            new InstantiateCompositeDynamicsFunctional2D<T,Descriptor>(compositeDynamics),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void setExternalScalar( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int whichScalar, T externalScalar )
{
    applyProcessingFunctional (
            new SetExternalScalarFunctional2D<T,Descriptor>(whichScalar, externalScalar),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor>
void setExternalScalar( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int whichScalar, MultiScalarField2D<T> &field  )
{
    applyProcessingFunctional (
            new SetExternalScalarFromScalarFieldFunctional2D<T,Descriptor>(whichScalar),
            domain, lattice, field );
}

template<typename T, template<class U> class Descriptor>
void setExternalVector( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int vectorStartsAt, Array<T,Descriptor<T>::d> externalVector )
{
    applyProcessingFunctional (
            new SetExternalVectorFunctional2D<T,Descriptor>(vectorStartsAt, externalVector),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor, class ExternalVector>
void setExternalVector( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int vectorStartsAt, ExternalVector f )
{
    applyProcessingFunctional (
            new SetCustomExternalVectorFunctional2D<T,Descriptor,ExternalVector>(vectorStartsAt, f),
            domain, lattice );
}

template<typename T, template<class U> class Descriptor, int nDim>
void setExternalVector( MultiBlockLattice2D<T,Descriptor>& lattice, Box2D domain,
                        int vectorStartsAt, MultiTensorField2D<T,nDim> &tensor )
{
    applyProcessingFunctional (
            new SetExternalVectorFromTensorFieldFunctional2D<T,Descriptor,nDim>(vectorStartsAt),
            domain, lattice, tensor );
}


/* *************** PART III ******************************************* */
/* *************** Initialization of scalar- and tensor-fields: ****** */
/* *************** Atomic-Block  ************************************* */
/* ******************************************************************* */

template<typename T>
void setToConstant(ScalarField2D<T>& field, Box2D domain, T value) {
    applyProcessingFunctional(new IniConstScalarFunctional2D<T>(value), domain, field);
}

template<typename T>
void setToConstant( ScalarField2D<T>& field, ScalarField2D<int>& mask, 
                    int flag, Box2D domain, T value )
{
    applyProcessingFunctional (
            new MaskedIniConstScalarFunctional2D<T>(flag, value), domain, field, mask );
}

template<typename T, int nDim>
void setToConstant( TensorField2D<T,nDim>& field, Box2D domain,
                    Array<T,nDim> const& value )
{
    applyProcessingFunctional (
            new IniConstTensorFunctional2D<T,nDim>(value), domain, field );
}

template<typename T, int nDim>
void setToConstant( TensorField2D<T,nDim>& field, ScalarField2D<int>& mask,
                    int flag, Box2D domain, Array<T,nDim> const& value )
{
    applyProcessingFunctional (
            new MaskedIniConstTensorFunctional2D<T,nDim>(flag, value), domain, mask, field );
}

template<typename T>
void setToCoordinate(ScalarField2D<T>& field, Box2D domain, plint index) {
    applyProcessingFunctional(new SetToCoordinateFunctional2D<T>(index), domain, field);
}

template<typename T>
void setToCoordinates(TensorField2D<T,2>& field, Box2D domain) {
    applyProcessingFunctional(new SetToCoordinatesFunctional2D<T>, domain, field);
}

template<typename T, int nDim>
void assignComponent(TensorField2D<T,nDim>& tensorField, int whichComponent,
                     ScalarField2D<T>& scalarField, Box2D domain)
{
    applyProcessingFunctional(new SetTensorComponentFunctional2D<T,nDim>(whichComponent),
                              domain, scalarField, tensorField);
}

/* *************** PART IV ******************************************* */
/* *************** Initialization of scalar- and tensor-fields: ****** */
/* *************** Multi-Block  ************************************** */
/* ******************************************************************* */

template<typename T>
void setToConstant(MultiScalarField2D<T>& field, Box2D domain, T value) {
    applyProcessingFunctional(new IniConstScalarFunctional2D<T>(value), domain, field);
}

template<typename T>
void setToConstant( MultiScalarField2D<T>& field, MultiScalarField2D<int>& mask,
                    int flag, Box2D domain, T value )
{
    applyProcessingFunctional (
            new MaskedIniConstScalarFunctional2D<T>(flag, value), domain, field, mask);
}

template<typename T, int nDim>
void setToConstant( MultiTensorField2D<T,nDim>& field, Box2D domain,
                    Array<T,nDim> const& value )
{
    applyProcessingFunctional (
            new IniConstTensorFunctional2D<T,nDim>(value), domain, field );
}

template<typename T, int nDim>
void setToConstant( MultiTensorField2D<T,nDim>& field, MultiScalarField2D<int>& mask,
                    int flag, Box2D domain, Array<T,nDim> const& value )
{
    applyProcessingFunctional (
            new MaskedIniConstTensorFunctional2D<T,nDim>(flag, value), domain, mask, field );
}

template<typename T>
void setToCoordinate(MultiScalarField2D<T>& field, Box2D domain, plint index) {
    applyProcessingFunctional(new SetToCoordinateFunctional2D<T>(index), domain, field);
}

template<typename T>
void setToCoordinates(MultiTensorField2D<T,2>& field, Box2D domain) {
    applyProcessingFunctional(new SetToCoordinatesFunctional2D<T>, domain, field);
}

template<typename T, int nDim>
void assignComponent(MultiTensorField2D<T,nDim>& tensorField, int whichComponent,
                     MultiScalarField2D<T>& scalarField, Box2D domain)
{
    applyProcessingFunctional(new SetTensorComponentFunctional2D<T,nDim>(whichComponent),
                              domain, scalarField, tensorField);
}

}  // namespace plb

#endif  // DATA_INITIALIZER_WRAPPER_2D_HH