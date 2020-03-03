var fn = func(a, b, c) {
    var d = a + b * c;
    if (d < a) {
        d = a;
    } elif (d < b) {
        d = b;
    } else {
        d = c;
    }

    return d;
}

print(fn(1, 2, 3)); # eval to 3

# use function as parameters
var fn2 = func(a, b, c) {
    var x = a + b;
    x = c(x, b, 1);
    return x;
}
fn2(1, 2, fn);

var sum = func(n) {
    var result = 0;
    var i = 0;
    for (i = 0; i < n; i += 1) {
        result += i;
    }
    return result;
}
print(sum(100));

# array and map operation
var arr = [1, 'a', 'b', "a string here"];
arr[0] = {'a': "char", 1: "int"};
print(arr[0]); # eval to "char"
print(arr[0]['a']); # eval to "char"
arr[0]["new key"] = "new val";
