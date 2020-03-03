var a = 1;
a = a + 1;
a;

var b = a + 2;
b += 1;
b;

var fn = func (n) {
    var r = 0;
    var i = 0;
    for (i = 0; i < n; i += 1) {
        r += i;
    }
    return r;
}

var r = fn(10);
print(r);

var fn2 = func() {
    print(a);
}

var r2 = fn2();
print(r2);
