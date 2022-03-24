#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int	valid_solutions = 0;
int no_zero_solutions = 0;

int non_zero_solutions = 0;
int commutative_solutions = 0;

typedef enum {ERR, ADD, SUB, MUL, DIV, EQ} op_t;

int		idx = 0;
char	c_ops[10];
op_t	ops[10];
int		nums[10];	// -1 will indicate an invalid number 

void	dump(const char *s = NULL)
{
	int	i;
	if (s) printf("%s :: ", s);
	for (i = 0; c_ops[i] != '='; i++)
	{
		printf(" %d %c ", nums[i], c_ops[i]);
	}
	printf("%d = %d \n", nums[i], nums[i+1]);
}

void	clear()
{
	idx = 0;
	c_ops[0] = 0;
	ops[0] = ERR;
	memset(nums, 0, 10 * sizeof(int));
}

void	out_exp(int lno, int *n, char *c, int ix)
{
	printf("Line: %4d:: ", lno);
	for (int i = 0; i < ix; i++)
	{
		printf("%d %c ", n[i], c[i]);
	}
	printf("%d\n", n[idx]);
}

bool	is_first_digit(char d)
{
	if ( (d >= '1') && (d <= '9') ) return true;
	return false;
}

int dig_value(char c)
{
	return c - '0';
}

op_t get_op(char p)
{
	switch(p)
	{
		case '+' : return ADD;
		case '-' : return SUB;
		case '*' : return MUL;
		case '/' : return DIV;
		case '=' : return EQ;
	}
	return ERR;
}

bool end_of_line(char c)
{
	return !isprint(c);
}

void	move(int *n, char *co, int index)
{
	//printf("MOVE: n[0] = %d   co[0] = %c   index = %d   num_nums = %d\n",
	//	*n,*co,index,num_nums);
	for (int j = index; co[j] != '='; j++)
	{
		//printf("Moving %d to %d\n", n[j+2], n[j+1]);
		//printf("Moving %c to %c\n", co[j+1], co[j]);
		n[j+1] = n[j+2];
		co[j] = co[j+1];
	}
}

bool	compute_div_mul(int *n, char *co, int ix)
{
	if (co[ix] == '=') return true;
	if (co[ix] == '+') return compute_div_mul(n, co, ++ix);
	if (co[ix] == '-') return compute_div_mul(n, co, ++ix);
	if (co[ix] == '*')
	{
		n[ix] = n[ix] * n[ix+1];
		move(n, co, ix);
		//dump();
		compute_div_mul(n, co, ix);
	}
	else if (co[ix] == '/')
	{
		//printf("n[i] : %d   n[i+1] : %d   mod: %d   div: %d\n",
		//	n[ix], n[ix+1], n[ix] % n[ix+1], n[ix] / n[ix+1]);
		if (n[ix+1] == 0) return false;
		if (n[ix] % n[ix+1]) return false;
		n[ix] = n[ix] / n[ix+1];
		move(n, co, ix);
		//dump();
		compute_div_mul(n, co, ix);
	}
	//printf("ERROR!!!!!!!!!!!!!!!!! ix = %d   co = %c\n", ix, co[ix]);
	//	dump();
	return true;
}

bool	compute_add_sub(int *n, char *co, int ix)
{
	if (co[ix] == '=') return true;
	if (co[ix] == '*') return true;
	if (co[ix] == '/') return true;
	if (co[ix] == '+')
	{
		n[ix] = n[ix] + n[ix+1];
		move(n, co, ix);
		//dump();
		compute_add_sub(n, co, ix);
	}
	else if (co[ix] == '-')
	{
		n[ix] = n[ix] - n[ix+1];
		move(n, co, ix);
		//dump();
		compute_add_sub(n, co, ix);
	}
	return true;
}
	
bool	compute_value(int *n, char *co, int &ix)
{
	bool rv = compute_div_mul(n, co, 0);
	//printf("Before add-sub (rv:%d)", rv);
	//dump();
	if (rv) rv = compute_add_sub(n, co, 0);
	return rv;
}

