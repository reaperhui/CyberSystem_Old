#ifndef ANGULAR_INTERVAL_HPP__
#define ANGULAR_INTERVAL_HPP__

#include <ostream>
#include <cmath>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
namespace rpp
{
    namespace util
    {
        template <typename T> struct Constants {};
        template <> struct Constants<double>
        {
            const static double eps;
            const static double pi;
        };
        const double Constants<double>::eps = pow(2.0, -30.0);
        const double Constants<double>::pi = 3.14159265346825122833251953125;
        template <> struct Constants<float>
        {
            const static float eps;
            const static float pi;
        };
        const float Constants<float>::eps = pow(2.0, -15.0);
        const float Constants<float>::pi = 3.141571044921875;

        template <typename T> class AngularIntervalSet;
        template <typename T>
        class AngularInterval
        {
        public:
            // template <typename U> class AngularIntervalSet;
            friend class AngularIntervalSet<T>;
            typedef T value_type;
            AngularInterval(const value_type lower, const value_type upper)
            {
                const static value_type PI = Constants<value_type>::pi;
                const static value_type EPS = Constants<value_type>::eps;
                if (std::fabs(upper - lower) >= 2*PI)
                {
                    _lower = -PI;
                    _upper = PI;
                }
                else
                {
                    _lower = lower;
                    _upper = (lower > upper ? upper + 2*PI : upper);
                    const int N = std::floor(_upper/(2*PI));
                    value_type u = _upper - 2*N*PI;
                    value_type l = _lower - 2*N*PI;
                    if (u > PI)
                    {
                        u -= 2*PI;
                        l -= 2*PI;
                    }
                    if (l <= -PI) l += 2*PI;
                    _lower = l - fmod(l, 2*EPS);
                    _upper = u - fmod(u, 2*EPS);
                }
            }
            value_type lower(const bool orig = false) const
            {
                const static value_type PI = Constants<value_type>::pi;
                return orig ? _lower : (_lower < _upper ? _lower : _lower - 2*PI);
            }
            value_type upper() const
            {
                return _upper;
            }
            bool contains(const value_type val) const
            {
                const static value_type PI = Constants<value_type>::pi;
                const int N = std::floor(val/(2*PI));
                value_type v = val - 2*N*PI;
                if (v > PI) v -= 2*PI;
                return (_lower <= _upper && _lower <= v && v <= _upper)
                    || (_lower > _upper && (v <= _lower || v >= _upper));
            }
            const AngularInterval operator +(const AngularInterval other)
            {
                return AngularInterval(lower() + other.lower(), 
                    upper() + other.upper());
            }
            const AngularInterval offset(const value_type val)
            {
                return AngularInterval(lower() + val, upper() + val);
            }
        private:
            value_type _lower;
            value_type _upper;
        };
        template <typename T>
        std::ostream &operator << (std::ostream &os, const AngularInterval<T> &I)
        {
            os << "[" << I.lower() << ", " << I.upper() << "]";
            return os;
        }

