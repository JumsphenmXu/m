func (x, y, z) {
    val r = x + y + z;
    if (r > x && r > y || x < z && y < z) {
        r = 1;
    }
    return r;
}
