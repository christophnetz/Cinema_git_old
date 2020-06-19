//
// Utilities for OpenGL Mathematics (glm)
//
// Hanno Hildenbrandt 2008
//

/** \defgroup colres Collision response
  \file collision_response.hpp Newtons Law of Restitution.

  "Newtons Law of Restitution for Instantaneous Collisions without Friction"
  is a quite simple model to describe rigid-body collision response.

  Given two bodies with masses \f$m_1\f$ and \f$m_2\f$ and
  (linear) velocities of \f$\mathbf{u_1}\f$ and \f$\mathbf{u_2}\f$ before
  the collision and the "collision normal" \f$\mathbf{n}\f$.
  Without friction, the velocities after the collision are given by:
  \f[  \mathbf{v_1} = \mathbf{u_1} + \frac{j}{m_1} \cdot \mathbf{n} \f]
  \f[  \mathbf{v_2} = \mathbf{u_2} - \frac{j}{m_2} \cdot \mathbf{n} \f]
  where \f$j \cdot \mathbf{n}\f$ is the impulse (of unknown magnitude)
  generated by the collision.

  Now, Newtons law states that the relative velocities before and after
  the collision are connected by a single scalar \f$\epsilon\f$:
  \f[
  ( \mathbf{v_1} - \mathbf{v_2} ) \circ \mathbf{n} =
  - \epsilon \, ( \mathbf{u_1} - \mathbf{u_2} ) \circ \mathbf{n}
  \f]
  The scalar \f$\epsilon\f$ is called the "coefficient of restitution" and
  discribes the energy dissipation during the collision:
  \f$\epsilon = 1\f$ stands for totally elastic collisions (no
  energy is lost) while \f$\epsilon = 0\f$ stands for totally plastic
  collisions (e.g. all energy is wasted in deformation).
  However, with this bit of information we can solve for \f$j\f$:
  \f[
  j = \frac{ -(1 + \epsilon) \cdot (\mathbf{u_1} - \mathbf{u_2}) \circ \mathbf{n} }
         { 1 / m_1 + 1 / m_2 }
  \f]

  \todo Add angular component.
  \ingroup colres
*/


#ifndef glmutils_collision_response_hpp
#define glmutils_collision_response_hpp


#include <limits>
#include <glm/glm.hpp>
#include <glmutils/traits.hpp>


namespace glmutils {

    using namespace glm;


  //! Resolve collision by means of Newtons Law of Restitution.
  //!
  //! \param[in] u1 Velocity of body 1 before collision
  //! \param[in] u2 Velocity of body 2 before collision
  //! \param[in] n Collision normal
  //! \param[in] m1 Mass of body 1
  //! \param[in] m2 Mass of body 2
  //! \param[in] e coefficient of restitution 1 -> totally elastic, 0 -> totally plastic
  //! \param[out] v1 Velocity of body 1 after collision
  //! \param[out] v2 Velocity of body 2 after collision
  //! \return \c false if no collision occurs
  //!
  //! See \ref collision_response.hpp for details.
  //!
  template<typename VEC>
  inline bool resolve_collision(const VEC& u1, const VEC& u2, const VEC& n,
                  typename trait<VEC>::value_type m1, typename trait<VEC>::value_type m2,
                  typename trait<VEC>::value_type e,
                  VEC& v1, VEC& v2)
  {
    typedef typename trait<VEC>::value_type T;
    T vn = dot(u1-u2, n);
    v1 = u1;
    v2 = u2;
    if (vn >= T(0))
    {
      return false;
    }
    double j = ( -(T(1) + e) * vn ) * ( (m1*m2) / (m1+m2) );
    v1 += (j/m1)*n;
    v2 -= (j/m2)*n;
    return true;
  }


  //! Resolve collision by means of Newtons Law of Restitution
  //! assuming \f$m_2 = \infty\f$, \f$m_1\f$ arbitrary.
  //!
  //! \param[in] u1 Velocity of body 1 before collision
  //! \param[in] u2 Velocity of body 2 before collision
  //! \param[in] n Collision normal
  //! \param[in] e coefficient of restitution 1 -> totally elastic, 0 -> totally plastic
  //! \param[out] v1 Velocity of body 1 after collision
  //! \return \a false if no collision occurs
  //!
  //! This function can be used if one object is much much heavier than the
  //! other. Its velocity is then hardly influenced by the collision.
  //!
  //! See \ref collision_response.hpp for details.
  //!
  template<typename VEC>
  inline bool resolve_collision(const VEC& u1, const VEC& u2, const VEC& n,
                  typename trait<VEC>::value_type e,
                  VEC& v1)
  {
    typedef typename trait<VEC>::value_type T;
    T vn = dot(u1-u2, n);
    v1 = u1;
    if (vn >= T(0))
    {
      return false;
    }
    double j = ( -(T(1) + e) * vn );
    v1 += j*n;
    return true;
  }


}   // namespace glmutils


#endif  // glmutils_collision_response_hpp
