#ifndef KINE_UTIL_HPP__
#define KINE_UTIL_HPP__
#include "eigen3/Eigen/Eigen"
#include <cmath>
namespace rpp
{
    namespace kine
    {
       template <typename T> 
       Eigen::Matrix<T, 4, 4> dh2t(const T alpha, const T A, 
               const T theta, const T D, const bool modified_dh = false)
       {
           Eigen::Matrix<T, 4, 4> M;
           const T sa = sin(alpha);
           const T ca = cos(alpha);
           const T st = sin(theta);
           const T ct = cos(theta);
           if (!modified_dh)
           {
               M << ct, -st*ca, st*sa, A*ct,
                    st,  ct*ca, -ct*sa, A*st,
                    0,   sa,     ca,    D,
                    0,   0,      0,     1;
           }
           else
           {
               M << ct,    -st,    0,    A,
                    st*ca, ct*ca,  -sa   -D*sa,
                    st*sa, ct*sa,  ca,   D*ca,
                    0,     0,      0,    1;
           }
           return M;
       }
    }
}
#endif
