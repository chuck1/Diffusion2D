#ifndef INDEX_LAMBDA_HPP
#define INDEX_LAMBDA_HPP


struct Index_Lambda {
	
	int	operator()(int i, int p) {
		return (a[i] * p + b[i]);
	}

	int	a[2];
	int	b[2];
	
	real	d;
};

#endif


