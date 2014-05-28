#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <memory>
#include <cstring>

#include "math.hpp"

template<typename T, int N> class __array;
template<typename T, int N> using array = std::shared_ptr< __array<T,N> >;




template <int D, typename U> struct __multivec {
	typedef std::vector< typename __multivec<D-1,U>::vec_type > vec_type;
};
template <typename U> struct __multivec<1,U> {
	typedef std::vector<U> vec_type;
};

template<int D, typename U> using multivec = typename __multivec<D,U>::vec_type;


template <int D, typename U> struct Initializer_list {
	typedef std::initializer_list<typename Initializer_list<D-1,U>::list_type > list_type;
};
template <typename U> struct Initializer_list<1,U> {
	typedef std::initializer_list<U> list_type;
};



template<typename T, int N> array<T,N>		make_array();
template<typename T, int N> array<T,N>		make_array(std::vector<size_t>, std::initializer_list<T> il);
template<typename T, int N> array<T,N>		make_ones(array<size_t,1> v);
template<typename T, int N> array<T,N>		make_ones(std::vector<size_t> v);
template<typename T, int N> array<T,N>		make_uninit(std::vector<size_t> n);






template <int N, typename T> std::vector<int>	size(typename Initializer_list<N,T>::list_type const & il) {
	std::vector<int> s;

	s.push_back(il.size());
	
	size(s, il[0]);

	return s;
}


template<typename T> void print(T* t, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << std::setw(16) << t[i];
	}
	std::cout << std::endl;
}
template<typename T> void print(std::vector<T> v) {
	for(T i : v) {
		std::cout << std::setw(16) << i;
	}
	std::cout << std::endl;
}


namespace arr {
	template<typename...> int prod(int p) {
		return p;
	}
	template<typename... B> int prod(int p, int a, B... b) {
		return prod(p*a,b...);
	}
}


template<typename...> void fill(int*) {
}
template<int A, int... B> void fill(int* i) {
	*i = A;
	fill<B...>(i+1);
}


template<typename...> void cum(int*, int) {
}
template<int A, int... B> void cum(int* i, int s) {
	*i = s/A;
	cum<B...>(i+1,*i);
}

