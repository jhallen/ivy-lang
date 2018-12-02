
fn af(&f) fn((n),if(n==0,1,n*(*f)(n-1)))

fn Y(f) f(Y(f))

fact=Y(af)

print fact(5)
