var a = 1, b = 2;
c = 3;
return (a => b => c => a+b+c)(a)(b)(c);