template<typename T, int N> class __array: public std::enable_shared_from_this< __array<T,N> > {
	public:
		typedef std::shared_ptr< __array<T,N> >			shared;
		typedef typename Initializer_list<N,T>::list_type	init_list;
	public:
		__array() {
		}
		__array(__array<T,N> const & rhs) {
			alloc(rhs.n_);
			memcpy(v_, rhs.v_, size_ * sizeof(T));
		}
	public:
		/** @name Allocation
		 * @{
		 */
		void					alloc(array<int,1> shape_arr) {
			std::vector<size_t> shape;
			for(int i : *shape_arr) shape.push_back(i);
			alloc(shape);
		}
		void					alloc(array<size_t,1> shape_arr) {
			std::vector<size_t> shape;
			for(int i : *shape_arr) shape.push_back(i);
			alloc(shape);
		}
		void					alloc(std::vector<size_t> n) {
			assert(this);
			//std::cout << "this" << this << std::endl;
			
			assert(n.size() == N);

			n_ = n;

			size_ = 1;
			for(int i : n) size_ *= i;

			c_.clear();
			int s = size_;
			for(int i : n_) {
				s /= i;
				c_.push_back(s);
			}

			v_ = new T[size_];

			std::cout << "n    "; print(n_);
			std::cout << "c    "; print(c_);
			std::cout << "size " << size_ << std::endl;
		}
		/** @} */
		void					zeros() {
			for(int i = 0; i < size_; ++i) {
				v_[i] = 0;
			}
		}
		void					ones() {
			for(int i = 0; i < size_; ++i) {
				v_[i] = 1;
			}
		}

		void					set(std::initializer_list<T> src) {
			__set(v_, src);
		}
		void					set(std::vector<int> i, std::initializer_list<T> src) {
			T* dst = __get(v_, i, c_);
			__set(dst, src);
		}
		void					__set(T* dst, std::initializer_list<T> src) {
			for(T s : src) {
				*dst = s;
				dst++;
			}
		}
		
		void					set(std::vector<int> i, T* src, int len) {
			T* dst = __get(v_, i, c_);
			
			memcpy(dst, src, len * sizeof(T));
		}
		
		
		
		shared					transpose_self() {
			assert(0);
			return shared();
		}
		shared					rot90_self(int i) {
			assert(0);
			return shared();
		}
		shared					fliplr_self() { return shared(); }
		std::vector<T>				ravel() {
			std::vector<T> ret;
			for(T* p = v_; p < (v_ + size_); ++p) {
				ret.push_back(*p);
			}
			return ret;
		}
		/** @name access
		 * @{ */
		T&						operator()(int i) {
			i %= size_;
			return v_[i];
		}
		T&						get(int i[N]) {
			std::vector<int> iv(i, i+N-1);
			return *(__get(v_, c_, i, i+N-1));
		}
		T*						__get(T* t, int* c, int* i, int* i_last) {
			t += (*c) * (*i);

			if(i == i_last) return t;

			return __get(t,c+1,i+1,i_last);
		}
		/** @brief get
		 * std::vector
		 */
		T&						get(std::vector<int> const & i) {
			assert(i.size() <= N);
			std::vector<size_t> iu;
			for(size_t a = 0; a < N; ++a) {
				int b = (i[a] + n_[a]) % n_[a];
				assert((b >= 0) && (b < n_[a]));
				iu.push_back((size_t)b);
			}
			return *(__get1(v_, c_.cbegin(), n_.cbegin(), iu.cbegin(), iu));
		}
		/** @brief get
		 * std::vector
		 */
		T&						get(std::vector<size_t> const & i) {
			assert(i.size() <= N);
			for(size_t a = 0; a < N; ++a) {
				assert(i[a] < n_[a]);
			}
			return *(__get1(v_, c_.cbegin(), n_.cbegin(), i.cbegin(), i));
		}
		T*						__get1(
				T* t,
				std::vector<size_t>::const_iterator c,
				std::vector<size_t>::const_iterator n,
				std::vector<size_t>::const_iterator it,
				std::vector<size_t> const & i) {
			if(it == i.cend()) return t;
			
			t += (*c) * (*it);
			
			return __get1(
					t,
					std::next(c),
					std::next(n),
					std::next(it),
					i);
		}
		/** @brief get
		 * variadic input
		 */
		template<typename... I> T&			get(I... b) {
			T* ptr = __get<I...>(v_, c_.begin(), n_.begin(), b...);

			std::cout << "get " << (int)(ptr - v_) << std::endl;

			return *(ptr);
		}
		template<typename... I> T*			__get(T* t, std::vector<size_t>::const_iterator c, std::vector<size_t>::const_iterator n, I... b) {
			return t;
		}
		template<typename A, typename... B> T*		__get(T* t, std::vector<size_t>::const_iterator c, std::vector<size_t>::const_iterator n, A a, B... b) {
			assert(c != c_.end());
			assert(n != n_.end());
			
			a = (a + (*n)) % (*n);
			assert((a >= 0) && (a < (*n)));

			t += (*c) * a;
			
			c++;
			n++;
			
			return __get(t,c,n,b...);
		}
		void						copy(
				T* src,
				T* dst,
				std::vector<size_t> vec_i_src_b,
				std::vector<size_t> vec_i_src_e,
				std::vector<size_t> vec_i_dst_b,
				std::vector<size_t> vec_i_dst_e,
				std::vector<size_t> vec_c_src,
				std::vector<size_t> vec_c_dst) {

			auto it_i_src_b = vec_i_src_b.begin();
			auto it_i_src_e = vec_i_src_e.begin();
			auto it_i_dst_b = vec_i_dst_b.begin();
			auto it_i_dst_e = vec_i_dst_e.begin();
			auto it_c_src   = vec_c_src.begin();
			auto it_c_dst   = vec_c_dst.begin();
			
			int c_src = *it_c_src;
			int c_dst = *it_c_dst;
			
			vec_i_src_b.erase(it_i_src_b);
			vec_i_src_e.erase(it_i_src_e);
			vec_i_dst_b.erase(it_i_dst_b);
			vec_i_dst_e.erase(it_i_dst_e);
			vec_c_src.erase(it_c_src);
			vec_c_dst.erase(it_c_dst);
			
			int i_src = *it_i_src_b;
			int i_dst = *it_i_dst_b;

			for(;
					i_src < *it_i_src_e;
					++i_src
					) {

				copy(
						src + i_src * c_src,
						dst + i_dst * c_dst,
						vec_i_src_b,
						vec_i_src_e,
						vec_i_dst_b,
						vec_i_dst_e,
						vec_c_src,
						vec_c_dst);


			}
		}



		/** @} */
		bool		equal_size(__array<T,N> const & rhs) {
			for(int i = 0; i < N; ++i) {
				if(n_[i] != rhs.n_[i]) return false;
			}
			return true;
		}

		__array<T,N>				operator+(__array<T,N> const & rhs) {
			__array<T,N> ret(*this);
			ret += rhs;
			return ret;
		}
		/** @name self arithmetic
		 * @{ */
		__array<T,N>&				operator+=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] += rhs.v_[i];
			}

			return *this;
		}
		__array<T,N>&				operator*=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] *= rhs.v_[i];
			}

			return *this;
		}
		__array<T,N>&				operator/=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] /= rhs.v_[i];
			}

			return *this;
		}
		/** @} */
		/** @name scalar self arithmatic @{ */
		__array<T,N>&				operator+=(T const & rhs) {
			for(int i = 0; i < size_; ++i) {
				v_[i] += rhs;
			}

			return *this;
		}
		__array<T,N>&				operator-=(T const & rhs) {
			for(int i = 0; i < size_; ++i) {
				v_[i] -= rhs;
			}

			return *this;
		}
		__array<T,N>&				operator*=(T const & rhs) {
			for(int i = 0; i < size_; ++i) {
				v_[i] *= rhs;
			}

			return *this;
		}
		__array<T,N>&				operator/=(T const & rhs) {
			for(int i = 0; i < size_; ++i) {
				v_[i] /= rhs;
			}

			return *this;
		}
		/** @} */
		/** @name shared self arithmetic
		 * @{ */
		shared					add_self(std::shared_ptr< __array<T,N> > const & rhs) {
			this->operator+=(*rhs);
			return __array<T,N>::shared_from_this();
		}
		shared					multiply_self(std::shared_ptr< __array<T,N> > const & rhs) {
			this->operator*=(*rhs);
			return __array<T,N>::shared_from_this();
		}
		shared					divide_self(std::shared_ptr< __array<T,N> > const & rhs) {
			this /= rhs;
			return *this;
		}
		/** @} */
		/** @name shared self scalar arithmetic
		 * @{ */
		shared					add_self(T const & rhs) {
			this->operator+=(rhs);
			return __array<T,N>::shared_from_this();
		}
		shared					multiply_self(T const & rhs) {
			operator*=(rhs);
			return __array<T,N>::shared_from_this();
		}
		shared					divide_self(T const & rhs) {
			operator/=(rhs);
			return __array<T,N>::shared_from_this();
		}
		/** @} */
		/** @name shared @{ */
		shared					add(std::shared_ptr< __array<T,N> > const & rhs) {
			auto ret = std::make_shared< __array<T,N> >(*this);
			(*ret) += (*rhs);
			return ret;
		}
		shared					multiply(std::shared_ptr< __array<T,N> > const & rhs) {
			auto ret = std::make_shared< __array<T,N> >(*this);
			ret *= rhs;
			return ret;
		}
		/** @} */
		/** @name math @{ */
		shared					square() {
			auto ret = std::make_shared< __array<T,N> >(*this);
			ret->square_self();
			return ret;
		}
		void					square_self() {
			for(T* t = v_; t < (v_ + size_); ++t) {
				(*t) *= (*t);
			}
		}
		T					sum() {
			T s = 0;
			for(T* t = v_; t < (v_ + size_); ++t) {
				s += *t;
			}
			return s;
		}
		T					min() {
			T s = 1E37;
			for(T* t = v_; t < (v_ + size_); ++t) {
				if(*t < s) s = *t;
			}
			return s;
		}
		T					max() {
			T s = -1E37;
			for(T* t = v_; t < (v_ + size_); ++t) {
				if(*t > s) s = *t;
			}
			return s;
		}
		/** @name indexing @{ */
		std::vector<int>			offset2index(int o) {
			std::vector<int> ret;
			int a = o;
			for(int c : c_) {
				if(c == 1) {
					ret.push_back(a);
				} else {
					a = a % c;
					ret.push_back((o - a)/c);
				}
			}
			return ret;
		}
		/** @} */
		/** @name lin alg
		 * @{ */
		std::shared_ptr< __array<T,N+1> >	gradient(array<T,N+1> d) {
			auto n = n_;
			n.push_back(N);
			
			auto ret = make_uninit<T,N+1>(n);
			
			std::cout << "sorry, not yet implemented" << std::endl;
			assert(0);

			//for(int i : range(size_)) {
			//}

			return ret;
		}
		/** @} */
		/** @name iterating
		 * @{ */
		T*					begin() {
			return v_;
		}
		T*					end() {
			return v_ + size_;
		}

		shared					sub(std::vector<int> beg_s, std::vector<int> end_s) {
			assert(beg_s.size() == end_s.size());
			
			std::vector<size_t> beg, end;
			
			// resolve negative indices
			for(size_t i = 0; i < end.size(); ++i) {
				beg.push_back((size_t)((beg_s[i] + n_[i]) % n_[i]));
				end.push_back((size_t)((end_s[i] + n_[i]) % n_[i]));
			}
			// calculate shape
			std::vector<size_t> shape = end;
			for(int i = 0; i < beg.size(); ++i) {
				shape[i] -= beg[i];
			}

			// alloc new array
			auto ret = std::make_shared< __array<T,N> >();
			ret->alloc(shape);

			// fill
			copy(
					v_,
					ret->v_,
					beg,
					end,
					std::vector<size_t>(N,0),
					n_,
					c_,
					ret->c_);
			return ret;
		}
		/** @} */
		/** @name info
		 * @{ */
		size_t					size() {
			return size_;
		}
		std::vector<size_t>			shape() {
			return n_;
		}
		/** @} */
	public://private:
		std::vector<size_t>		c_;
		std::vector<size_t>		n_;
		size_t				size_;
		T*				v_;
};




