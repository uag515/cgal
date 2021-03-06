namespace CGAL {

/*!
\ingroup PkgMesh_3MeshClasses

The class `Mesh_facet_criteria_3` is a model of `MeshFacetCriteria_3`. 
It provides a uniform bound for the shape criterion, 
a uniform or variable sizing field 
for the size criterion and/or 
a uniform or variable distance field 
for the approximation error criterion. 

\tparam Tr must be identical to the nested type 
`Triangulation` of the instance used as model of 
`MeshComplex_3InTriangulation_3`. 

\cgalModels `MeshFacetCriteria_3`` 

\sa `CGAL::Mesh_facet_topology` 
\sa `MeshCriteria_3` 
\sa `MeshFacetCriteria_3` 
\sa `CGAL::Mesh_criteria_3<Tr>` 
\sa `MeshDomainField_3` 
\sa `CGAL::make_mesh_3()`

*/
template< typename Tr >
class Mesh_facet_criteria_3 {
public:

/// \name Types 
/// @{

/*!
Numerical type 
*/ 
typedef Tr::Geom_traits::FT FT; 

/// @} 

/// \name Creation 
/// @{

/*!
Returns an object to serve as criteria for facets. The argument 
`angle_bound` is the lower bound for the angle in degrees of the 
surface mesh facets. The argument `radius_bound` is a uniform upper bound 
for the radius of the surface Delaunay balls. The argument 
`distance_bound` is an upper bound for the center-center distances 
of the surface mesh facets. `topology` is the set of topological constraints 
which have to be verified by each surface facet. See 
section \ref introsecparam for further details. 
Note that if one parameter is set to 0, then its corresponding criteria is ignored. 
*/ 
Mesh_facet_criteria_3( 
FT angle_bound, 
FT radius_bound, 
FT distance_bound, 
Mesh_facet_topology topology = FACET_VERTICES_ON_SURFACE); 

/*!
Returns an object to serve as criteria for facets. The types `SizingField` and 
`DistanceField` must 
be models of the concept `MeshDomainField_3`. The behavior and semantic of the arguments are the same 
as above, except that the radius and distance bound parameters are functionals instead of constants. 
*/ 
template <class SizingField, class DistanceField> 
Mesh_facet_criteria_3( 
FT angle_bound, 
SizingField radius_bound, 
DistanceField distance_bound, 
Mesh_facet_topology topology = FACET_VERTICES_ON_SURFACE); 

/// @}

}; /* end Mesh_facet_criteria_3 */
} /* end namespace CGAL */