        template <typename T>
        class AngularIntervalSet
        {
        public:
            typedef T value_type;
            typedef std::vector<AngularInterval<value_type> > vector_type;
            typedef typename vector_type::iterator iterator;
            typedef typename vector_type::const_iterator const_iterator;
        public:
            AngularIntervalSet() {}
            AngularIntervalSet(const AngularInterval<value_type> &elem)
            {
                 _intervals.push_back(elem);
            }
            AngularIntervalSet(const AngularIntervalSet &other)
                : _intervals(other._intervals) {}
            AngularIntervalSet operator +(const AngularIntervalSet &set)
            {
                AngularIntervalSet s(*this);
                s._intervals.insert(s._intervals.end(), set._intervals.begin(), 
                        set._intervals.end());
                s.simplify();
                return s;
            }
            AngularIntervalSet operator +=(const AngularIntervalSet &set)
            {
                *this = *this + set;
                return *this;
            }
            AngularIntervalSet operator *(const AngularIntervalSet &set)
            {
                AngularIntervalSet s;
                if (this->empty() || set.empty())
                    return s;
                return merge(*this, set, std::logical_and<bool>());
            }
            AngularIntervalSet operator *=(const AngularIntervalSet &set)
            {
                *this = (*this) * set;
                return *this;
            }
            AngularIntervalSet operator -(const AngularIntervalSet &set)
            {
                if (this->empty()) return empty_set();
                if (set->empty()) return *this;
                struct {
                    bool operator() (const bool op1, const bool op2)
                    {
                        return op1 && !op2;
                    }
                } op;
                return merge(*this, set, op);
            }
            AngularIntervalSet operator -=(const AngularIntervalSet &set)
            {
                *this = *this - set;
                return *this;
            }
            bool empty() const
            {
                return _intervals.empty();
            }
            std::size_t size() const
            {
                return _intervals.size();
            }
            bool contains(const value_type val) const
            {
                for (auto it = _intervals.begin(); it != _intervals.end(); ++it)
                    if (it->contains(val)) return true;
                return false;
            }
            static const AngularIntervalSet<T> universal_set()
            {
                const static value_type PI = Constants<value_type>::pi;
                return AngularIntervalSet<T>(AngularInterval<T>(-PI, PI));
            }
            static const AngularIntervalSet<T> empty_set()
            {
                return AngularIntervalSet<T>();
            }
        public:
            // iterator
            iterator begin()
            {
                return _intervals.begin();
            }
            iterator end()
            {
                return _intervals.end();
            }
            const_iterator begin() const
            {
                return _intervals.begin();
            }
            const_iterator end() const
            {
                return _intervals.end();
            }
        private:
            static std::vector<std::pair<value_type, bool> > convert(const AngularIntervalSet<value_type> set)
            {

                typedef std::vector<std::pair<value_type, bool> > result_type;
                result_type res;
                std::vector<std::pair<value_type, int> > points;
                int n = 0;
                for (auto it = set._intervals.begin(); it != set._intervals.end(); ++it)
                {
                    points.push_back(std::make_pair(it->_lower, -1));
                    points.push_back(std::make_pair(it->_upper, 1));
                    if (it->_lower > it->_upper) n += 1;
                }
                std::sort(points.begin(), points.end()); 
                for (auto it = points.begin(); it != points.end(); ++it)
                {
                    res.push_back(std::make_pair(it->first, n > 0));
                    n = n - it->second;
                }
                // for (auto it = res.begin(); it != res.end(); ++it)
                // {
                //     std::cout << it->first << ", " << it->second << std::endl;
                // }
                return res;
            }
            static const AngularIntervalSet<value_type> restore(std::vector<std::pair<value_type, bool> > pts)
            {
                const static value_type PI = Constants<value_type>::pi;
                AngularIntervalSet res;
                std::size_t num_pos = 0, num_neg = 0;
                for (auto it = pts.begin(); it != pts.end(); ++it)
                {
                    if (it->second) num_pos++;
                    else num_neg++;
                }
                if (num_neg == pts.size())
                    return AngularIntervalSet<T>();
                if (num_pos == pts.size())
                    return AngularIntervalSet<T>(AngularInterval<T>(-PI, PI));
                value_type lower;
                for (std::size_t i = 0; i < pts.size(); ++i)
                {
                    const auto cur = pts[i];
                    const auto next = pts[(i + 1) % pts.size()];
                    if (!cur.second && next.second)
                        lower = cur.first;
                }
                for (std::size_t i = 0; i < pts.size(); ++i)
                {
                    const auto cur = pts[i];
                    const auto next = pts[(i + 1) % pts.size()];
                    if (!cur.second && next.second)
                        lower = cur.first;
                    if (cur.second && !next.second)
                        res._intervals.push_back(AngularInterval<T>(lower, cur.first));
                }
                return res;
            }

            template <typename OP>
            AngularIntervalSet<value_type> merge(const AngularIntervalSet<value_type> set1, 
                    const AngularIntervalSet<value_type> set2, OP op)
            {
                std::vector<std::pair<value_type, bool>> res;
                auto pts1 = convert(set1);
                auto pts2 = convert(set2);
                if (pts1.size()*pts2.size() == 0)
                {
                    res.insert(res.end(), pts1.begin(), pts1.end());
                    res.insert(res.end(), pts2.begin(), pts2.end());
                }
                else
                {
                    std::size_t i = 0;
                    std::size_t j = 0;
                    for (std::size_t k = 0; k < pts1.size() + pts2.size(); ++k)
                    {
                        const bool flag = op(pts1[i%pts1.size()].second, pts2[j%pts2.size()].second);
                        if (i >= pts1.size())
                            res.push_back(std::make_pair(pts2[j++].first, flag));
                        else if (j >= pts2.size())
                            res.push_back(std::make_pair(pts1[i++].first, flag));
                        else if (pts1[i].first > pts2[j].first)
                            res.push_back(std::make_pair(pts2[j++].first, flag));
                        else
                            res.push_back(std::make_pair(pts1[i++].first, flag));
                    }
                }
                return restore(res);
            }
            void simplify()
            {
                if (!this->empty())
                    *this = restore(convert(*this));
            }
            void clear()
            {
                _intervals.clear();
            }
            vector_type _intervals; 
        };
        template<typename T>
        std::ostream &operator << (std::ostream &os, const AngularIntervalSet<T> &s)
        {
            os << "{";
            for (auto it = s.begin(); it != s.end(); ++it)
                os << *it << (it == s.end() - 1 ? "" : ", ");
            os << "}";
            return os;
        }
    }
}
#endif // ANGULAR_INTERVAL_HPP__