/*template<typename T, int N> array<T,N>		make_array() {
  return std::make_shared< __array<T,N> >();
  }*/

template<typename T, int N> array<T,N>		make_array_1(std::initializer_list<T> il) {
	std::vector<size_t> n({il.size()});
	return make_array<T,N>(n, il);
}
template<typename T, int N> array<T,N>		make_array(std::vector<size_t> n, std::initializer_list<T> il) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	arr->set(il);
	return arr;
}

template<typename T, int N> array<T,N>		make_ones_arr(array<size_t,1> v) {
	auto arr = std::make_shared< __array<T,N> >();
	std::vector<size_t> a(std::begin(*v), std::end(*v));
	arr->alloc(a);
	arr->ones();
	return arr;
}
template<typename T, int N> array<T,N>		make_ones(std::vector<size_t> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	arr->ones();
	return arr;
}

template<typename T, int N> array<T,N>		make_uninit(std::vector<size_t> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	return arr;
}

template<typename T, int N> array<T,N>		make_zeros(std::vector<size_t> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	arr->zeros();
	return arr;
}

#define ARR_LOOP_START(arr) for(int p0 = 0; p0 < (arr->c_[0] * arr->n_[0]); p0 += arr->c_[0])

#define ARR_LOOP(arr, p, level) for(int p##level = p; p##level < (p + (arr->c_[level] * arr->n_[level])); p##level += arr->c_[level])


template<typename T> array<T,1>		linspace(T s, T e, size_t n) {

	auto ret = make_uninit<T,1>({n});

	T step = (e - s) / ((T)(n - 1));

	for(int i : range(n)) {
		ret->get(i) = s + step * i;
	}

	return ret;
}


template<typename T> std::pair< array<T,2>, array<T,2> >		meshgrid(array<T,1> x, array<T,1> y) {

	auto nx = x->n_[0];
	auto ny = y->n_[0];

	auto X = make_uninit<T,2>({nx,ny});

	auto Y = make_uninit<T,2>({nx,ny});

	for(int i : range(nx)) {
		for(int j : range(ny)) {
			X->get(i,j) = x->get(i);
			Y->get(i,j) = y->get(j);
		}
	}

	return std::make_pair(X,Y);
}

#endif




