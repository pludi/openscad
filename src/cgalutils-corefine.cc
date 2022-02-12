// Portions of this file are Copyright 2021 Google LLC, and licensed under GPL2+. See COPYING.
#include "cgalutils.h"

#include "cgalutils-corefinement-visitor.h"

namespace CGALUtils {

#if FAST_CSG_KERNEL_IS_LAZY

/*! Visitor that forces exact numbers for the vertices of all the faces created during corefinement.
 */
template <typename TriangleMesh>
struct ExactLazyNumbersVisitor
  : public PMP::Corefinement::Default_visitor<TriangleMesh> {
  typedef typename TriangleMesh::Face_index face_descriptor;

#if CGAL_VERSION_NR >= CGAL_VERSION_NUMBER(5, 4, 0)
  void new_vertex_added(std::size_t i_id, vertex_descriptor v, const TriangleMesh& tm) {
    auto& pt = tm.point(v);
    CGAL::exact(pt.x());
    CGAL::exact(pt.y());
    CGAL::exact(pt.z());
  }
#else
  face_descriptor split_face;
  std::vector<face_descriptor> created_faces;

  void before_subface_creations(face_descriptor f_split, TriangleMesh& tm)
  {
    split_face = f_split;
    created_faces.clear();
  }

  void after_subface_creations(TriangleMesh& mesh)
  {
    for (auto& fi : created_faces) {
      CGAL::Vertex_around_face_iterator<TriangleMesh> vbegin, vend;
      for (boost::tie(vbegin, vend) = vertices_around_face(mesh.halfedge(fi), mesh); vbegin != vend;
           ++vbegin) {
        auto& v = mesh.point(*vbegin);
        CGAL::exact(v.x());
        CGAL::exact(v.y());
        CGAL::exact(v.z());
      }
    }
    created_faces.clear();
  }
  void after_subface_created(face_descriptor fi, TriangleMesh& tm) { created_faces.push_back(fi); }
#endif // if CGAL_VERSION_NR >= CGAL_VERSION_NUMBER(5, 4, 0)
};

#endif// FAST_CSG_KERNEL_IS_LAZY

template <class TriangleMesh>
bool corefineAndComputeUnion(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
  auto remesh = Feature::ExperimentalFastCsgRemesh.is_enabled();
  auto exactCallback = Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled();
  if (exactCallback && !remesh) {
    auto param = PMP::parameters::visitor(ExactLazyNumbersVisitor<TriangleMesh>());
    return PMP::corefine_and_compute_union(lhs, rhs, out, param, param);
  } else if (remesh) {
    CorefinementVisitor<TriangleMesh> visitor(lhs, rhs, out, exactCallback);
    auto param = PMP::parameters::visitor(visitor);
    auto result = PMP::corefine_and_compute_union(lhs, rhs, out, param, param, param);
    visitor.remeshSplitFaces(out);
    return result;
  } else {
    return PMP::corefine_and_compute_union(lhs, rhs, out);
  }
}

template <class TriangleMesh>
bool corefineAndComputeIntersection(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
  auto remesh = Feature::ExperimentalFastCsgRemesh.is_enabled();
  auto exactCallback = Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled();
  if (exactCallback && !remesh) {
    auto param = PMP::parameters::visitor(ExactLazyNumbersVisitor<TriangleMesh>());
    return PMP::corefine_and_compute_intersection(lhs, rhs, out, param, param);
  } else if (remesh) {
    CorefinementVisitor<TriangleMesh> visitor(lhs, rhs, out, exactCallback);
    auto param = PMP::parameters::visitor(visitor);
    auto result = PMP::corefine_and_compute_intersection(lhs, rhs, out, param, param, param);
    visitor.remeshSplitFaces(out);
    return result;
  } else {
    return PMP::corefine_and_compute_intersection(lhs, rhs, out);
  }
}

template <class TriangleMesh>
bool corefineAndComputeDifference(TriangleMesh& lhs, TriangleMesh& rhs, TriangleMesh& out)
{
  auto remesh = Feature::ExperimentalFastCsgRemesh.is_enabled();
  auto exactCallback = Feature::ExperimentalFastCsgExactCorefinementCallback.is_enabled();
  if (exactCallback && !remesh) {
    auto param = PMP::parameters::visitor(ExactLazyNumbersVisitor<TriangleMesh>());
    return PMP::corefine_and_compute_difference(lhs, rhs, out, param, param);
  } else if (remesh) {
    CorefinementVisitor<TriangleMesh> visitor(lhs, rhs, out, exactCallback);
    auto param = PMP::parameters::visitor(visitor);
    auto result = PMP::corefine_and_compute_difference(lhs, rhs, out, param, param, param);
    visitor.remeshSplitFaces(out);
    return result;
  } else {
    return PMP::corefine_and_compute_difference(lhs, rhs, out);
  }
}

template bool corefineAndComputeUnion(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);
template bool corefineAndComputeIntersection(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);
template bool corefineAndComputeDifference(
  CGAL_HybridMesh& lhs, CGAL_HybridMesh& rhs, CGAL_HybridMesh& out);

} // namespace CGALUtils

