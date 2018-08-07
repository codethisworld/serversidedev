int main()
{
	long    x;
	long    y;
	long    res;    
	char    c;

	x = 10; 
	y = 123;
	res = __sync_bool_compare_and_swap(&x, x, y);
	if (!res || x != y) { 
		return(1);
	}

	x = 10; 
	y = 123;
	res = __sync_bool_compare_and_swap(&x, x + 1, y);
	if (res || x != 10) {
		return(1);
	}
	x = 10; 
	y = 123;
	res = __sync_add_and_fetch(&x, y);
	if (res != 123 + 10 || x != 123 + 10) { 
		return(1);
	}

	c = 10; 
	res = __sync_lock_test_and_set(&c, 123);
	if (res != 10 || c != 123) {
		return(1);
	}
	return(0);
}
