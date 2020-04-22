{"a": 1, "b": "c"}["b"];

[1, 2, 3, 4][1:3];
[1, 2, 3, 4][func(a,b){return a + b;}(0, 1) : 3];

var a = true && false;
a;

val d = 1 + 2 * (4 + 5) - 8;
#d = d + 1;
#d;

var fn = func(a, b) {
    val c = a + b;
    return c;
}
fn(7, 3);

func(c, d, e) {
    val x = c * d;
    var y = e(x, 2);
    return y;
}(1, 2, fn);
