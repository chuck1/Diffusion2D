#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

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
template<typename T, int N> array<T,N>		make_array(typename Initializer_list<N,T>::list_type il);
template<typename T, int N> array<T,N>		make_ones(array<int,1> v);
template<typename T, int N> array<T,N>		make_ones(std::vector<int> v);






template <int N, typename T> std::vector<int>	size(typename Initializer_list<N,T>::list_type const & il) {
	std::vector<int> s;

	s.push_back(il.size());
	
	size(s, il[0]);

	return s;
}


template<typename T> void print(T* t, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << t[i] << std::endl;
	}
}
template<typename T> void print(std::vector<T> v) {
	for(T i : v) {
		std::cout << i << std::endl;
	}
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
		/*__array(init_list il) {
			//alloc()
			set(il, v_, c_);
		}*/
		__array(__array<T,N> const & rhs) {

		}
	public:
		/*template<typename U> void		set(std::initializer_list<U> il, T* ptr, std::vector<int> C) {
			auto beg = C.begin();
			int c = *beg;
				
			C.erase(beg);
			
			for(auto a : il) {
				set(a, ptr, C);
				ptr += c;
			}
		}
		void					set(std::initializer_list<T> il, T* ptr, std::vector<int> C) {
			for(auto a : il) {
				*ptr = a;
				ptr++;
			}
		}*/


		void					alloc(std::vector<int> n) {
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

			print(n_);
			print(c_);
		}
		template<typename... I> void		zeros() {
			for(int i = 0; i < size_; ++i) {
				v_[i] = 0;
			}
		}
		template<typename... I> void		ones() {
			for(int i = 0; i < size_; ++i) {
				v_[i] = 1;
			}
		}
		/*template<typename... I> void		ones(array<int,1> n) {
			assert(n->size_ == N);

			std::vector<int> v;

			for(int* i = n->v_; i < (n->v_ + n->size_); ++i) {
				v.push_back(*i);
			}
		}*/


		/** @name access @{ */
		T&						operator()(int i) {
			i %= size_;
			return v_[i];
		}
		T&						get(int i[N]) {
			return *(__get(v_, c_, i, i+N-1));
		}
		T*						__get(T* t, int* c, int* i, int* i_last) {
			t += (*c) * (*i);

			if(i == i_last) return t;

			return __get(t,c+1,i+1,i_last);
		}
		
		T&						get(std::vector<int> i) {
			return *(__get1(v_, c_, i));
		}
		T*						__get1(T* t, std::vector<int> c, std::vector<int> i) {
			auto itc = c.begin();
			auto iti = i.begin();
			
			t += (*itc) * (*iti);
			
			c.erase(itc);
			i.erase(iti);
			
			if(i.empty()) return t;
			
			return __get1(t,c,i);
		}

		
		template<typename... I> T&			get(I... b) {
			return *(__get<I...>(v_, c_, b...));
		}
		template<typename... I> T*			__get(T* t, std::vector<int> c, I... b) {
			return t;
		}
		template<typename A, typename... B> T*		__get(T* t, std::vector<int> c, A a, B... b) {
			t += c[0] * a;

			c.erase(c.begin());
			//std::cout << "a = " << a << std::endl;

			return __get(t,c,b...);
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
		// self
		__array<T,N>&				operator+=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] += rhs.v_[i];
			}

			*this;
		}
		__array<T,N>&				operator*=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] *= rhs.v_[i];
			}

			*this;
		}
		__array<T,N>&				operator*=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] *= rhs.v_[i];
			}

			*this;
		}
		/** @name scalar self arithmatic @{ */
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
		// shared self
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
		shared					multiply_self(T const & rhs) {
			operator*=(rhs);
			return __array<T,N>::shared_from_this();
		}
		// shared
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
		// math
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
		// indexing
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

		// lin alg
		shared					gradient(array<T,N+1> d) {
			auto ret = make_array<T,N+1>();

			std::vector<int> n = n_;
			n.push_back(N);

			ret->alloc(n);

			for(int i : range(size_)) {
			}
		}

		// iterating
		T*					begin() {
			return v_;
		}
		T*					end() {
			return v_ + size_;
		}

		shared					sub(std::vector<int> beg, std::vector<int> end) {

			for(int i = 0; i < end.size(); ++i) {
				if(end[i] < 0) {
					end[i] = n_[i] + end[i];
				}
				if(beg[i] < 0) {
					beg[i] = n_[i] + beg[i];
				}
			}
			auto size = end;
			for(int i = 0; i < beg.size(); ++i) {
				size[i] -= beg[i];
			}
		}

		size_t					size() {
			return size_;
		}
	public://private:
		std::vector<int>		c_;
		std::vector<int>		n_;
		size_t				size_;
		T*				v_;
};




/*template<typename T, int N> array<T,N>		make_array() {
  return std::make_shared< __array<T,N> >();
  }*/
/*template<typename T, int N> array<T,N>		make_array(typename Initializer_list<N,T>::list_type il) {
	return std::make_shared< __array<T,N> >(il);
}*/

template<typename T, int N> array<T,N>		make_ones(array<int,1> v) {
	auto arr = std::make_shared< __array<T,N> >();

	std::vector<int> a(std::begin(*v), std::end(*v));

	arr->ones(v);
}
template<typename T, int N> array<T,N>		make_uninit(std::vector<int> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	return arr;
}
template<typename T, int N> array<T,N>		make_ones(std::vector<int> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	arr->ones();
	return arr;
}
template<typename T, int N> array<T,N>		make_zeros(std::vector<int> n) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->alloc(n);
	arr->zeros();
	return arr;
}

#define ARR_LOOP_START(arr) for(int p0 = 0; p0 < (arr->c_[0] * arr->n_[0]); p0 += arr->c_[0])

#define ARR_LOOP(arr, p, level) for(int p##level = p; p##level < (p + (arr->c_[level] * arr->n_[level])); p##level += arr->c_[level])



#endif



