width: 1200px;
height: 480px;

plot {
  x: csv('tests/testdata/simple.csv', var1);
  y: csv('tests/testdata/simple.csv', var3);

  layer {
    type: lines;
  }
}