bool	get_values(const char *p)
{
	if (end_of_line(*p))
	{
		return true;
	}
	else if (isdigit(*p))
	{
		nums[idx] = 10 * nums[idx] + dig_value(*p);
	}
	else
	{
		ops[idx] = get_op(*p);
		c_ops[idx] = *p;
		idx++;
	}
	return get_values(p+1);
}

bool    has_zero()
{
	for (int i = 0; i < idx; i++)
		if (nums[i] == 0) return true;
	return false;
}

bool	expression(const char *str)
{
	// returns true if str (eight chars) is a valid expression
	
	const char *pc = strdup(str);
	const char *p = pc;
	bool rv;
	get_values(p);
	bool is_zero = has_zero();
	bool equals_zero = (nums[idx] == 0);
	//out_exp(__LINE__, nums, c_ops, idx);
	if (!compute_value(nums, c_ops, idx)) rv = false;
	else rv = (nums[0] == nums[1]);
	//printf("VALID: %s  %d  =?= %d \n", (rv?"TRUE":"FALSE"), nums[0], nums[1]);
	clear();
	free((void *)pc);
	if (rv) {
		printf("%s", str);
		valid_solutions++;
		if (!is_zero) {
			if (!equals_zero) no_zero_solutions++;
			non_zero_solutions++;
			printf("  *");
		}
		printf("\n");
	}
	return rv;
}

const char digits[]={'0', '1','2','3','4','5','6','7','8','9', 0};
const char digops[]={'0', '1','2','3','4','5','6','7','8','9',
						'+', '-','*', '/', 0};
const char operators[]={'+', '-','*', '/', 0};
const char nzdigits[]={'1','2','3','4','5','6','7','8','9', 0};
const int  pten[]={0, 10, 100, 1000, 10000, 1000000};

void	getnextdig(char *exp, int ix)
{
	//printf("ix = %d :: %s\n", ix, exp);
	if (exp[ix] == '=')
	{
		//printf("--- %s\n", exp);
		if ( expression(exp) )
		clear();
		return;
	}
	if (ix == 0)
	{
		for(int i = 1; i <= 9; i++)
		{
			exp[ix] = i + '0';
			getnextdig(exp, 1);
		}
	}
	else if (exp[ix+1] == '=')
	{
		for(int i = 0; i <= 9; i++)
		{
			exp[ix] = i + '0';
			getnextdig(exp, ix+1);
		}		
	}
	else if (isdigit(exp[ix-1]))
	{
		for(int i = 0; digops[i]; i++)
		{
			exp[ix] = digops[i];
			getnextdig(exp, ix+1);
		}
	}
	else // is +, -, /, or * 
	{
		for(int i = 1; i <= 9; i++)
		{
			exp[ix] = i + '0';
			getnextdig(exp, ix+1);
		}
	}
}

void test1()
{
	expression("1+1/2=10");
	expression("8+1+8=17");
	expression("8*1*8=63");
	expression("12+34=46");
	expression("64-29=35");
	expression("5*3-1=14");
	expression("15-3*4=3");
	expression("16/3+4=9");
	expression("9+9/3=12");
	expression("9+9/2=12");
	expression("8*1*8=64");
	exit(1);
}

int main(int argc, char **argv)
{
	char	exp[10];
	//test1();
	exp[8] = 0;
 	for (int eq = 4; eq <= 6; eq++)
	{
		strcpy(exp, "        ");
		exp[eq] = '=';
		for (int ans = pten[6-eq]; ans < pten[7-eq]; ans++)
		{
			sprintf(exp+eq+1,"%d", ans);
			{
				getnextdig(exp, 0);
			}
		}
	}
	printf("\n\nNon-zero solutions: %d\n", non_zero_solutions);
	printf("NO-zero solutions: %d\n", no_zero_solutions);
	printf("   Total solutions:%d\n", valid_solutions);
}
