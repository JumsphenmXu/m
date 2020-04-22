var a = [1, 2, 3, 4];
a = append(a, 5);
print(a);

del(a, 0);
print(a);

var b = {"a": 1, "v": "vi"};
print(b);
del(b, "a");
print(b);

# define array
var arr = [1, 'a', 'b', "a string here"];
# set the first element of array to map
arr[0] = {'a': "char", 1: "int"};
print(arr[0]['a']); # eval to "char"
# add key-value pair to map
arr[0]["new key"] = "new val";

# append new element to array
arr = append(arr, "new element");
print(arr);

# get array length
print(len(arr));
# get map length
print(len(arr[0]));

# delete element
del(arr[0], "new key");
print(arr);
del(arr, 0);
print(arr);
