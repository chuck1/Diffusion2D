#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>
#include <cassert>

template<typename T> void print(T* t, int n) {
	for(int i = 0; i < n; ++i) {
		std::cout << t[i] << std::endl;
	}
}


template<int P> int prod() {
	return P;
}
template<int P, int A, int... B> int prod() {
	return prod<P * A,B...>();
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

template<typename T, int N> class array {
	public:
		array() {
			n_ = new int[N];
			c_ = new int[N];
		}
		array(std::initializer_list<T> il) {
			/** replace with static assert */
			assert(N == 1);
			
			assert(il.size() == n_[0]);
			
			for(int i = 0; i < n_[0]; ++i) {
				get(i) = il[i];
			}
		}
		template<typename... I> void		alloc(I... i) {
			assert(sizeof...(I) == N);

			int p = prod<1,I...>();

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

		
		template<typename... I> T&				get(I... b) {
			return *(__get<I...>(v_, c_, b...));
		}
		template<typename... I> T*		__get(T* t, int* c, I... b) {
			return t;
		}
		template<typename A, typename... B> T*		__get(T* t, int* c, A a, B... b) {
			t += (*c) * a;
			
			//std::cout << "a = " << a << std::endl;

			return __get(t,c+1,b...);
		}
		
		bool		equal_size(array<T,N> const & rhs) {
			for(int i = 0; i < N; ++i) {
				if(n_[i] != rhs.n_[i]) return false;
			}
			return true;
		}
		
		array<T,N>	operator+(array<T,N> const & rhs) {
		}
		array<T,N>&	operator+=(array<T,N> const & rhs) {
			assert(equal_size(rhs));
			
			for(int i = 0; i < size_; ++i) {
				v_[i] += rhs.v_[i];
			}
		}
		


	public://private:
		int*		c_;
		int*		n_;
		int		size_;
		T*		v_;
};

#endif



