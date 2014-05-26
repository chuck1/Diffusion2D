#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

template <int D, typename U> struct Initializer_list {
	typedef std::initializer_list<typename Initializer_list<D-1,U>::list_type > list_type;
};
template <typename U> struct Initializer_list<1,U> {
	typedef std::initializer_list<U> list_type;
};



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

template<typename T, int N> class __array {
	public:
	public:
		__array() {
			n_ = new int[N];
			c_ = new int[N];
		}
		__array(typename Initializer_list<N,T>::list_type il): __array() {

			assert(il.size() == n_[0]);

			for(int i = 0; i < n_[0]; ++i) {
				get(i) = il[i];
			}
		}


		template<typename... I> void		alloc(I... i) {
			assert(sizeof...(I) == N);

			int p = arr::prod(1,i...);

			v_ = new T[p];

			fill<I...>(n_);
			cum<I...>(c_, p);

			print(n_, N);
			print(c_, N);
		}
		template<typename... I> void		zeros(I... i) {
			alloc(i...);

			for(int i = 0; i < size_; ++i) {
				v_[i] = 0;
			}
		}
		template<typename... I> void		ones(I... i) {
			alloc(i...);

			for(int i = 0; i < size_; ++i) {
				v_[i] = 1;
			}
		}

		T&						get(int i[N]) {
			return *(__get(v_, c_, i, i+N-1));
		}
		T*						__get(T* t, int* c, int* i, int* i_last) {
			t += (*c) * (*i);

			if(i == i_last) return t;

			return __get(t,c+1,i+1,i_last);
		}
		template<typename... I> T&			get(I... b) {
			return *(__get<I...>(v_, c_, b...));
		}
		template<typename... I> T*			__get(T* t, int* c, I... b) {
			return t;
		}
		template<typename A, typename... B> T*		__get(T* t, int* c, A a, B... b) {
			t += (*c) * a;

			//std::cout << "a = " << a << std::endl;

			return __get(t,c+1,b...);
		}

		bool		equal_size(__array<T,N> const & rhs) {
			for(int i = 0; i < N; ++i) {
				if(n_[i] != rhs.n_[i]) return false;
			}
			return true;
		}

		__array<T,N>	operator+(__array<T,N> const & rhs) {
			__array<T,N> ret(*this);
			ret += rhs;
			return ret;
		}
		__array<T,N>&				operator+=(__array<T,N> const & rhs) {
			assert(equal_size(rhs));

			for(int i = 0; i < size_; ++i) {
				v_[i] += rhs.v_[i];
			}
		}
		// shared
		std::shared_ptr< __array<T,N> >		add(std::shared_ptr< __array<T,N> > const & rhs) {
			auto ret = std::make_shared< __array<T,N> >(*this);
			ret += rhs;
			return ret;
		}




	public://private:
		int*		c_;
		int*		n_;
		int		size_;
		T*		v_;
};

template<typename T, int N> class array{
	public:
		array();
		array(std::shared_ptr<__array<T,N>>);
		__array<T,N>* operator->();
		operator std::shared_ptr<__array<T,N>>();
};



//template<typename T, int N> using array<T,N> = std::shared_ptr< __array<T,N> >;

template<typename T, int N> array<T,N>		make_array() {
	return std::make_shared< __array<T,N> >();
}
template<typename T, int N> array<T,N>		make_array(typename Initializer_list<N,T>::list_type il) {
	return std::make_shared< __array<T,N> >(il);
}
template<typename T, int N> array<T,N>		make_ones(std::vector<int> v) {
	auto arr = std::make_shared< __array<T,N> >();
	arr->ones(v);
}


#define ARR_LOOP_START(arr) for(auto p0 = arr->v_; p0 < (arr->v_ + (arr->c_[0] * arr->n_[0])); p0 += arr->c_[0])

#define ARR_LOOP(arr, ptr, level) for(auto p##level = ptr; p##level < (ptr + (arr->c_[level] * arr->n_[level])); p##level += arr->c_[level])



#endif



