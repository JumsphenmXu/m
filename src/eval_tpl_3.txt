var a = {"a": "1", "b": "2", "c": 1};

var b = {"x": 1, "y": 2};

b["z"] = a;
print(a);
print(type(a));
b;
print(type(b));

a["a"];
b["z"]["a"];
b["z"]["a"] = [1, 2, 3, 4];
print(type(b["z"]["a"]));
print(len(a));
b["z"]["a"][1];
b["z"]["a"][1] = 10000;
b["z"]["a"][1] = {"a": 1, "b": 2};
b["z"]["a"][1];
b["z"]["b"];
b["z"]["c"];
