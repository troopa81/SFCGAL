/**
 *   SFCGAL
 *
 *   Copyright (C) 2012-2013 Oslandia <infos@oslandia.com>
 *   Copyright (C) 2012-2013 IGN (http://www.ign.fr)
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.

 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <SFCGAL/detail/triangulate/ConstraintDelaunayTriangulation.h>

#include <SFCGAL/Exception.h>
#include <SFCGAL/TriangulatedSurface.h>


#include <SFCGAL/detail/triangulate/markDomains.h>

namespace SFCGAL {
namespace triangulate {

///
///
///
ConstraintDelaunayTriangulation::ConstraintDelaunayTriangulation():
    _cdt()
{

}


///
///
///
ConstraintDelaunayTriangulation::Vertex_handle ConstraintDelaunayTriangulation::addVertex( const Coordinate& position )
{
    if ( position.isEmpty() ) {
        BOOST_THROW_EXCEPTION( Exception(
                                   "try to add empty position to ConstraintDelaunayTriangulation"
                               ) );
    }

    Vertex_handle vertex = _projectionPlane
                           ? _cdt.insert( _projectionPlane->to_2d( position.toPoint_3() ) )
                           : _cdt.insert( position.toPoint_2() );
    vertex->info().original = position ;
    return vertex ;
}

///
///
///
void  ConstraintDelaunayTriangulation::addConstraint( Vertex_handle source, Vertex_handle target )
{
    if ( source == target ) {
        return ;
    }

    _cdt.insert_constraint( source, target );
}

///
///
///
void ConstraintDelaunayTriangulation::clear()
{
    _cdt.clear();
}

///
///
///
size_t	ConstraintDelaunayTriangulation::numVertices() const
{
    return _cdt.number_of_vertices() ;
}

///
///
///
size_t	ConstraintDelaunayTriangulation::numTriangles() const
{
    return _cdt.number_of_faces() ;
}

///
///
///
void ConstraintDelaunayTriangulation::setProjectionPlane( const Kernel::Plane_3& projectionPlane )
{
    BOOST_ASSERT( ! projectionPlane.is_degenerate() ) ;
    _projectionPlane = projectionPlane ;
}

///
///
///
Kernel::Plane_3 ConstraintDelaunayTriangulation::projectionPlane() const
{
    if ( _projectionPlane ) {
        return *_projectionPlane ;
    }
    else {
        return Kernel::Plane_3( Kernel::RT( 0 ), Kernel::RT( 0 ), Kernel::RT( 1 ), Kernel::RT( 0 ) );
    }
}


///
/// adapted from CGAL example
///
void ConstraintDelaunayTriangulation::markDomains()
{
    detail::markDomains( _cdt );
}




///
///
///
void ConstraintDelaunayTriangulation::getTriangles( TriangulatedSurface& triangulatedSurface, bool filterExteriorParts ) const
{
    triangulatedSurface.reserve( triangulatedSurface.numTriangles() + numTriangles() );

    for ( Finite_faces_iterator it = finite_faces_begin(); it != finite_faces_end(); ++it ) {
        if ( filterExteriorParts && ( it->info().nestingLevel % 2 == 0 ) ) {
            continue ;
        }

        const Coordinate& a = it->vertex( 0 )->info().original ;

        const Coordinate& b = it->vertex( 1 )->info().original ;

        const Coordinate& c = it->vertex( 2 )->info().original ;

        if ( !a.isEmpty() &&  !b.isEmpty() && !c.isEmpty() ) {
            triangulatedSurface.addTriangle( new Triangle( Point( a ), Point( b ), Point( c ) ) );
        }
    }
}

///
///
///
std::unique_ptr< TriangulatedSurface > ConstraintDelaunayTriangulation::getTriangulatedSurface() const
{
    std::unique_ptr< TriangulatedSurface > result( new TriangulatedSurface );
    getTriangles( *result, false );
    return result ;
}


} // namespace triangulate
} // namespace SFCGAL

